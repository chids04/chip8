#include "cpu.hpp"
#include <algorithm>
#include <unordered_map>

namespace Chip8
{
void Cpu::handleKey(SDL_Event *e){
    SDL_assert(e->type == SDL_EVENT_KEY_DOWN);

   static const std::unordered_map<SDL_Scancode, int> keyMap = {
        {SDL_SCANCODE_1, 0},
        {SDL_SCANCODE_2, 1},
        {SDL_SCANCODE_3, 2},
        {SDL_SCANCODE_4, 3},
        {SDL_SCANCODE_Q, 4},
        {SDL_SCANCODE_W, 5},
        {SDL_SCANCODE_E, 6},
        {SDL_SCANCODE_R, 7},
        {SDL_SCANCODE_A, 8},
        {SDL_SCANCODE_S, 9},
        {SDL_SCANCODE_D, 10},
        {SDL_SCANCODE_F, 11},
        {SDL_SCANCODE_Z, 12},
        {SDL_SCANCODE_X, 13},
        {SDL_SCANCODE_C, 14},
        {SDL_SCANCODE_V, 15}
    };

    auto it = keyMap.find(e->key.scancode);
    if (it != keyMap.end()) {
        key[it->second] = 1;
    } 

}

void Cpu::clearKeys(){
    std::fill(std::begin(key), std::end(key), 0);
}
}