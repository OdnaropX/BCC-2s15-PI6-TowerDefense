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
#include "Renderer.h"
#include "GameScene.h"

//SDL stuff
SDL_Window *main_Window;
SDL_Surface *main_Surface;
SDL_Renderer *renderer;

//Media
TTF_Font *title_Font;
TTF_Font *desc_Font;
TTF_Font *credits_Font;
SDL_Texture *title_Texture;
SDL_Texture *desc_Texture;
SDL_Texture *credits_Texture;

SDL_Surface *map_Surface;

//Constants
SDL_Color black = {0, 0, 0, 255};
SDL_Color white = {0, 0, 0, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color red = {255, 0, 0, 255};

SDL_Rect title_Rect = {265, 0, 750, 150};
SDL_Rect desc_Rect = {440, 375, 400, 50};
SDL_Rect credits_Rect = {530, 690, 750, 30};

CONFIGURATION *config;

list_minion *minions;
list_projectile *projectiles;
list_turret *turrets;

bool main_init();
void main_quit();

int main(int argc, const char * argv[]) {
    //Main init
    if(!main_init()){
        main_quit();
    }
    
    bool quit = false;
	screen current_screen = MAIN;
	screen previous_screen = MAIN;
	main_options main_option = OPT_NONE;
	main_options select_option = OPT_PLAY;
    config_options config_option = NONE;
	config_options select_config_option = MUSIC_EFFECT;
    tab current_tab = TOP_MENU;
    SDL_Event event;
    
	bool game_started = false;
	bool game_paused = false;
	bool clicked = false;
    int game_is_active = 1;
    int lifes = 1;
    int gold = 0;
    
    //FPS
    int t1, t2;
    int delay = 17; //Aprox. de 1000ms/60
    t1 = SDL_GetTicks();
    
    // Following parts are only for first interation;
    int monsterSpawner[] = {6, 8, 12, 14, 17, 18, 25, 16, 18, 50};
	
    //Main loop
    while(!quit){
        //FPS Handling
        t2 = SDL_GetTicks() - t1;
        if(t2 < delay)
            SDL_Delay(delay - t2);
        
        t1 = SDL_GetTicks();

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
							if (select_option != OPT_NONE){
								main_option = select_option;
							}
							break;
						//Keyboard enter
						case SDLK_RETURN:
							//Check current selected option and initiated it.
							if (select_option != OPT_NONE){
								main_option = select_option;
							}
                            break;
						case SDLK_UP:
							//Change current selected option
							if (select_option == OPT_PLAY) {
								select_option = OPT_CREDIT;
							}
							else {
								select_option--;
							}
							break;
						
						case SDLK_LEFT:
							//Change current selected option
							select_option = (select_option + 1) % 5;
							break;
						case SDLK_RIGHT:
							//Change current selected option
							if (select_option == OPT_PLAY) {
								select_option = OPT_CREDIT;
							}
							else {
								select_option--;
							}
							break;
						
						case SDLK_DOWN:
							//Change current selected option
							select_option = (select_option + 1) % 5;
							break;
						
						//Handle mouse event
						case SDL_MOUSEBUTTONUP:
							clicked = false;
							main_option = OPT_NONE;
										
							if(event.button.button == SDL_BUTTON_LEFT){
								//Check if location selected is a valid one
                                
								if (event.motion.x >= 980 && event.motion.x <= 980 + BUTTON_MENU_WIDTH) {//Near main config
									if (event.motion.y >= 480 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT) {//First option PLAY
										clicked = true;
										main_option = OPT_PLAY;
									}
									else if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 2) {
										clicked = true;
										main_option = OPT_CONFIG;
									}
									else if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 3) {
										clicked = true;
										main_option = OPT_SCORE;
									}
									else if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 4) {
										clicked = true;
										main_option = OPT_EXIT;
										
									}
									
								}
								else if (event.motion.x >= 30 && event.motion.x <= 30 + BUTTON_MENU_WIDTH / 2) {
								//Near credits 
									if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 4) {
										clicked = true;
										main_option = OPT_CREDIT;
									}
								}
							}
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
							//Go back main screen or paused screem
							if (previous_screen == MAIN){
								current_screen = MAIN;
							}
							else {
								current_screen = GAME_PAUSED;
							}
							break;	
						//Keypad enter
						case SDLK_KP_ENTER:
							//Check current selected option
							switch(select_config_option){
								case BACK:
									if (previous_screen == MAIN){
										current_screen = MAIN;
									}
									else {
										current_screen = GAME_PAUSED;
									}
									break;
								case MUSIC_EFFECT:
									config_option = MUSIC_EFFECT;
									break;
								case MUSIC_AMBIENCE:
									config_option = MUSIC_AMBIENCE;
									break;
								case LANGUAGE:
									config_option = LANGUAGE;
									break;
								case NONE:
									//do nothing
									break;
							}
							break;
						//Keyboard enter
						case SDLK_RETURN:
							//Check current selected option and initiated it.
							switch(select_config_option){
								case BACK:
									if (previous_screen == MAIN){
										current_screen = MAIN;
									}
									else {
										current_screen = GAME_PAUSED;
									}
									break;
								case MUSIC_EFFECT:
									config_option = MUSIC_EFFECT;
									break;
								case MUSIC_AMBIENCE:
									config_option = MUSIC_AMBIENCE;
									break;
								case LANGUAGE:
									config_option = LANGUAGE;
									break;
								case NONE:
									//do nothing
									break;
							}
							break;
						case SDLK_UP:
							//Change current selected option
							if (select_config_option == MUSIC_EFFECT) {
								select_config_option = BACK;
							}
							else {
								select_config_option--;
							}
							break;
						
						case SDLK_LEFT:
							//Change current selected option
							if (select_config_option == LANGUAGE) {
							
							}
							else {
								config_option = select_config_option;
							}
							break;
						case SDLK_RIGHT:
							//Change current selected option
							if (select_config_option == LANGUAGE) {
							
							}
							else {
								config_option = select_config_option;
							}
							break;
						
						case SDLK_DOWN:
							//Change current selected option
							select_config_option = (select_config_option + 1) % 4;
							break;
							
						//Handle mouse event
						case SDL_MOUSEBUTTONUP:
							clicked = false;
							config_option = NONE;
										
							if(event.button.button == SDL_BUTTON_LEFT){							
                                //Check if location selected is a valid one
								if (event.motion.x >= 400 && event.motion.x <= 400 + BUTTON_MENU_WIDTH ) {//Near main config
									if (event.motion.y >= 150 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT) {//First option
										//Trogle option
										clicked = true;
										config_option = MUSIC_EFFECT;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 2) {
										clicked = true;
										config_option = MUSIC_AMBIENCE;
										
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 3) {
										clicked = true;
										config_option = LANGUAGE;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 4) {
										clicked = true;
										config_option = BACK;
									}
								}
								else if (event.motion.x >= 420 + BUTTON_MENU_WIDTH &&  event.motion.x >= 420 + BUTTON_MENU_WIDTH + BUTTON_MENU_WIDTH / 2){
									if (event.motion.y >= 150 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT) {//First option
										//Trogle option
										clicked = true;
										config_option = MUSIC_EFFECT;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 2) {
										clicked = true;
										config_option = MUSIC_AMBIENCE;
										
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 3) {
										clicked = true;
										config_option = LANGUAGE;
									}
								}
							}
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
        }
        
		//Action Performancer
		/////////////////////////////////////////////////////
		switch(current_screen) {
			case MAIN:
				//if option
				switch(main_option){
					case OPT_PLAY:
						game_started = true;
						//Generate Map and resources to use on Game_Running
					
					
						//Change current screen.
						current_screen = GAME_RUNNING;
					case OPT_EXIT:
						quit = true;
						break;
					case OPT_CONFIG:
						current_screen = CONFIG;
						previous_screen = MAIN;
						break;
					case OPT_SCORE:
						//Load SCORE data from someplace in time
						
						
						current_screen = SCORE;
						break;
					case OPT_CREDIT:
						//Load Credits from file or use hardcoded.

						current_screen = CREDITS;
					case OPT_NONE:
						//Do nothing.
						break;
				}
				
				break;
			case CONFIG:
				//Set selected option to show on MAIN to OPT_PLAY
				select_option = OPT_PLAY;
				switch(config_option) {
					case MUSIC_EFFECT:
						//Trogle efffect muisc
						if (config->music_effect == true) {
							config->music_effect = false;
						}
						else {
							config->music_effect = true;
						}
						break;
					case MUSIC_AMBIENCE:
						//Trogle ambience music
						if (config->music_ambience == true) {
							config->music_ambience = false;
						}
						else {
							config->music_ambience = true;
						}
						break;
					case LANGUAGE:
						//Change select language
						
						break;
						
					case BACK:
						//Check if the previous_screen is GAME_PAUSED or MAIN.
						if (previous_screen == MAIN) {
							current_screen = MAIN;
						}
						else if (previous_screen == GAME_PAUSED){
							current_screen = GAME_PAUSED;
						}
						break;
					case NONE:
						//Do nothing.
						break;
				}
				break;
			
            /*
			case GAME_RUNNING:
				switch(game_option) {
					if (clicked){
						
						
						
					}
					
				}
            break;
			
			case GAME_PAUSED:
				//Set selected option to show on MAIN to OPT_PLAY
				select_option = OPT_PLAY;
				if(clicked) {
					
				}
			
				switch(pause_option) {
					case RESUME:
						current_screen = GAME_RUNNING;
						
						break;
						
					case EXIT:
						quit = true;
						break;
					case MAIN:
						current_screen = MAIN;
						game_started = false;
						game_paused = false;
					case CONFIG:
						current_screen = CONFIG;
						previous_screen = GAME_PAUSED;
					case NONE:
						//Do nothing
						break;
				}
            break;
            */
		}
		
		//Scene Renderer 
		/////////////////////////////////////////////////////
        switch (current_screen) {
            case CONFIG:
                draw_screen_config(main_Surface);
                break;
            
            case CREDITS:
                break;
                
            case GAME_PAUSED:
                draw_screen_game_paused(main_Surface);
                break;
                
            case GAME_RUNNING:
                draw_screen_game_running(main_Surface, map_Surface, minions, projectiles, turrets);
                break;
                
            case MAIN:
                draw_screen_main(main_Surface);
                break;
                
            case SCORE:
                break;
                
            default:
                break;
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
    main_quit();
    return 0;
}

//Init SDL, configs e menu principal
bool main_init(){
    //Read Settings
    FILE *settings = fopen("Config.txt", "r");
    if(!settings){
        printf("Config file not found!\n");
        return false;
    }
    
    int screen_Width, screen_Height;
	char *music_effect, *music_ambience, *language;
	
    fscanf(settings, "w = %d\n", &screen_Width);
    fscanf(settings, "h = %d\n", &screen_Height);
    /*
    fscanf(settings, "music_effect = %s\n", music_effect);
    fscanf(settings, "music_ambiance = %s\n", music_ambience);
    fscanf(settings, "language = %s\n", language);
     */
	
	if(!config){
		config = malloc(sizeof(CONFIGURATION));
	}
	
    /*
	if (strcmp(music_effect, "true")){
		config->music_effect = true;
	}
	else {
		config->music_effect = false;
	}
	if (strcmp(music_ambience, "true")){
		config->music_ambience = true;
	}
	else {
		config->music_ambience = false;
	}
	config->language = language;
    */
    
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

void main_quit(){
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
	
	//Free config
	free(config->language);
	free(config);
}