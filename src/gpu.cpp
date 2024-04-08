#include "cpu.hpp"
#include <iostream>

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
