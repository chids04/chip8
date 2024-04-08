#ifndef CPU_H
#define CPU_H

#include <string>
#include <cstdint>
#include <random>
#include <chrono>
#include <SDL2/SDL.h>

#define NIB_MASK(n) (0xF << ((n * 4))) 
#define GET_FIRST_NIB (NIB_MASK(3))
#define GET_SECOND_NIB (NIB_MASK(2))
#define GET_THIRD_NIB (NIB_MASK(1))
#define GET_FOURTH_NIB (NIB_MASK(0))
#define GET_SEC_BYTE (0x00FF)
#define GET_12_BIT (0x0FFF)

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320


class Chip8 {

public:
    bool df;
    SDL_Window *window;
    SDL_Renderer *renderer;

    Chip8(char *path);
    void loadGame(char *path);
    void emulatecycle();
    void handleKey(SDL_Event *e);
    void clearKeys();
    bool initWin();

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

    std::mt19937 gen;    
    uint8_t getRand();

    std::chrono::high_resolution_clock::time_point lastDelayTick;
    std::chrono::high_resolution_clock::time_point lastSoundTick;
    std::chrono::high_resolution_clock::time_point currentTime;
    bool delayEnded;
    bool soundEnded;

};

#endif 
