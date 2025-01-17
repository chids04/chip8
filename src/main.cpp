#define SDL_MAIN_USE_CALLBACKS 1

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <filesystem>

#include "cpu.hpp"

namespace {
    struct AppState 
    {
        AppState() : cpu("../roms/2-ibm-logo.ch8") {}
        Chip8::Cpu cpu;

    };
}

extern "C"{

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    std::cout << "current working dir "  << std::filesystem::current_path() << std::endl;
    std::cout << "sdl window init.." << std::endl;
    SDL_SetAppMetadata("a chip8 emu", "0.1", "com.chids.chip8");

    if(!SDL_Init(SDL_INIT_VIDEO)){
        std::cerr << "couldn't init sdl: " << SDL_GetError();
        return SDL_APP_FAILURE;
    }

    AppState *state = new AppState;

    if(!SDL_CreateWindowAndRenderer("chip8", 640, 320, 0, &state->cpu.window, &(state->cpu.renderer))){
        std::cerr << "error creating window and/or renderer: " << SDL_GetError();
        return SDL_APP_FAILURE;
    }

    //add to screen if path is incorrect;

    *appstate = state;

    return SDL_APP_CONTINUE;

}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *state = static_cast<AppState*>(appstate);
    state->cpu.emulatecycle();

    state->cpu.updateScreen();
    SDL_RenderPresent(state->cpu.renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppState *state = static_cast<AppState*>(appstate);
    
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    if(event->type == SDL_EVENT_KEY_DOWN){
        state->cpu.handleKey(event);
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    //cleanup here
    /* SDL will clean up the window/renderer for us. */
}

}

// int main(int argc, char *argv[]){

//     Chip8::Cpu cpu(argv[1]);
//     if(!cpu.initWin()){
//         std::cerr << "Failed to initialise SDL Window";
//         return 1;
//     }

//     SDL_Event e;

//     bool quit = false;
//     bool keyPressed = false;

//     while(!quit){
//         while(SDL_PollEvent(&e)){
//             if(e.type == SDL_QUIT){
//                 quit = true;
//             }

//             if(e.type == SDL_EVENT_KEY_DOWN){
//                 cpu.handleKey(&e);
//                 keyPressed = true;
//             }
//         }
    
//         // Present the renderer
//         cpu.emulatecycle();

//         SDL_RenderClear(cpu.renderer);
//         cpu.updateScreen();
//         SDL_RenderPresent(cpu.renderer); 


//         if(keyPressed){
//             cpu.clearKeys();
//             keyPressed = false;
//         }
        
//     }
    
    
//     return 0;
// }
