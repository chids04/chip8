#include <iostream>
#include "cpu.hpp"

int main(int argc, char *argv[]){

    Chip8 cpu(argv[1]);
    cpu.emulatecycle();
    
    return 0;
}
