#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include "cpu.hpp"

void Chip8::loadGame(char *path){
    
    std::ifstream game(path, std::ios::binary);

    if(!game.is_open()){
        std::cerr << "invalid file path";
    }
    
    //sets pointer to end of file, computes file size and goes back to the beginning
    game.seekg(0, std::ios::end);
    std::streampos gameSize = game.tellg();
    game.seekg(0, std::ios::beg);
    
    char *buf = new char[static_cast<size_t>(gameSize)];

    game.read(buf, gameSize);
    game.close();

    for(int i=0; i<gameSize && (i + 512) < 4096; i++){
        memory[512 + i] = buf[i];
    }

    delete[] buf;

    //initalise random variable
}

Chip8::Chip8(char *path){
    //initializing registers

    sp = 0, opcode = 0, pc = 0, delayTimer = 0, soundTimer = 0, df = false;

    pc = 0x200; //program starts at this address

    delayEnded = true;
    soundEnded = true;
   
    std::fill(memory, memory + sizeof(memory), 0);
    std::fill(V, V + sizeof(V), 0);
    std::fill(stack, stack + sizeof(stack), 0);

    //setting up the input keys and loading into memory
    for(uint8_t i = 0x00; i <= 0xF; i++){
        key[i] = i;
    }

    uint8_t fonts[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    
    //need to copy the contents of this array into chip8 memory
    std::copy(fonts, fonts+80, &memory[0x050]);

    loadGame(path);

    gen.seed(std::random_device()());

     
}

uint8_t Chip8::getRand(){
    std::uniform_int_distribution<uint8_t> distribution(0,255);
    return distribution(gen);
}

void Chip8::emulatecycle(){
    //fetch
    opcode = memory[pc] << 8 | memory[pc+1]; //combining into one 16bit instruction
    pc += 2;
    df = false; //setting draw flag to false

    long long diff;
    currentTime = std::chrono::high_resolution_clock::now();
    
    if(delayTimer > 0){
        diff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime-lastDelayTick).count();

        if(diff >= 16.7){
            delayTimer -= 1;
            lastDelayTick = std::chrono::high_resolution_clock::now();
        } 
    }

    if(soundTimer > 0){
        diff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastSoundTick).count();
        std::cout << "BEEP\n";

        if(diff >= 16.7){
            soundTimer -= 1;
            lastSoundTick = std::chrono::high_resolution_clock::now();
        }
    }

    std::cout << "##################################" << "\n";
    std::cout << "register vals before instruction execution" << "\n";
    std::cout << "sp: " << std::hex << static_cast<int>(sp) << "\n";
    std::cout << "pc: " << static_cast<int>(pc) << "\n";
    std::cout << "dt: " << static_cast<int>(delayTimer) << "\n";
    std::cout << "st: " << static_cast<int>(soundTimer) << "\n";
    std::cout << "df: " << static_cast<int>(df) << "\n\n";

    std::cout << std::hex << "opcode: " << static_cast<int>(opcode) << "\n";

    //decode
    uint8_t instruction = (opcode & GET_FIRST_NIB) >> 12;

    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    uint8_t y = (opcode & GET_THIRD_NIB) >> 4;
    uint8_t z = (opcode & GET_FOURTH_NIB);
    uint8_t kk = (opcode & GET_SEC_BYTE);
    uint16_t nnn = (opcode & GET_12_BIT);

    std::cout <<

        std::hex <<
        "MSB: " <<
        static_cast<int>(instruction) << "\n" <<

        "second nib: " <<
        static_cast<int>(x) << "\n" <<

        "third nib: " <<
        static_cast<int>(y) << "\n" <<

        "fourth nib: " <<
        static_cast<int>(z) << "\n" <<

        "lower byte: " <<
        static_cast<int>(kk) << "\n" <<

        "lower 12 bits: " <<
        static_cast<int>(nnn) << "\n\n";
 
    switch(instruction){
        case 0x0: //two instructions have this code
            if(z == 0x0){
                //clear the screen (CLS)
                std::cout << "instruction: 00E0\n";

                std::fill(std::begin(gfx), std::end(gfx), 0); 
                df = true;
            }
            else if(z == 0xE){
                //return from a subroutine (RET)
                std::cout << "instruction 00EE\n";

                pc = stack[sp];
                sp--;
            }
            break;
       
        case 0x1:
            //jump to memory address at mem location NNN
            std::cout << "instruction 1nnn\n";

            pc = nnn; 
            break;

        case 0x2:
            //call subroutine at mem location NNN
            std::cout << "instruction 2nnn\n";
            
            sp++;
            stack[sp] = pc;
            pc = memory[nnn];
            break;

        case 0x3:
            //skip next instruction if Vx == kk
            std::cout << "instruction 3xkkk\n";

            if(V[x] == (kk)){
                pc+=1; 
            }
            break;

        case 0x4:
            //skip next instruction if Vx != kk
            std::cout << "instruction 4xkk\n";
            if(V[x] != (kk)){
                pc+=2; 
            } 
            break;

        case 0x5:
            //skip next instruction if Vx == Vy
            std::cout << "instruction 5xy0\n";

            if(V[x] == V[y]){
                pc+=2;
            }
            break;
        
        case 0x6:
            //value kk put into Vx
            std::cout << "instruction 6xkk\n";

            V[x] = kk;
            break;

        case 0x7:
            //add kk to reg Vx and store result in reg Vx
            std::cout << "instruction 7xkk\n";
            
            V[x] += kk;
            break;

        case 0x8:
            //multiple opcodes start with this nibble so i check the last nibble
            switch(z){
                case 0x0:
                    //sets Vx = Vy
                    std::cout << "instruction 8xy0\n";

                    V[x] = V[y];
                    break;
                
                case 0x1:
                    //Vx = Vx OR Vy;
                    std::cout << "instruction 8xy1\n";

                    V[x] = V[x] | V[y];
                    break;                
                
                case 0x2:
                    //Vx = Vx AND Vy
                    std::cout << "instruction 8xy2";

                    V[x] = V[x] & V[y];
                    break;

                case 0x3:
                    //Vx = Vx XOR Vy
                    std::cout << "instruction 8xy3";

                    V[x] = V[x] ^ V[y];
                    break; 

                case 0x4:
                    //add Vx and Vy, if result > 8 bits, the set carry flag. then store lower 8 bits of number
                    std::cout << "instruction 8xy4\n";

                    {
                        uint16_t result = V[x] + V[y];
                        
                        if(result > 255){
                            V[0xF] = 1;
                        }
                        else{
                            V[0xF] = 0;
                        }
                        
                        
                        //store lowest 8 bits
                        V[x] = result & GET_SEC_BYTE;
                        break;
                    }

                case 0x5:
                    // minus Vx and Vy, if Vx > Vy then set carry register, else set to 0
                    std::cout << "instruction 8xy5\n";
                    
                    {
                        if(V[x] >= V[y]){
                            V[0xF] = 1;
                        }
                        else{
                            V[0xF] = 0;
                        }

                        V[x] = V[x] - V[y];
                    }
                    break;

                case 0x6:
                    //Vx = Vx SHR 1
                    std::cout << "instruction 8xy6\n";

                    if((V[x] & 0b00000001) == 1 ){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    
                    V[x] = V[x] / 2;
                    break;

                case 0x7:
                    //set VF if Vx > Vy, then Vx = Vx - Vy
                    std::cout << "instruction 8xy7\n";

                    if(V[y] > V[x]){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    
                    V[x] -= V[y];
                    break;
                
                case 0xE:
                    //set VF if MSB of Vx is 1 then multiple Vx by 2
                    std::cout << "instruction 8xyE\n";

                    if((V[x] & 0b10000000) != 0){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                   }

                    V[x] = V[x] * 2; 
                    break;

            }

            break;

        case 0x9:
            //skip next instruction if Vx != Vy
            std::cout << "instruction 9xy0\n";

            if(V[x] != V[y]){
                pc += 2;
            }
            
            break;

        case 0xA:
            //set I to nnn
            std::cout << "instruction Annn\n";

            I = opcode & GET_12_BIT;
            break;

        case 0xB:
            //jump to location nnn + V0
            std::cout << "instruction Bnnn\n";

            pc = V[0x0] + nnn;
            break;
            
        case 0xC:
            //set Vx = randomNum & kk
            std::cout << "instruction Cxkk\n";

        {
            uint8_t result = getRand() & kk;
            V[x] = result;
        }        
            break;

        case 0xD:
            //display sprite at coords (Vx, Vy) and set VF if collision
            std::cout << "instruction Dxyn\n";

            {
                uint8_t pixel;
                uint8_t xcoord = x;
                uint8_t ycord = y;
                V[0xF] = 0;

                for(uint8_t i=0; i<z; i++){
                    pixel = memory[I+i];
                    
                    for(int j=0; j<8; j++){
                        if((pixel & (0xF >> j)) != 0){
                            if(gfx[j + xcoord + (i + ycord) * 64] == 1){
                                V[0xF]  = 1;
                            }
                            gfx[i + xcoord + (j + ycord) * 64] ^= 1;
                        }
                    }
                    
                }
            }
            
            break;
        
        case 0xE:
            switch(z){
                case 0xE:
                    //skip next instruction if key[Vx] is pressed
                    std::cout << "instruction Ex9E\n";

                    if(key[V[x]] == 1){
                        pc += 2;
                    } 
                    break;
                
                case 0x1:
                    //skip next instruction if key[Vx] is not pressed
                    std::cout << "instruction ExA1\n";

                    if(key[V[x]] == 0){
                        pc += 2;
                    }
                    break;
            }
            break;
        
        case 0xF:
            switch(kk){
                case 0x07:
                    //Vx = delay timer
                    std::cout << "instruction Fx07\n";
                    V[x] = delayTimer;
                    break;
                
                case 0x0A:
                    //wait for key press and store value of key into V[x]
                    std::cout << "instruction Fx0A\n";

                    {
                        bool pressed = false;

                        for(uint8_t i=0; i<16; i++){
                            if(key[i] == 1){
                                pressed = true;
                                V[x] = key[i];
                                break;
                            }
                        }

                        if(pressed == false){
                            pc -= 2;
                        }

                        break;
                    }
                
                case 0x15:
                    //delay timer = Vx
                    std::cout << "instruction Fx15\n";

                    delayTimer = V[x];
                    break;
                
                case 0x18:
                    //sound timer = Vx
                    std::cout << "instruction Fx18\n";

                    soundTimer = V[x];
                    break;
                
                case 0x1E:
                    //set I = I + Vx
                    std::cout << "instruction Fx1E\n";

                    I += V[x];
                    break;
                
                case 0x29:
                    //set I to sprite location of value Vx
                    std::cout << "instruction Fx29\n";

                    I = gfx[V[x]];
                    break;
                
                case 0x33:
                    //store digits of Vx in mem locations I, I+1, I+2
                    std::cout << "instruction Fx33\n";

                    {
                        uint8_t hundreds = V[x] / 100;
                        uint8_t tens = (V[x] / 10) % 10;
                        uint8_t ones = V[x] % 10;

                        memory[I] = hundreds;
                        memory[I+1] = tens;
                        memory[I+2] = ones;
                    }

                    break;
                
                case 0x55:
                    //store reg V0 through to Vx into memory starting at location I
                    std::cout << "instruction Fx55\n";

                    for(int i=0; i<=x; i++){
                        memory[I+i] = V[i];
                    }

                    break;
                
                case 0x65:
                    //read registers V0 through Vx from memory starting at location I
                    std::cout << "instruction Fx65\n";

                    for(int i=0; i<=x; i++){
                        V[i] = memory[I+i];
                    }

                    break;
            }
    }
    
    std::cout << "\nregister vals after instruction execution\n";
    std::cout << "sp: " << std::hex << static_cast<int>(sp) << "\n";
    std::cout << "pc: " << static_cast<int>(pc) << "\n";
    std::cout << "dt: " << static_cast<int>(delayTimer) << "\n";
    std::cout << "st: " << static_cast<int>(soundTimer) << "\n";
    std::cout << "df: " << static_cast<int>(df) << "\n";
       
    std::cout << "##################################" << "\n\n";

}























