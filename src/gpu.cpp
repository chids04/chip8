#include <iostream>
#include "cpu.hpp"

bool Chip8::initWin(){
    
    bool success = true;
    
    if(SDL_Init(SDL_INIT_VIDEO) > 0){
        success = false;
        std::cerr << "Error initalising SDL";
    }
    else{
        window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if(window == NULL){
            success = false;
            std::cerr << "Error initialising SDL window";
        }
        else{
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            if(renderer == NULL){
                success = false;
                std::cerr << "Failed to initialise renderer";
            }
        
        }
    }

    
    return success;
}

void Chip8::updateScreen(){
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
        SDL_SetRenderDrawColor(renderer, p.colours[0], p.colours[1], p.colours[2], p.colours[3]);
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
