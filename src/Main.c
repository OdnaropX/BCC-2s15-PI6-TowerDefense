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
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

#include "Estruturas.h"
#include "Renderer.h"
#include "GameScene.h"

#define main_menu_text_count 6
#define config_menu_text_count 5
#define FRAMES_PER_SEC 60

//SDL stuff
SDL_Window *main_Window;
SDL_Surface *main_Surface;
SDL_Renderer *renderer;

//Media
TTF_Font *font;
SDL_Surface *map_Surface;

//Constants
SDL_Color black = {0, 0, 0, 255};
SDL_Color white = {0, 0, 0, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color red = {255, 0, 0, 255};

//Static texts
//Main menu order: title, play, config, score, credits, exit
SDL_Texture *main_menu_texts[main_menu_text_count];
SDL_Rect main_menu_rects[main_menu_text_count];

//Config order: Config, sfx, music, language, back
SDL_Texture *config_menu_texts[config_menu_text_count];
SDL_Rect config_menu_rects[config_menu_text_count];

CONFIGURATION *config;

list_minion *minions;
list_projectile *projectiles;
list_turret *turrets;

bool main_init();
void main_quit();

void get_config();

int main(int argc, const char * argv[]) {
    //Main init
    if(!main_init()){
        main_quit();
    }
    
    bool quit = false;
	
	//Screen control
	screen current_screen = MAIN;
	screen previous_screen = MAIN;
	
	//Options control
	main_options main_option = OPT_NONE;
    config_options config_option = NONE;
	pause_options pause_option = OPT_P_NONE;
    tab current_tab = TOP_MENU;
	running_top_options running_top_option = OPT_R_T_NONE;
	running_left_options running_left_option = OPT_R_L_NONE;
	running_area_right_options running_area_right_option = OPT_R_A_R_NONE;
	running_area_left_options running_area_left_option = OPT_R_A_L_NONE;
	
	//Keyboard options control
	main_options select_option = OPT_PLAY;
	config_options select_config_option = AUDIO_SFX;
	pause_options select_pause_option = OPT_P_NONE;
    running_top_options select_running_top_option = OPT_R_T_NONE;
	running_left_options select_running_left_option = OPT_R_L_NONE;
	running_area_right_options select_running_area_right_option = OPT_R_A_R_NONE;
	running_area_left_options select_running_area_left_option = OPT_R_A_L_NONE;
	
	SDL_Event event;
    
	bool game_started = false;
	bool game_paused = false;
	bool active_clicked = false;
	
	//Game area control
    bool selected_left = false;//Right click equals to tower and left to minions.
	int add_tower = 0;
	int add_minion = 0;
    int game_is_active = 1;
    int lifes = 5;
    int gold = 0;
	
	//Display control
	bool show_gold_info = false;
	bool show_mana_info = false;
	bool show_life_info = false;
	
	//FPS and timer
    int t1, t2;
    int delay = 17; //Aprox. de 1000ms/60
	int show_timer = 0;
	int frame = 0;
    t1 = SDL_GetTicks();
    
	int select_grid = 0;
	int max_grid = 17 * 13;
	
    // Following parts are only for first interation;
    int monsterSpawner[] = {6, 8, 12, 14, 17, 18, 25, 16, 18, 50};
	//Wave 20 seconds, after 
	
	int grid_clicked[] = {0,0};
	
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
            printf("");
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
							main_option = OPT_NONE;
										
							if(event.button.button == SDL_BUTTON_LEFT){
								//Check if location selected is a valid one
                                
								if (event.motion.x >= 980 && event.motion.x <= 980 + BUTTON_MENU_WIDTH) {//Near main config
									if (event.motion.y >= 480 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT) {//First option PLAY
										main_option = OPT_PLAY;
                                        printf("Play\n");
									}
									else if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 2) {
										main_option = OPT_CONFIG;
                                        printf("Config\n");
									}
									else if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 3) {
										main_option = OPT_SCORE;
                                        printf("Score\n");
									}
									else if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 4) {
										main_option = OPT_EXIT;
                                        quit = true;
                                        printf("exit");
									}
									
								}
								else if (event.motion.x >= 30 && event.motion.x <= 30 + BUTTON_MENU_WIDTH / 2) {
								//Near credits 
									if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 4) {
										main_option = OPT_CREDIT;
									}
								}
							}
							break;
						
						case SDL_MOUSEMOTION:
							//Change select_option with mouse over data.
							if (event.motion.x >= 980 && event.motion.x <= 980 + BUTTON_MENU_WIDTH) {//Near main config
								if (event.motion.y >= 480 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT) {//First option PLAY
									select_option = OPT_PLAY;
								}
								else if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 2) {
									select_option = OPT_CONFIG;
								}
								else if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 3) {
									select_option = OPT_SCORE;
                                }
								else if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 4) {
									select_option = OPT_EXIT;
								}
							}
							else if (event.motion.x >= 30 && event.motion.x <= 30 + BUTTON_MENU_WIDTH / 2) {
							//Near credits 
								if(event.motion.y >= 480 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 480 + BUTTON_MENU_HEIGHT * 4) {
									select_option = OPT_CREDIT;
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
								case AUDIO_SFX:
									config_option = AUDIO_SFX;
									break;
								case AUDIO_MUSIC:
									config_option = AUDIO_MUSIC;
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
								case AUDIO_SFX:
									config_option = AUDIO_SFX;
									break;
								case AUDIO_MUSIC:
									config_option = AUDIO_MUSIC;
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
							if (select_config_option == AUDIO_SFX) {
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
										config_option = AUDIO_SFX;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 2) {
										config_option = AUDIO_MUSIC;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 3) {
										config_option = LANGUAGE;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 4) {
										config_option = BACK;
									}
								}
								else if (event.motion.x >= 420 + BUTTON_MENU_WIDTH &&  event.motion.x >= 420 + BUTTON_MENU_WIDTH + BUTTON_MENU_WIDTH / 2){
									if (event.motion.y >= 150 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT) {//First option
										//Trogle option
<<<<<<< HEAD
										config_option = MUSIC_EFFECT;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 2) {
										config_option = MUSIC_AMBIENCE;
=======
										clicked = true;
										config_option = AUDIO_SFX;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 2) {
										clicked = true;
										config_option = AUDIO_MUSIC;
>>>>>>> 1e901bcd3acdcb913632293f459f30129c55a915
										
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 3) {
										config_option = LANGUAGE;
									}
								}
							}
							break;
						case SDL_MOUSEMOTION:
							if (event.motion.x >= 400 && event.motion.x <= 400 + BUTTON_MENU_WIDTH ) {//Near main config
									if (event.motion.y >= 150 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT) {//First option
										select_config_option = MUSIC_EFFECT;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 2) {
										select_config_option = MUSIC_AMBIENCE;									
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 3) {
										select_config_option = LANGUAGE;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 4) {
										select_config_option = BACK;
									}
								}
								else if (event.motion.x >= 420 + BUTTON_MENU_WIDTH &&  event.motion.x >= 420 + BUTTON_MENU_WIDTH + BUTTON_MENU_WIDTH / 2){
									if (event.motion.y >= 150 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT) {//First option
										select_config_option = MUSIC_EFFECT;
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 2) {
										select_config_option = MUSIC_AMBIENCE;
										
									}
									else if(event.motion.y >= 150 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 150 + BUTTON_MENU_HEIGHT * 3) {
										select_config_option = LANGUAGE;
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
							current_screen = GAME_PAUSED;
							select_pause_option = OPT_P_RESUME;
							break;	
						case SDLK_q:
							if (current_tab == GAME_AREA && !active_clicked){
								//Active click on current location
								active_clicked = true;
								selected_left = true;
							}
						case SDLK_e:
							if (current_tab == GAME_AREA && !active_clicked){
								//Active click on current location
								active_clicked = true;
								selected_left = false;
							}
						
						//Mouse motion
						case SDL_MOUSEMOTION:
							//Move mouse selector over game area
							if (!active_clicked && get_touched_grid_address(event.motion.x, event.motion.y, grid_clicked)){
								select_grid = get_grid_address_linear(grid_clicked[0], grid_clicked[1], 16);
							}
							break;
						
						//Keypad enter
						case SDLK_KP_ENTER:
							//Check current action selected from menu and initiated it.
							switch(current_tab) {
								case TOP_MENU:
									switch(select_running_top_option){
										case OPT_R_T_PAUSE:
											//Dont do any action after this, only render the scene. 
											current_screen = GAME_PAUSED;
											game_paused = true;
											break;
										case OPT_R_T_RESUME:
											current_screen = GAME_RUNNING;
											game_paused = false;
											break;
									}
									break;
								case LEFT_MENU:
									switch(select_running_left_option){
										case OPT_R_L_GOLD:
											show_gold_info = true;
											show_timer = 0;
											break;
										case OPT_R_L_MANA:
											show_mana_info = true;
											show_timer = 0;
											break;
										case OPT_R_L_LIFE:
											show_life_info = true;
											show_timer = 0;
											break;
									}
									break;
								case GAME_AREA:
									//Enter only work on active_clicked.
									if (active_clicked){
										if (selected_left) {
											add_tower = select_running_area_right_option + 1;
										}
										else {
											add_minion = select_running_area_left_option + 1;
										}
									}
									break;
								
							}
							break;
						//Keyboard enter
						case SDLK_RETURN:
							//Check current action selected from menu and initiated it.
							switch(current_tab) {
								case TOP_MENU:
									switch(select_running_top_option){
										case OPT_R_T_PAUSE:
											//Dont do any action after this, only render the scene. 
											current_screen = GAME_PAUSED;
											game_paused = true;
											break;
										case OPT_R_T_RESUME:
											current_screen = GAME_RUNNING;
											game_paused = false;
											break;
									}
									break;
								case LEFT_MENU:
									switch(select_running_left_option){
										case OPT_R_L_GOLD:
											show_gold_info = true;
											show_timer = 0;
											break;
										case OPT_R_L_MANA:
											show_mana_info = true;
											show_timer = 0;
											break;
										case OPT_R_L_LIFE:
											show_life_info = true;
											show_timer = 0;
											break;
									}
									break;
								case GAME_AREA:
									if (active_clicked){
										if (selected_left) {
											add_tower = select_running_area_right_option + 1;
										}
										else {
											add_minion = select_running_area_left_option + 1;
										}
									}
									break;
								
							}
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
									if (select_running_top_option == OPT_R_T_PAUSE) {
										select_running_top_option = OPT_R_T_RESUME;
									}
									else {
										select_running_top_option--;
									}
									break;
								
								case LEFT_MENU:
									//Move up left menu
									if (select_running_left_option == OPT_R_L_GOLD) {
										select_running_left_option = OPT_R_L_LIFE;
									}
									else {
										select_running_left_option--;
									}
									break;
								
								case GAME_AREA:
									//Move up mouse cursor between options 
									if(active_clicked) {//Equals to mouse clicked previously
										if (selected_left) {
											if (select_running_area_left_option == OPT_R_A_L_TOWER_1) {
												select_running_area_left_option = OPT_R_A_L_TOWER_3;
											}
											else {
												select_running_area_left_option--;
											}
										}
										else {
											if (select_running_area_right_option == OPT_R_A_R_MINION_1) {
												select_running_area_right_option = OPT_R_A_R_MINION_3;
											}
											else {
												select_running_area_right_option--;
											}
										}
									}
									else {
									//Move up mouse cursor from GAME_AREA. 
										if (select_grid == 0) {
											select_grid = max_grid - 1;
										}
										else {
											select_grid--;
										}
									}
									break;
							}
							break;
						
						case SDLK_LEFT:
							//Move selected selector. Selector is choose with tab.
							switch(current_tab) {
								case TOP_MENU:
									//Move left top menu - Same as UP because menu is linear.
									if (select_running_top_option == OPT_R_T_PAUSE) {
										select_running_top_option = OPT_R_T_RESUME;
									}
									else {
										select_running_top_option--;
									}
									break;
								
								case LEFT_MENU:
									//Move left left menu
									if (select_running_left_option == OPT_R_L_GOLD) {
										select_running_left_option = OPT_R_L_LIFE;
									}
									else {
										select_running_left_option--;
									}
									break;
								
								case GAME_AREA:
									//Move left mouse cursor from GAME_AREA
									if(active_clicked) {//Equals to mouse clicked previously
										if (selected_left) {
											if (select_running_area_left_option == OPT_R_A_L_TOWER_1) {
												select_running_area_left_option = OPT_R_A_L_TOWER_3;
											}
											else {
												select_running_area_left_option--;
											}
										}
										else {
											if (select_running_area_right_option == OPT_R_A_R_MINION_1) {
												select_running_area_right_option = OPT_R_A_R_MINION_3;
											}
											else {
												select_running_area_right_option--;
											}
										}
									}
									else {
									//Move left mouse cursor from GAME_AREA
										if (select_grid == 0) {
											select_grid = max_grid - 1;
										}
										else {
											select_grid--;
										}
									}
									break;
							}
							break;
                            
						case SDLK_RIGHT:
							//Move selected selector. Selector is choose with tab.
							switch(current_tab) {
								case TOP_MENU:
									//Move right top menu
									select_running_top_option = (select_running_top_option + 1) % 2;
									break;
								
								case LEFT_MENU:
									//Move right left menu
									select_running_left_option = (select_running_left_option + 1) % 3;
									break;

								case GAME_AREA:
									//Move right mouse cursor from GAME_AREA
									if(active_clicked){
										if (selected_left) {
											select_running_area_left_option = (select_running_area_left_option + 1) % 3;
										}
										else {
											select_running_area_right_option = (select_running_area_right_option + 1) % 3;
										}
									}
									else {
									//Move left mouse cursor from GAME_AREA. 
										select_grid = (select_grid + 1) % max_grid;
									}
									break;
							}
							break;
						
						case SDLK_DOWN:
							//Move selected selector. Selector is choose with tab.
							switch(current_tab) {
								case TOP_MENU:
									//Move down top menu
									select_running_top_option = (select_running_top_option + 1) % 2;
									break;
								
								case LEFT_MENU:
									//Move down left menu
									select_running_left_option = (select_running_left_option + 1) % 3;
									break;
								
								case GAME_AREA:
									//Move down mouse cursor from GAME_AREA
									if(active_clicked){
										if (selected_left) {
											select_running_area_left_option = (select_running_area_left_option + 1) % 3;
										}
										else {
											select_running_area_right_option = (select_running_area_right_option + 1) % 3;
										}
									}
									else {
									//Move down mouse cursor from GAME_AREA. 
										select_grid = (select_grid + 1) % max_grid;
									}
									break;
							}
							break;
							
						//Handle mouse event
						case SDL_MOUSEBUTTONUP:
							if(event.button.button == SDL_BUTTON_LEFT){
								//Game area
								if(get_touched_grid_address(event.motion.x, event.motion.y, grid_clicked)){
									if (!active_clicked){
										select_grid = get_grid_address_linear(grid_clicked[0], grid_clicked[1], 16);
										active_clicked = true;
										selected_left = true;
									}
									else {
										//Check where was clicked.
										if (selected_left) {
											add_tower = get_touched_menu_address(grid_clicked[0], grid_clicked[1], grid_clicked, 1);
										}
										else {
											add_minion = get_touched_menu_address(grid_clicked[0], grid_clicked[1], grid_clicked, 0);
										}
									}
								}
								//Top menu
								else if(event.motion.y >= 0 && event.motion.y <= BUTTON_MENU_HEIGHT){
									if (event.motion.x >= BUTTON_MENU_HEIGHT && event.motion.x <= BUTTON_MENU_HEIGHT * 2){
										//Pause game
										game_paused = !game_paused;
										//Not sure if must use current_screen to show the pause screen or just change status to make network request.
										current_screen = GAME_PAUSED;
									}

								}
								else if(event.motion.x >= window_width - 10 - BUTTON_MENU_HEIGHT && event.motion.y <= window_width - 10) {
									if (event.motion.y >= TOP_LAYER_SPACING && event.motion.y <= TOP_LAYER_SPACING + BUTTON_MENU_HEIGHT) {
										//Set selected gold
										show_gold_info = true;
										show_timer = 0;
									}
									else if (event.motion.y >= TOP_LAYER_SPACING + BUTTON_MENU_HEIGHT + 5 && event.motion.y <= TOP_LAYER_SPACING + 5 + BUTTON_MENU_HEIGHT * 2) {
										//Set selected mana
										show_mana_info = true;
										show_timer = 0;
									}
									else if (event.motion.y >= TOP_LAYER_SPACING + BUTTON_MENU_HEIGHT * 2 + 5 && event.motion.y <= TOP_LAYER_SPACING + 5 + BUTTON_MENU_HEIGHT * 3) {
										//Set selected life
										show_life_info = true;
										show_timer = 0;
									}

								}
							}
							//Left click only work on grid and when there is no grid already selected 
							else if (event.button.button == SDL_BUTTON_RIGHT && !active_clicked){
								if (get_touched_grid_address(event.motion.x, event.motion.y, grid_clicked)){
									select_grid = get_grid_address_linear(grid_clicked[0], grid_clicked[1], 16);
									active_clicked = true;
									selected_left = false;
								}
							}
							break;
					}
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
                            current_screen = GAME_RUNNING;
							
							break;	
						//Keypad enter
						case SDLK_KP_ENTER:
							//Check current action selected from menu paused and initiated it.
							switch(select_pause_option){
								case OPT_P_RESUME:
									//Resume game
									current_screen = GAME_RUNNING;
									break;
								case OPT_P_CONFIG:
									current_screen = CONFIG;
									previous_screen = GAME_PAUSED;
									break;
								case OPT_P_SCORE:	
									current_screen = SCORE;
									previous_screen = GAME_PAUSED;
									break;
								case OPT_P_EXIT:
									//End the game
									quit = true;
									break;
								case OPT_P_MAIN:
									current_screen = MAIN;
									break;
								case OPT_P_NONE:
                                    break;
									//Do nothing
							}
							
							break;
						//Keyboard enter
						case SDLK_RETURN:
							//Check current action selected from menu paused and initiated it.
							switch(select_pause_option){
								case OPT_P_RESUME:
									//Resume game
									current_screen = GAME_RUNNING;
									break;
								case OPT_P_CONFIG:
									current_screen = CONFIG;
									previous_screen = GAME_PAUSED;
									break;
								case OPT_P_SCORE:	
									current_screen = SCORE;
									previous_screen = GAME_PAUSED;
									break;
								case OPT_P_EXIT:
									//End the game
									quit = true;
									break;
								case OPT_P_MAIN:
									current_screen = MAIN;
									break;
								case OPT_P_NONE:
									//Do nothing
                                    break;
							}
							break;
						case SDLK_UP:
							//Move to option above
							if (select_pause_option == OPT_P_RESUME) {
								select_pause_option = OPT_P_CREDITS;
							}
							else {
								select_pause_option--;
							}
							break;
						case SDLK_LEFT:
							//Move to option left 
							select_pause_option = (select_pause_option + 1) % 5;
							break;
						case SDLK_RIGHT:
							//Move to option right
							if (select_pause_option == OPT_P_RESUME) {
								select_pause_option = OPT_P_CREDITS;
							}
							else {
								select_pause_option--;
							}
							break;
							
						case SDLK_DOWN:
							//Move to option bellow
							select_pause_option = (select_pause_option + 1) % 5;
							break;
						
						//Handle mouse event
						case SDL_MOUSEBUTTONUP:
							pause_option = OPT_P_NONE;
							
							if(event.button.button == SDL_BUTTON_LEFT){
								//Check if location selected is a valid one
								if (event.motion.x >= 1100 && event.motion.x <= 1100 + BUTTON_MENU_WIDTH) {
									//Check buttons
									if(event.motion.y >= 30 && event.motion.y <= 30 + BUTTON_MENU_HEIGHT){
										pause_option = OPT_P_RESUME;
										printf("Play\n");
									}
									else if (event.motion.y >= 30 + BUTTON_MENU_HEIGHT && event.motion.y <= 30 + BUTTON_MENU_HEIGHT * 2) {
										pause_option = OPT_P_CONFIG;
										printf("Config\n");
									}
									else if (event.motion.y >= 30 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 30 + BUTTON_MENU_HEIGHT * 3) {
										pause_option = OPT_P_SCORE;
										printf("Score\n");
									}
									else if (event.motion.y >= 30 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 30 + BUTTON_MENU_HEIGHT * 4) {
										pause_option = OPT_P_EXIT;
										printf("Exit\n");
									}
									else if (event.motion.y >= 30 + BUTTON_MENU_HEIGHT * 4 && event.motion.y <= 30 + BUTTON_MENU_HEIGHT * 5) {
										pause_option = OPT_P_MAIN;
										printf("Main\n");
									}
									else if (event.motion.y >= 30 + BUTTON_MENU_HEIGHT * 5 && event.motion.y <= 30 + BUTTON_MENU_HEIGHT * 6) {
										pause_option = OPT_P_CREDITS;
										printf("Main\n");
									}
								}
							}
							break;
                        case SDL_MOUSEMOTION:
							if (event.motion.x >= 1100 && event.motion.x <= 1100 + BUTTON_MENU_WIDTH) {
								//Check buttons
								if(event.motion.y >= 30 && event.motion.y <= 30 + BUTTON_MENU_HEIGHT){
									select_pause_option = OPT_P_RESUME;
									}
								else if (event.motion.y >= 30 + BUTTON_MENU_HEIGHT && event.motion.y <= 30 + BUTTON_MENU_HEIGHT * 2) {
									select_pause_option = OPT_P_CONFIG;
									printf("Config\n");
								}
								else if (event.motion.y >= 30 + BUTTON_MENU_HEIGHT * 2 && event.motion.y <= 30 + BUTTON_MENU_HEIGHT * 3) {
									select_pause_option = OPT_P_SCORE;
									printf("Score\n");
								}
								else if (event.motion.y >= 30 + BUTTON_MENU_HEIGHT * 3 && event.motion.y <= 30 + BUTTON_MENU_HEIGHT * 4) {
									select_pause_option = OPT_P_EXIT;
										printf("Exit\n");
								}
								else if (event.motion.y >= 30 + BUTTON_MENU_HEIGHT * 4 && event.motion.y <= 30 + BUTTON_MENU_HEIGHT * 5) {
									select_pause_option = OPT_P_MAIN;
									printf("Main\n");
								}
								else if (event.motion.y >= 30 + BUTTON_MENU_HEIGHT * 5 && event.motion.y <= 30 + BUTTON_MENU_HEIGHT * 6) {
									select_pause_option = OPT_P_CREDITS;
									printf("Main\n");
								}
							}
							break;
						
					}
            }
        }
		
        //Timer handling
		frame++;
		if (frame == FRAMES_PER_SEC) {
			//One second timer
			show_timer++;
			frame = 0;
		}
		
		//Check if display time for menu if over. If it is, dont display info anymore.
		if (current_screen == GAME_RUNNING || current_screen == GAME_PAUSED) {
			if (show_gold_info || show_mana_info || show_life_info){
				if (show_timer >= 60){
					show_gold_info = false;
					show_mana_info = false;
					show_life_info = false;
					show_timer = 0;
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
                        break;
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
						previous_screen = MAIN;
						break;
					case OPT_CREDIT:
						//Load Credits from file or use hardcoded.

						current_screen = CREDITS;
						previous_screen = MAIN;
					case OPT_NONE:
						//Do nothing.
						break;
				}
				
				break;
			case CONFIG:
				//Set selected option to show on MAIN to OPT_PLAY
				select_option = OPT_PLAY;
				switch(config_option){
					case AUDIO_SFX:
						//Trogle efffect muisc
						if (config->audio_sfx == true) {
							config->audio_sfx = false;
						}
						else {
							config->audio_sfx = true;
						}
						break;
					case AUDIO_MUSIC:
						//Trogle ambience music
						if (config->audio_music == true) {
							config->audio_music = false;
						}
						else {
							config->audio_music = true;
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
                
                //Recarrega textos de config
                get_config();
                
				break;
			
            
            case GAME_RUNNING:{
				if (!game_paused){
				
					if (add_tower > 0){
						//Add tower
						
						//Reset tower
						add_tower = 0;
					}
					if (add_minion > 0){
						//Add minion
						
						//Reset minion
						add_minion = 0;
					
					}
				                
                    // DEM ROUTINES, YO!

                    list_minion* enemy = minions;
                    while(enemy){
                        move_minion(enemy->e);
                        list_projectile *shoot = enemy->e->targetted_projectils;
                        while (shoot) {
                            if(move_bullet(enemy->e, shoot->e)){ // The movement is made in the if call.
                                remove_projectile_from_list(enemy->e->targetted_projectils, shoot->e);
                                enemy->e->HP -= shoot->e->damage;
                                list_projectile *temp = shoot;
                                shoot = shoot->next;
                                remove_projectile(temp->e);
                                free_list_projectile(temp);
                            }
                            else
                                shoot = shoot->next;
                        }
                        enemy = enemy->next;
                    }
                    list_turret *turret = turrets;
                    while (turret) {
                        turret->e->timeUntilNextAttack -= 0.017;
                        if(turret->e->timeUntilNextAttack <= 0){
                            enemy = minions;
                            minion *target = NULL;
                            
                            while(enemy && target == NULL){
                                
                                if((target->node.xPos - turret->e->node.xPos) * 2 + (target->node.yPos - turret->e->node.yPos) < 22500) // If within range 150px
                                    if(enemy->e->HP > 0) // If enemy not dead
                                        target = enemy->e;
                                
                                enemy = enemy->next;
                            }
                            if(target){
                                projectile* newShoot = init_projectile(0, turret->e);
                                add_projectile_to_list(target->targetted_projectils, newShoot);
                                turret->e->timeUntilNextAttack = 1.0;
                            }
                        }
                        turret = turret->next;
                    }
                
                }

            }
                
            break;

			case GAME_PAUSED:
				//Set selected option to show on MAIN to OPT_PLAY			
				switch(pause_option){
					case OPT_P_RESUME:
						current_screen = GAME_RUNNING;
						show_timer = 0;
						game_paused = false;
						break;
					case OPT_P_EXIT:
						quit = true;
						break;
					case OPT_P_MAIN:
						current_screen = MAIN;
						game_started = false;
						game_paused = false;
					case OPT_P_CONFIG:
						current_screen = CONFIG;
						previous_screen = GAME_PAUSED;
					case OPT_P_SCORE:
						previous_screen = GAME_PAUSED;
						current_screen = SCORE;
						break;
					case OPT_P_CREDITS:
						previous_screen = GAME_PAUSED;
						current_screen = CREDITS;
						break;
					case OPT_P_NONE:
						//Do nothing
						break;
				}
				break;
            
		}
        
        //Clear render
        SDL_RenderClear(renderer);
		
		//Scene Renderer 
		/////////////////////////////////////////////////////
        switch (current_screen) {
            case CONFIG:
<<<<<<< HEAD
				//select_config_option
				
                draw_screen_config(main_Surface);
=======
                draw_screen_config(renderer, config_menu_texts, config_menu_rects, config_menu_text_count);
>>>>>>> 1e901bcd3acdcb913632293f459f30129c55a915
                break;
            
            case CREDITS:
				
                break;
                
            case GAME_PAUSED:
                //show_gold_info
				//show_mana_info
				//show_life_info 
             
				//select_pause_option
				
				draw_screen_game_paused(main_Surface);
			
				break;
                
            case GAME_RUNNING:
				//active_clicked
				//select_grid
				//selected_left
				
				//show_gold_info
				//show_mana_info
				//show_life_info 
				
				
			
                draw_screen_game_running(main_Surface, map_Surface, minions, projectiles, turrets);
                break;
                
            case MAIN:
				//select_option
				
                draw_screen_main(renderer, main_menu_texts, main_menu_rects, main_menu_text_count);
                break;
                
            case SCORE:
                break;
                
            default:
                break;
        }
        
        if(current_screen != MAIN && current_screen != CONFIG){
            SDL_Texture *surfaces = SDL_CreateTextureFromSurface(renderer, main_Surface);
            SDL_RenderCopy(renderer, surfaces, NULL, &(SDL_Rect){0, 0, 1280, 720});
        }
        
        //Update render and surfaces
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
	char language[6], audio_sfx[6], audio_music[6];
	
    fscanf(settings, "w = %d\n", &screen_Width);
    fscanf(settings, "h = %d\n", &screen_Height);
    
    fscanf(settings, "audio_sfx = %s\n", audio_sfx);
    fscanf(settings, "audio_music = %s\n", audio_music);
    fscanf(settings, "language = %s\n", language);
	
	if(!config){
		config = malloc(sizeof(CONFIGURATION));
	}
	
	if (strcmp(audio_sfx, "true")){
		config->audio_sfx = true;
	}
	else {
		config->audio_sfx = false;
	}
	if (strcmp(audio_music, "true")){
		config->audio_music = true;
	}
	else {
		config->audio_music = false;
	}
	config->language = language;
    
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
    
    //Set main_Surface
    main_Surface = SDL_CreateRGBSurface(0, 1280, 720, 32, 0, 0, 0, 0);
    
    //Init renderer
    renderer = SDL_CreateRenderer(main_Window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    //Init TTF
    if(TTF_Init() == -1){
        printf("TTF init error: %s\n", TTF_GetError());
    }
    
    font = TTF_OpenFont("../fonts/8bitOperatorPlus-Regular.ttf", 30);
    if(!font){
        printf("Title font not loaded! %s\n", TTF_GetError());
        return false;
    }
    
    //Init main menu texts
    for(int i = 0; i < main_menu_text_count; i++){
        char *text;
        SDL_Rect rect;
        
        //Set texts and rects
        switch (i) {
            case 0:
                text = "PI-6 Tower Defense";
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1:
                text = "Play";
                rect = (SDL_Rect){980, 480, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 2:
                text = "Config";
                rect = (SDL_Rect){980, 480 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3:
                text = "Score";
                rect = (SDL_Rect){980, 480 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 4:
                text = "Credits";
                rect = (SDL_Rect){30, 480 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 5:
                text = "Exit";
                rect = (SDL_Rect){980, 480 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        main_menu_rects[i] = rect;
        
        SDL_Surface *surface = TTF_RenderText_Solid(font, text, black);
        
        if(!surface){
            printf("Text not rendered! %s\n", TTF_GetError());
            return false;
        }
        
        main_menu_texts[i] = SDL_CreateTextureFromSurface(renderer, surface);
        
        if(!main_menu_texts[i]){
            printf("Text texture not rendered! %s\n", SDL_GetError());
            return false;
        }
        
        SDL_FreeSurface(surface);
    }
	
	//Init game running screen texts
    get_config();
	
	//Init game paused screen texts
	
	//Init game credits screen texts
	
	//Init game score screen texts
	
	
    map_Surface = init_map();
    
    if(!map_Surface){
        printf("Falha ao inicializar mapa!\n");
        return false;
    }
    
    return true;
}

//Encerra SDL
void main_quit(){
    //Close fonts
    TTF_CloseFont(font);
    
    //Destroy textures
    for(int i = 0; i < main_menu_text_count; i++){
        SDL_DestroyTexture(main_menu_texts[i]);
    }
    
    for(int i = 0; i < config_menu_text_count; i++){
        SDL_DestroyTexture(config_menu_texts[i]);
    }
    
    //Free surfaces
    SDL_FreeSurface(map_Surface);
    
    //Free window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(main_Window);
    
    //Quit SDL, TTF, IMG
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
	
	//Free config
	//free(config->language);
	free(config);
    
    //Free lists
    if(minions)
        free_list_minion(minions);
    if(turrets)
        free_list_turret(turrets);
    if(projectiles)
        free_list_projectile(projectiles);
}

//Carrega textos do menu de configurações
void get_config(){
    for(int i = 0; i < config_menu_text_count; i++){
        char *text;
        SDL_Rect rect;
        
        //Set texts and rects
        switch(i){
            case 0:
                text = "Config";
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1:
                if(config->audio_sfx)
                    text = "Sound Effects:  ON";
                else
                    text = "Sound Effects:  OFF";
                
                rect = (SDL_Rect){400, 150, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 2:
                if(config->audio_music)
                    text = "Music:  ON";
                else
                    text = "Music:  OFF";
                
                rect = (SDL_Rect){400, 150 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3:
                text = "Language";
                
                rect = (SDL_Rect){400, 150 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 4:
                text = "Back";
                
                rect = (SDL_Rect){400, 150 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        config_menu_rects[i] = rect;
        
        SDL_Surface *surface = TTF_RenderText_Solid(font, text, black);
        
        if(!surface){
            printf("Text not rendered! %s\n", TTF_GetError());
            return;
        }
        
        config_menu_texts[i] = SDL_CreateTextureFromSurface(renderer, surface);
        
        if(!config_menu_texts[i]){
            printf("Text texture not rendered! %s\n", SDL_GetError());
            return;
        }
        
        SDL_FreeSurface(surface);
    }
}