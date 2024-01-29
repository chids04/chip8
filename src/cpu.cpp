#include <fstream>
#include <iostream>
#include <vector>
#include "cpu.hpp"

void Chip8::loadGame(char *path){
    
    std::ifstream game(path, std::ios::binary);

    if(!game.is_open()){
        std::cerr << "invalid file path";
    }
    
    //sets pointer to end of file, computes file size and goes back to the beginning
    game.seekg(0, std::ios::end);
    std::streampos gameSize = game.tellg();
    char *buff = new char[gameSize];

    //data casted as char* so it can be read
    game.seekg(0, std::ios::beg);
    game.read(reinterpret_cast<char*>(&memory[0x200]), gameSize);
    game.close();

}

void Chip8::loadFonts(){

}
