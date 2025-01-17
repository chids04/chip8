#include <iostream>
#include <iomanip>
#include "cpu.hpp"

namespace Chip8{

void Cpu::updateScreen(){
    Pixel p;

    int pixW = SCREEN_WIDTH / 64;
    int pixH = SCREEN_HEIGHT / 32;

    p.r.w = pixW;
    p.r.h = pixH;

    p.r.x = 0;
    p.r.y = 0; 

    p.colours[0] = 0;
    p.colours[1] = 0;
    p.colours[2] = 0;
    p.colours[3] = 255;

    int xpos = 0;
    int ypos = 0;

    for(int i=0; i<(64*32) - 1; i++){
        if(gfx[i] == 1){
            p.colours[0] = 255;
            p.colours[1] = 255;
            p.colours[2] = 255;
            
        }else{
            p.colours[0] = 0;
            p.colours[1] = 0;
            p.colours[2] = 0;
        }
        SDL_SetRenderDrawColor(renderer, p.colours[0], p.colours[1], p.colours[2], SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &p.r);

        xpos += pixW;

        if(xpos == SCREEN_WIDTH) {
            xpos = 0;
            ypos += pixH;
        }

        p.r.x = xpos;
        p.r.y = ypos;
    }
}
}