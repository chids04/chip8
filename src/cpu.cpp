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

    I = 0x200; //program starts at this address
   
    std::cout << sizeof(memory);
    //std::fill_n(memory, sizeof(memory) / sizeof(uint8_t), 0);
}
