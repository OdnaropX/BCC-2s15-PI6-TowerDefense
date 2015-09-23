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

#include "Estruturas.h"

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
	screen current_screen = MAIN;
    tab current_tab = TOP_MENU;
    SDL_Event event;
    
    //Main loop
    while(!quit){
        //Event Handler
        ///////////////////////////////////////////////////
		while(SDL_PollEvent(&event) != 0){
			switch (current_screen){
				//Main screen event
				case MAIN:
					//Main screen events handled
					switch (event.type) {
						//Quit
						case SDL_QUIT:
							quit = true;
							break;
						//Escape key
						case SDLK_ESCAPE:
							quit = true;
							break;
						//Keypad enter
						case SDLK_KP_ENTER:
							//Check current selected option
							
							break;
						//Keyboard enter
						case SDLK_RETURN:
							//Check current selected option and initiated it.
						
							break
						case SDLK_UP:
							//Change current selected option
							break;
						
						case SDLK_LEFT:
							//Change current selected option
						
							break;
						case SDLK_RIGHT:
							//Change current selected option
						
							break;
						
						case SDLK_DOWN:
							//Change current selected option
							
							break;
					}
					
					break;
				case CONFIG:
					switch (event.type) {
						//Quit
						case SDL_QUIT:
							quit = true;
							break;
						//Escape
						case SDLK_ESCAPE:
							//Go back main screen
							
							break;	
						//Keypad enter
						case SDLK_KP_ENTER:
							//Check current selected option
							
							break;
						//Keyboard enter
						case SDLK_RETURN:
							//Check current selected option and initiated it.
						
							break
						case SDLK_UP:
							//Change current selected option
							break;
						
						case SDLK_LEFT:
							//Change current selected option
						
							break;
						case SDLK_RIGHT:
							//Change current selected option
						
							break;
						
						case SDLK_DOWN:
							//Change current selected option
							
							break;
					}
					break;
				case GAME_RUNNING:
					switch (event.type) {
						//Quit
						case SDL_QUIT:
							quit = true;
							break;
						//Escape
						case SDLK_ESCAPE:
							//Show Game Pause screen with options
							
							break;	
						//Keypad enter
						case SDLK_KP_ENTER:
							//Check current action selected from menu and initiated it.
							
							break;
						//Keyboard enter
						case SDLK_RETURN:
							//Check current action selected from menu and initiated it.
						
							break;
						case SDLK_TAB:
							//Move selector location
							current_tab = (current_tab + 1) % 4;
							
							break;
						case SDLK_UP:
							//Move selected selector. Selector is choose with tab.
							switch(current_tab) {
								case TOP_MENU:
									//Move up top menu
									
									break;
								
								case LEFT_MENU:
									//Move up left menu
									
									break;
								
								case BOTTOM_MENU:
									//Move up bottom menu
									
									break;
								
								case GAME_AREA:
									//Move up mouse cursor from GAME_AREA
									
									break;
							}
							break;
						
						case SDLK_LEFT:
							//Move selected selector. Selector is choose with tab.
							switch(current_tab) {
								case TOP_MENU:
									//Move left top menu
									
									break;
								
								case LEFT_MENU:
									//Move left left menu
									
									break;
								
								case BOTTOM_MENU:
									//Move left bottom menu
									
									break;
								
								case GAME_AREA:
									//Move left mouse cursor from GAME_AREA
									
									break;
							}
							break;
						case SDLK_RIGHT:
							//Move selected selector. Selector is choose with tab.
							switch(current_tab) {
								case TOP_MENU:
									//Move right top menu
									
									break;
								
								case LEFT_MENU:
									//Move right left menu
									
									break;
								
								case BOTTOM_MENU:
									//Move right bottom menu
									
									break;
								
								case GAME_AREA:
									//Move right mouse cursor from GAME_AREA
									
									break;
							}
							break;
						
						case SDLK_DOWN:
							//Move selected selector. Selector is choose with tab.
							switch(current_tab) {
								case TOP_MENU:
									//Move down top menu
									
									break;
								
								case LEFT_MENU:
									//Move down left menu
									
									break;
								
								case BOTTOM_MENU:
									//Move down bottom menu
									
									break;
								
								case GAME_AREA:
									//Move down mouse cursor from GAME_AREA
									
									break;
							}
							break;
					break;
				
				case GAME_PAUSED:
					switch (event.type) {
						//Quit
						case SDL_QUIT:
							quit = true;
							break;
						//Escape
						case SDLK_ESCAPE:
							//Return to play the game
							
							
							break;	
						//Keypad enter
						case SDLK_KP_ENTER:
							//Check current action selected from menu paused and initiated it.
							
							break;
						//Keyboard enter
						case SDLK_RETURN:
							//Check current action selected from menu paused and initiated it.
							
							break;
						case SDLK_UP:
							//Move to option above
							break;
						case SDLK_LEFT:
							//Move to option left 
						
							break;
						case SDLK_RIGHT:
							//Move to option right
						
							break;
							
						case SDLK_DOWN:
							//Move to option bellow
							
							break;
						
						//Handle mouse event
						case SDL_MOUSEBUTTONUP:
							if(event.button.button == SDL_BUTTON_LEFT){
								//Check if location selected is a valid one
							}
							break;
							
							
					}
			}
        }
        
		//Action Performancer
		/////////////////////////////////////////////////////
		switch(current_screen) {
			case MAIN:
			
				break;
			case CONFIG:
				break;
			
			case GAME_RUNNING:
				break;
			
			case GAME_PAUSED:
				break;
				
		}
		
		//Scene Renderer 
		/////////////////////////////////////////////////////
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