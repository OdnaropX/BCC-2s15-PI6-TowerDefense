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
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

SDL_Window *main_Window = NULL;
SDL_Surface *main_Surface = NULL;
SDL_Renderer *renderer = NULL;

TTF_Font *title_Font = NULL;
TTF_Font *desc_Font = NULL;
TTF_Font *credits_Font = NULL;
SDL_Texture *title_Texture = NULL;
SDL_Texture *desc_Texture = NULL;
SDL_Texture *credits_Texture = NULL;

SDL_Color black = {0, 0, 0, 255};
SDL_Rect title_Rect = {265, 0, 750, 150};
SDL_Rect desc_Rect = {440, 375, 400, 50};
SDL_Rect credits_Rect = {530, 690, 750, 30};

bool main_Init();
void main_Quit();

int main(int argc, const char * argv[]) {
    //Main init
    if(!main_Init()){
        main_Quit();
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
        
        //Clear render
        SDL_RenderClear(renderer);
        
        //Draw textures
        SDL_RenderCopy(renderer, title_Texture, NULL, &title_Rect);
        SDL_RenderCopy(renderer, desc_Texture, NULL, &desc_Rect);
        SDL_RenderCopy(renderer, credits_Texture, NULL, &credits_Rect);
        
        //Update render
        SDL_RenderPresent(renderer);
    }
    
    //Quit
    main_Quit();
    return 0;
}

//Init SDL, configs e menu principal
bool main_Init(){
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
    main_Window = SDL_CreateWindow("PI-6 Tower Defense", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_Width, screen_Height, SDL_WINDOW_SHOWN);
    if(!main_Window){
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        return false;
    }
    
    //Init renderer
    renderer = SDL_CreateRenderer(main_Window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    //Init TTF
    if(TTF_Init() == -1){
        printf("TTF init error: %s\n", TTF_GetError());
    }
    
    //Title text
    title_Font = TTF_OpenFont("../fonts/8bitOperatorPlus-Regular.ttf", 60);
    if(!title_Font){
        printf("Title font not loaded! %s\n", TTF_GetError());
        return false;
    }
    
    SDL_Surface *title_Surface = TTF_RenderText_Solid(title_Font, "PI-6 Tower Defense", black);
    
    if(!title_Surface){
        printf("Title Text Surface not rendered! %s\n", TTF_GetError());
        return false;
    }
    
    title_Texture = SDL_CreateTextureFromSurface(renderer, title_Surface);
    
    if(!title_Texture){
        printf("Title text texture not created! %s\n", SDL_GetError());
        return false;
    }
    
    SDL_FreeSurface(title_Surface);
    
    //Desc text
    desc_Font = TTF_OpenFont("../fonts/8bitOperatorPlus-Regular.ttf", 36);
    if(!desc_Font){
        printf("Desc font not loaded! %s\n", TTF_GetError());
        return false;
    }
    
    SDL_Surface *desc_Surface = TTF_RenderText_Solid(desc_Font, "Click to start game", black);
    
    if(!desc_Surface){
        printf("Desc Text Surface not rendered! %s\n", TTF_GetError());
        return false;
    }
    
    desc_Texture = SDL_CreateTextureFromSurface(renderer, desc_Surface);
    
    if(!desc_Texture){
        printf("Desc text texture not created! %s\n", SDL_GetError());
        return false;
    }
    
    SDL_FreeSurface(desc_Surface);
    
    //Credits text
    credits_Font = TTF_OpenFont("../fonts/8bitOperatorPlus-Regular.ttf", 18);
    if(!credits_Font){
        printf("Credits font not loaded! %s\n", TTF_GetError());
        return false;
    }
    
    SDL_Surface *credits_Surface = TTF_RenderText_Solid(credits_Font, "Made by: Danilo Ikuta, Gabriel Fontenelle and Gabriel Nopper", black);
    
    if(!credits_Surface){
        printf("Credits Text Surface not rendered! %s\n", TTF_GetError());
        return false;
    }
    
    credits_Texture = SDL_CreateTextureFromSurface(renderer, credits_Surface);
    
    if(!credits_Texture){
        printf("Credits text texture not created! %s\n", SDL_GetError());
        return false;
    }
    
    SDL_FreeSurface(credits_Surface);
    
    return true;
}

void main_Quit(){
    //Close fonts
    TTF_CloseFont(title_Font);
    TTF_CloseFont(desc_Font);
    TTF_CloseFont(credits_Font);
    
    //Destroy textures
    SDL_DestroyTexture(title_Texture);
    SDL_DestroyTexture(desc_Texture);
    SDL_DestroyTexture(credits_Texture);
    
    //Free window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(main_Window);
    
    //Quit SDL, TTF, IMG
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}