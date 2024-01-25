#include <climits>
#include <string>

class Chip8 {

public:
    unsigned short df;
    void loadGame(std::string str);

private:
    unsigned char sp;
    unsigned short opcode;
    unsigned short I;
    unsigned short pc; 
    unsigned short delayTimeTimeer;
    unsigned short soundTimer;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short stack[16];
    unsigned short gfx[64*32];
};
