//
//  main.c
//  PI6 - TowerDefense
//
//  Created by Danilo Makoto Ikuta on 10/09/15.
//  Copyright (c) 2015 Danilo Makoto Ikuta. All rights reserved.
//

//Main Menu (Loads SDL and main menu assets)

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

bool main_Init(SDL_Window **window, SDL_Surface **screenSurface);
void main_Quit(SDL_Window **window);

int main(int argc, const char * argv[]) {
    SDL_Window *main_Window = NULL;
    SDL_Surface *main_Surface = NULL;
    
    //Main init
    if(!main_Init(&main_Window, &main_Surface)){
        main_Quit(&main_Window);
    }
    
    bool quit = false;
    
    SDL_Event event;
    
    //Main loop
    while(!quit){
        //Event Handler
        while(SDL_PollEvent(&event) != 0){
            switch (event.type) {
                //Quit
                case SDL_QUIT:
                    quit = true;
                    break;
                
                //Left Click(play game)
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_LEFT)
                        printf("play game!\n");
                    break;
                    
                default:
                    break;
            }
        }
        
        //Fill screen_Surface
        SDL_FillRect(main_Surface, NULL, 0xFFFFFF);
        
        //Update Surface
        SDL_UpdateWindowSurface(main_Window);
    }
    
    //Quit
    main_Quit(&main_Window);
    return 0;
}

//Init SDL, configs e menu principal
bool main_Init(SDL_Window **window, SDL_Surface **screen_Surface){
    //Read Settings
    FILE *settings = fopen("Config.txt", "r");
    if(!settings){
        printf("Config file not found!\n");
        return false;
    }
    
    int screen_Width, screen_Height;
    fscanf(settings, "w = %d\n", &screen_Width);
    fscanf(settings, "h = %d\n", &screen_Height);
    
    fclose(settings);
    
    //Init SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL_Init error: %s\n", SDL_GetError());
        return false;
    }
    
    //Create window
    *window = SDL_CreateWindow("PI-6 Tower Defense", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_Width, screen_Height, SDL_WINDOW_SHOWN);
    if(!window){
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        return false;
    }
    
    //Set
    *screen_Surface = SDL_GetWindowSurface(*window);
    
    return true;
}

void main_Quit(SDL_Window **window){
    //Free window
    SDL_DestroyWindow(*window);
    
    //Quit SDL
    SDL_Quit();
}