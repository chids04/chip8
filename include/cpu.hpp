#include <string>
#include <cstdint>

class Chip8 {

public:
    int df;
    Chip8();
    void loadGame(char *path);
    void emulatecycle();

private:
    uint8_t sp;
    uint16_t opcode;
    uint16_t I;
    uint16_t pc; 
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint8_t key[16];
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t stack[16];
    uint32_t gfx[64*32];
    uint8_t fonts[80];
};
