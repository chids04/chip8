#include <iostream>
#include "cpu.hpp"

int main(int argc, char *argv[]){

    Chip8 cpu(argv[1]);
    if(!cpu.initWin()){
        std::cerr << "Failed to initialise SDL Window";
        return 1;
    }

    SDL_Event e;

    bool quit = false;
    bool keyPressed = false;

    while(!quit){
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                quit = true;
            }

            if(e.type == SDL_KEYDOWN){
                cpu.handleKey(&e);
                keyPressed = true;
            }
        }
    
        // Present the renderer
        cpu.emulatecycle();

        SDL_RenderClear(cpu.renderer);
        cpu.updateScreen();
        SDL_RenderPresent(cpu.renderer); 


        if(keyPressed){
            cpu.clearKeys();
            keyPressed = false;
        }
        
    }
    
    
    return 0;
}
