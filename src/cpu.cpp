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

Chip8::Chip8(){
    //initializing registers

    sp = 0, opcode = 0, pc = 0, delayTimer = 0, soundTimer = 0;

    pc = 0x200; //program starts at this address
   
    std::cout << sizeof(memory);

    std::fill_n(memory, sizeof(memory),0);
    std::cout << memory[3000];

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
    
}
