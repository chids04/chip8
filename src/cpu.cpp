#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
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

    //memory array casted as char* so it can be read into
    game.read(reinterpret_cast<char*>(&memory[0x200]), gameSize);
    game.close();

}

Chip8::Chip8(char *path){
    //initializing registers

    sp = 0, opcode = 0, pc = 0, delayTimer = 0, soundTimer = 0;

    pc = 0x200; //program starts at this address
   
    std::cout << sizeof(memory);

    std::fill_n(memory, sizeof(memory),0);
    std::cout << memory[3000] << "\n";

    std::fill_n(V, sizeof(V), 0);
    std::fill_n(stack, sizeof(stack), 0);

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
     
}

void Chip8::emulatecycle(){
    
    //fetch
    opcode = memory[pc] << 8 | memory[pc+1]; //combining into one 16bit instruction
    pc += 2;

    std::cout << std::hex << static_cast<int>(opcode) << "\n";

    //decode
    uint8_t instruction = (opcode & GET_FIRST_NIB) >> 12;

    std::cout <<

        std::hex <<
        "instruction: " <<
        static_cast<int>(instruction) << "\n" <<
        "second nib: " <<
        static_cast<int>((opcode & GET_SECOND_NIB) >> 8) << "\n" <<
        "third nib: " <<
        static_cast<int>((opcode & GET_THIRD_NIB) >> 4) << "\n" <<
        "fourth nib: " <<
        static_cast<int>((opcode & GET_FOURTH_NIB)) << "\n"; 
        
    
    switch(instruction){
        case 0x0: //two instructions have this code
            if((opcode & GET_FOURTH_NIB) == 0xE){
                //clear the screen (CLS)
                std::fill(std::begin(gfx), std::end(gfx), 0); 
            }
            else{
                //return from a subroutine (RET)
                pc = stack[sp];
                sp--;
            }
            
            break;
       
        case 0x1:
            //jump to memory address at mem location NNN
            pc = memory[opcode & GET_12_BIT];
            break;

        case 0x2:
            //call subroutine at mem location NNN
            sp++;
            stack[sp] = pc;
            pc = memory[opcode & GET_12_BIT];
            break;

        case 0x7:
            //add kk to reg Vx and store result in reg Vx
            V[opcode & GET_SECOND_NIB] += opcode & GET_SEC_BYTE;
            std::cout << static_cast<int>(V[opcode & GET_SECOND_NIB]) << "\n"; 
    }

}
























