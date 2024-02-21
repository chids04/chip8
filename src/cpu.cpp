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
    
    char *buf = new char[gameSize];

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
    pc += 1;
    df = false; //setting draw flag to false

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
        static_cast<int>(nnn) << "\n";
 
    switch(instruction){
        case 0x0: //two instructions have this code
            if((z) == 0xE){
                //clear the screen (CLS)
                std::fill(std::begin(gfx), std::end(gfx), 0); 
                df = true;
            }
            else{
                //return from a subroutine (RET)
                pc = stack[sp];
                sp--;
            }
            
            break;
       
        case 0x1:
            //jump to memory address at mem location NNN
            pc = nnn; 
            break;

        case 0x2:
            //call subroutine at mem location NNN
            sp++;
            stack[sp] = pc;
            pc = memory[nnn];
            break;

        case 0x3:
            //skip next instruction if Vx == kk
            if(V[x] == (kk)){
                pc+=1; 
            }
            break;

        case 0x4:
            //skip next instruction if Vx != kk
            if(V[x] != (kk)){
                pc+=1; 
            } 
            break;

        case 0x5:
            //skip next instruction if Vx == Vy
            if(V[x] == V[y]){
                pc+=1;
            }
            break;
        
        case 0x6:
            //value kk put into Vx
            V[x] = kk;
            break;

        case 0x7:
            //add kk to reg Vx and store result in reg Vx
            V[x] += kk;
            std::cout << std::hex << V[x] << "\n";
            break;

        case 0x8:
            //multiple opcodes start with this nibble so i check the last nibble
            switch(z){
                case 0x0:
                    //sets Vx = Vy
                    V[x] = V[y];
                    break;
                
                case 0x1:
                    V[x] = V[x] | V[y];
                    break;                
                
                case 0x2:
                    V[x] = V[x] & V[y];
                    break;

                case 0x3:
                    V[x] = V[x] ^ V[y];
                    break; 

                case 0x4:
                    //add Vx and Vy, if result > 8 bits, the set carry flag. then store lower 8 bits of number
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
                    if((V[x] & 0b0000000000000001) == 1 ){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    
                    V[x] = V[x] / 2;
                    break;

                case 0x7:
                    if(V[y] > V[x]){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    
                    V[x] -= V[y];
                    break;
                
                case 0xE:
                    if(((V[x] & 0b1000000000000000) >> 15) == 1){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }

                    V[x] *= V[x] * 2; 
                    break;

            }

            break;

        case 0x9:
            if(V[x] != V[y]){
                pc += 1;
            }
            
            break;

        case 0xA:
            I = opcode & GET_THIRD_NIB;
            break;

        case 0xB:
            pc = V[0x0] + nnn;
            break;
            
        case 0xC:
        {
            uint8_t result = getRand() & kk;
            V[x] = result;
        }        
            break;

        case 0xD:
            uint8_t pixel;
            uint8_t xcoord = x;
            uint8_t ycord = y;

            for(uint8_t i=0; i<z; i++){
                pixel = memory[I+i];
                
                for(int j=0; j<8; i++){
                    if((pixel & (0xF >> i)) != 0){
                        if(gfx[i + xcoord + (j + ycord) * 64]){

                        }
                    }
                }
                
            }
    }
        

}























