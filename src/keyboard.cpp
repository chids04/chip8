#include "cpu.hpp"
#include <algorithm>

void Chip8::handleKey(SDL_Event *e){
    switch(e->key.keysym.sym){
        case SDLK_1:
            key[0] = 1;
            break;

        case SDLK_2:
            key[1] = 1;
            break;
        
        case SDLK_3:
            key[2] = 1;
            break;
        
        case SDLK_4:
            key[3] = 1;
            break;
        
        case SDLK_q:
            key[4] = 1;
            break;
        
        case SDLK_w:
            key[5] = 1;
            break;
        
        case SDLK_e:
            key[6] = 1;
            break;

        case SDLK_r:
            key[7] = 1;
            break;
        
        case SDLK_a:
            key[8] = 1;
            break;
        
        case SDLK_s:
            key[9] = 1;
            break;
        
        case SDLK_d:
            key[10] = 1;
            break;
        
        case SDLK_f:
            key[11] = 1;
            break;

        case SDLK_z:
            key[12] = 1;
            break;

        case SDLK_x:
            key[13] = 1;
            break;

        case SDLK_c:
            key[14] = 1;
            break;

        case SDLK_v:
            key[15] = 1;
            break;

    }
}

void Chip8::clearKeys(){
    std::fill(std::begin(key), std::end(key), 0);
}
