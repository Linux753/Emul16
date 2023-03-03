#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "emul.h"


int initScreenSDL(struct Screen * screen){ //Done
    if(0!= SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS)){
        fprintf(stderr, "Error SDL_init: %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    screen->window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    if(screen->window == NULL){
        return EXIT_FAILURE;
    }

    screen->renderer = SDL_CreateRenderer(screen->window, -1, SDL_RENDERER_ACCELERATED);
    if(screen->renderer == NULL){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int SetRenderPaletteColor(SDL_Renderer * renderer, struct Screen * screen, uint8_t color){ //Done
    return SDL_SetRenderDrawColor(renderer, screen->palette[color].r, screen->palette[color].g, screen->palette[color].b, screen->palette[color].a);
}

void renderScreen(struct Screen * screen){ //Done
    if(0!=SetRenderPaletteColor(screen->renderer, screen, screen->bg)){
        fprintf(stderr, "Error while clearing the renderer : SDL_RenderClear : %s", SDL_GetError());

    }
    if(0 != SDL_RenderClear(screen->renderer)){
        fprintf(stderr, "Error while clearing the renderer : SDL_RenderClear : %s", SDL_GetError());
    }
    SDL_Rect pixel = {0, 0, 0, 0};
    for(int i=0; i<PIXEL_BY_WIDTH; i++){
        for(int j=0; j<PIXEL_BY_HEIGHT; j++){
            if(screen->fg[i][j] != 0x00){
                if(0!=SetRenderPaletteColor(screen->renderer, screen, screen->fg[i][j])){
                    fprintf(stderr, "Error while clearing the renderer : SDL_RenderClear : %s", SDL_GetError());
                }
                pixel.x = i*screen->pixelWidth;
                pixel.y = j*screen->pixelHeight;
                pixel.h = screen->pixelHeight;
                pixel.w = screen->pixelWidth;
                if(0!= SDL_RenderFillRect(screen->renderer, &pixel)){
                    fprintf(stderr, "Error while drawing pixel SDL_RenderFillRect() : %s", SDL_GetError());
                }           
            }
        }
    }
    screen->reg.vblank = true;
    SDL_RenderPresent(screen->renderer);
}

void destroyScreen(struct Screen * screen){ //Done
    if(screen->window != NULL){
        SDL_DestroyWindow(screen->window);
    }
    if(screen->renderer != NULL){
        SDL_DestroyRenderer(screen->renderer);
    }
    SDL_Quit();
}

void updateEvent(struct Control * control){ //Update the key table and other relevent event variable by scraping the event that hapenned
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_KEYUP:
                control->key[event.key.keysym.scancode] = SDL_FALSE;
                break;
            case SDL_KEYDOWN:
                control->key[event.key.keysym.scancode] = SDL_TRUE;
                break;
            case SDL_QUIT:
                control->quit = SDL_TRUE;
                break;
            case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_RESIZED){
                    control->resizeWindow = SDL_TRUE;
                }
                break;
        }
    }
}
