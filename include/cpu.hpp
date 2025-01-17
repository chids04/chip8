#ifndef CPU_H
#define CPU_H

#include <string>
#include <array>
#include <cstdint>
#include <random>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <functional>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>

namespace Chip8 {

typedef struct {
    SDL_FRect r;
    int colours[4];
} Pixel;

constexpr int NIB_MASK(int n) {
    return 0xF << (n * 4);
}

constexpr int GET_FIRST_NIB = NIB_MASK(3);
constexpr int GET_SECOND_NIB = NIB_MASK(2);
constexpr int GET_THIRD_NIB = NIB_MASK(1);
constexpr int GET_FOURTH_NIB = NIB_MASK(0);
constexpr int GET_SEC_BYTE = 0x00FF;
constexpr int GET_12_BIT = 0x0FFF;

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 320;

class Cpu {
public:
    bool df;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    Cpu(const std::string &path);

    void loadGame(const std::string &path);
    void emulatecycle();
    void handleKey(SDL_Event *e);
    void clearKeys();
    void updateScreen();
    void printRegisters(const std::string &message) const;

private:
    uint8_t sp;
    uint16_t opcode;
    uint16_t I;
    uint16_t pc; 
    uint8_t delayTimer;
    uint8_t soundTimer;
    std::array<uint8_t, 16> key;
    std::array<uint8_t, 4096> memory;
    std::array<uint8_t, 16> V;
    std::array<uint8_t, 16> stack;

    std::array<uint32_t, 64 * 32> gfx;
    std::array<Pixel, 64 * 32> pixelBuffer;

    std::mt19937 gen;
    uint8_t getRand();

    std::chrono::high_resolution_clock::time_point lastDelayTick;
    std::chrono::high_resolution_clock::time_point lastSoundTick;
    std::chrono::high_resolution_clock::time_point currentTime;
    bool delayEnded;
    bool soundEnded;


    std::unordered_map<uint8_t, std::function<void()>> opcodeTable; // Jump table

    // Initialize the opcode jump table
    void initializeOpcodeTable();

    // Opcode handling functions
    void handle0x0();
    void handle0x1();
    void handle0x2();
    void handle0x3();
    void handle0x4();
    void handle0x5();
    void handle0x6();
    void handle0x7();
    void handle0x8();
    void handle0x9();
    void handle0xA();
    void handle0xB();
    void handle0xC();
    void handle0xD();
    void handle0xE();
    void handle0xF();
};

} // namespace Chip8

#endif
