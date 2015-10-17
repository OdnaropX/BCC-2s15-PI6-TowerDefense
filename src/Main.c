//
//  main.c
//  PI6 - TowerDefense
//
//  Created by Danilo Makoto Ikuta on 10/09/15.
//  Copyright (c) 2015 Danilo Makoto Ikuta. All rights reserved.
//

//Main Menu (Loads SDL and main menu assets)

#include "Estruturas.h"
#include "Renderer.h"
#include "GameScene.h"

#define main_menu_assets_count 11
#define config_menu_assets_count 9
#define game_interface_assets_count 2
#define pause_interface_assets_count 13
#define credits_menu_assets_count 7
#define score_menu_assets_count 3
#define game_over_interface_assets_count 7
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
SDL_Color white = {255, 255, 255, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color red = {255, 0, 0, 255};

//Static texts
//Main menu order: title, play(+sel), config(+sel), score(+sel), credits(+sel), exit(+sel)
SDL_Texture *main_menu_assets[main_menu_assets_count];
SDL_Rect main_menu_rects[main_menu_assets_count];

//Config order: Config, sfx(+sel), music(+sel), language(+sel), back(+sel)
SDL_Texture *config_menu_assets[config_menu_assets_count];
SDL_Rect config_menu_rects[config_menu_assets_count];

//Game interface order: Pause button, Right bar
SDL_Texture *game_interface_assets[game_interface_assets_count];
SDL_Rect game_interface_rects[game_interface_assets_count];

//Pause game interface order: overlay, resume(+sel), config(+sel), score(+sel), exit(+sel), main(+sel), credits(+sel)
SDL_Texture *pause_interface_assets[pause_interface_assets_count];
SDL_Rect pause_interface_rects[pause_interface_assets_count];

//Credits screen order: title, text(4 lines), back(+sel)
SDL_Texture *credits_menu_assets[credits_menu_assets_count];
SDL_Rect credits_menu_rects[credits_menu_assets_count];

//Score screen order: title, back(+sel)
SDL_Texture *score_menu_assets[score_menu_assets_count];
SDL_Rect score_menu_rects[score_menu_assets_count];

//Game over order: overlay, retry(+sel), main menu(+sel), exit(+sel)
SDL_Texture *game_over_interface_assets[game_over_interface_assets_count];
SDL_Rect game_over_interface_rects[game_over_interface_assets_count];

CONFIGURATION *config;

list_minion *minions;
//list_projectile *projectiles;     Not used by now
list_turret *turrets;

int health = 5;
int gold = 1000;
int mana = 0;

//Cabeçalhos
bool main_init();
void main_quit();

void get_config();
void reset_game_data();

int main(int argc, char * argv[]) {
    bool quit = false;
    
    //Main init
    if(!main_init()){
        quit = true;
    }
	
	
	//Screen control
	screen current_screen = MAIN;
	screen previous_screen = MAIN;
	
	//Options control
	main_options main_option = OPT_NONE;
    config_options config_option = NONE;
	pause_options pause_option = OPT_P_NONE;
	GAME_RUNNING_OPTIONS running_option;
    game_over_options game_over_option = GO_NONE;
	
	running_option.current_tab = TOP_MENU;
	running_option.top = OPT_R_T_NONE;
	running_option.left = OPT_R_L_NONE;
	running_option.game_area.left = OPT_R_A_L_NONE;
	running_option.game_area.right = OPT_R_A_R_NONE;
	
	//Keyboard options control
	main_options select_option = OPT_PLAY;
	config_options select_config_option = AUDIO_SFX;
	pause_options select_pause_option = OPT_P_NONE;
	GAME_RUNNING_OPTIONS select_running_option;
    game_over_options select_game_over_option = GO_NONE;
	
	select_running_option.current_tab = TOP_MENU;
	select_running_option.top = OPT_R_T_NONE;
	select_running_option.left = OPT_R_L_NONE;
	select_running_option.game_area.left = OPT_R_A_L_NONE;
	select_running_option.game_area.right = OPT_R_A_R_NONE;
	
	SDL_Event event;
    
	bool game_started = false;
	bool game_paused = false;
	
	//Click control
	bool active_clicked = false;
    bool run_action = false;
	bool left_click = true;
	
    bool return_to_previous_screen = false;     //For scores and credits menus
    bool select_return_to_previous_screen = false;     //For scores and credits menus
	
	
	//Game area control
    bool selected_left = false;//Right click equals to tower and left to minions.
	int add_tower = 0;
	int add_minion = 0;
	
	int current_position[] = {0,0};
	
	minion *new_minion;
	turret *new_turret;
	
	//Display control
	bool show_gold_info = false;
	bool show_mana_info = false;
	bool show_life_info = false;
	
	
	//FPS and timer
    int t1, t2;
    int delay = 17; //Aprox. de 1000ms/60
	int show_timer = 0;
	int timer_count = 0;
	int frame = 0;
    t1 = SDL_GetTicks();
    
	int select_grid = 0;
	int click_grid = 0;
	int max_grid = 17 * 13;
	
    // Following parts are only for first interation;
    int monsterSpawner[] = {6, 8, 12, 14, 17, 18, 25, 16, 18, 50};
	
	//Wave control - 20 seconds, after 
	int spawn_minion = 0;
	int pending_wave_number = 0;
	int timer_minion = 20;
	
	int grid_clicked[] = {0,0};
	int center_clicked[] = {0,0};
	
	int select_grid_option = 0;
	
	int temp_option;
	
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
						case SDL_KEYDOWN:
							switch(event.key.keysym.sym){
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
								}
							break;
						//Handle keyboard key release event.
						case SDL_KEYUP:
							switch(event.key.keysym.sym){
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
							}
							break;
						//Handle mouse event
						case SDL_MOUSEBUTTONUP:												
							if(event.button.button == SDL_BUTTON_LEFT){
								main_option = OPT_NONE;//Maybe dont need this. bug check
								
								//Check if location selected is a valid one
										
								if (event.motion.x >= 980 && event.motion.x <= 980 + BUTTON_MENU_WIDTH) {//Near main config
									//This is the same as the code commented below, because I have yet to test I didnt remove the comment. If someone can test tell me if is working as aspect.
									temp_option = (event.motion.y - 480) / BUTTON_MENU_HEIGHT;
									if (temp_option < OPT_CREDIT && temp_option >= 0) {
										main_option = temp_option;
									}
											
								}
								else if (event.motion.x >= 30 && event.motion.x <= 30 + BUTTON_MENU_WIDTH) {
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
								temp_option = (event.motion.y - 480) / BUTTON_MENU_HEIGHT;
								if (temp_option < OPT_CREDIT && temp_option >= 0) {
									select_option = temp_option;
								}
							}
							else if (event.motion.x >= 30 && event.motion.x <= 30 + BUTTON_MENU_WIDTH) {
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
						//Handle keyboard event
						case SDL_KEYUP:
							switch(event.key.keysym.sym){
								//Backspace
								case SDLK_BACKSPACE:
									//Go back main screen or paused screem
									if (previous_screen == MAIN){
										current_screen = MAIN;
									}
									else {
										current_screen = GAME_PAUSED;
									}
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
									else if (select_config_option == BACK) {
										
									}
									else {
										config_option = select_config_option;
									}
									break;
								case SDLK_RIGHT:
									//Change current selected option
									if (select_config_option == LANGUAGE) {
										
									}
									else if (select_config_option == BACK) {
										
									}
									else {
										config_option = select_config_option;
									}
									break;
								
								case SDLK_DOWN:
									//Change current selected option
									select_config_option = (select_config_option + 1) % 4;
									break;
							}
						//Handle mouse event
						case SDL_MOUSEBUTTONUP:									
							if(event.button.button == SDL_BUTTON_LEFT){	
								//Check if location selected is a valid one
								if (event.motion.x >= 595 && event.motion.x <= 595 + BUTTON_MENU_WIDTH ) {//Near main config
									temp_option = (event.motion.y - 150) / BUTTON_MENU_HEIGHT;
									if (temp_option <= BACK && temp_option >= 0) {
										config_option = temp_option;
									}
								}
							}
							break;
						case SDL_MOUSEMOTION:
							if (event.motion.x >= 595 && event.motion.x <= 595 + BUTTON_MENU_WIDTH ) {//Near main config
									temp_option = (event.motion.y - 150) / BUTTON_MENU_HEIGHT;
									if (temp_option <= BACK && temp_option >= 0) {
										select_config_option = temp_option;
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
						
						//Handle keyboard release
						case SDL_KEYUP:
							switch(event.key.keysym.sym){
								//Backspace
								case SDLK_BACKSPACE:
									current_screen = MAIN;
									previous_screen = GAME_RUNNING;
									break;
								//Escape
								case SDLK_ESCAPE:
									//Show Game Pause screen with options
									current_screen = GAME_PAUSED;
									select_pause_option = OPT_P_RESUME;
									break;	
								case SDLK_q:
									printf("Key pressed: q\n"); 
									if (running_option.current_tab == GAME_AREA && !active_clicked){
										//Active click on current location
										active_clicked = true;
										selected_left = true;
									}
									break;
								case SDLK_e:
									printf("Key pressed: e\n"); 
									if (running_option.current_tab == GAME_AREA && !active_clicked){
										//Active click on current location
										active_clicked = true;
										selected_left = false;
									}
									break;

								//Keypad enter
								case SDLK_KP_ENTER:
									//Check current action selected from menu and initiated it.
									switch(running_option.current_tab) {
										case TOP_MENU:
											switch(select_running_option.top){
												case OPT_R_T_PAUSE:
													//Dont do any action after this, only render the scene. 
													current_screen = GAME_PAUSED;
													game_paused = true;
													break;
												case OPT_R_T_RESUME:
													current_screen = GAME_RUNNING;
													game_paused = false;
													break;
												case OPT_R_T_NONE:
													//Do nothing. Need this to not show warning on Windows.
													break;
											}
											break;
										case LEFT_MENU:
											switch(select_running_option.left){
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
												case OPT_R_L_NONE:
													//Do nothing. Need this to not show warning on Windows.
													break;
											}
											break;
										case BOTTOM_MENU:
											//Do nothing. Need this to not show warning on Windows.
											break;
										case GAME_AREA:
											//Enter only work on active_clicked.
											if (active_clicked){
												if (selected_left) {
													add_tower = select_running_option.game_area.right + 1;
                                                    get_cartesian_from_grid_number(select_grid, current_position, 17);
												}
												else {
													add_minion = select_running_option.game_area.left + 1;
													get_cartesian_from_grid_number(select_grid, current_position, 17);
												}
											}
											break;
										
									}
									break;
								//Keyboard enter
								case SDLK_RETURN:
									//Check current action selected from menu and initiated it.
									switch(running_option.current_tab) {
										case TOP_MENU:
											switch(select_running_option.top){
												case OPT_R_T_PAUSE:
													//Dont do any action after this, only render the scene. 
													current_screen = GAME_PAUSED;
													game_paused = true;
													break;
												case OPT_R_T_RESUME:
													current_screen = GAME_RUNNING;
													game_paused = false;
													break;
												case OPT_R_T_NONE:
													//Do nothing. Need this to not show warning on Windows.
													break;
													
											}
											break;
										case LEFT_MENU:
											switch(select_running_option.left){
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
												case OPT_R_L_NONE:
													//Do nothing. Need this to not show warning on Windows.
													break;
											}
											break;
											
										case BOTTOM_MENU:
											//Do nothing. Need this to not show warning on Windows.
											break;
										case GAME_AREA:
											if (active_clicked){
												if (selected_left) {
													add_tower = select_running_option.game_area.right + 1;
													get_cartesian_from_grid_number(select_grid, current_position, 17);
												}
												else {
													add_minion = select_running_option.game_area.left + 1;
													get_cartesian_from_grid_number(select_grid, current_position, 17);
												}
											}
											break;
										
									}
									break;
								case SDLK_TAB:
									//Move selector location
									running_option.current_tab = (running_option.current_tab + 1) % 4;
									
									break;
								case SDLK_UP:
									//Move selected selector. Selector is choose with tab.
									switch(running_option.current_tab) {
										case TOP_MENU:
											//Move up top menu
											if (select_running_option.top == OPT_R_T_PAUSE) {
												select_running_option.top = OPT_R_T_RESUME;
											}
											else {
												select_running_option.top--;
											}
											break;
										
										case LEFT_MENU:
											//Move up left menu
											if (select_running_option.left == OPT_R_L_GOLD) {
												select_running_option.left = OPT_R_L_LIFE;
											}
											else {
												select_running_option.left--;
											}
											break;
											
										case BOTTOM_MENU:
											//Do nothing. Need this to not show warning on Windows.
											break;
										
										case GAME_AREA:
											//Move up mouse cursor between options 
											if(active_clicked) {//Equals to mouse clicked previously
												if (selected_left) {
													if (select_running_option.game_area.left == OPT_R_A_L_TOWER_1) {
														select_running_option.game_area.left = OPT_R_A_L_TOWER_3;
													}
													else {
														select_running_option.game_area.left--;
													}
												}
												else {
													if (select_running_option.game_area.right == OPT_R_A_R_MINION_1) {
														select_running_option.game_area.right = OPT_R_A_R_MINION_3;
													}
													else {
														select_running_option.game_area.right--;
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
									switch(running_option.current_tab) {
										case TOP_MENU:
											//Move left top menu - Same as UP because menu is linear.
											if (select_running_option.top == OPT_R_T_PAUSE) {
												select_running_option.top = OPT_R_T_RESUME;
											}
											else {
												select_running_option.top--;
											}
											break;
										
										case LEFT_MENU:
											//Move left left menu
											if (select_running_option.left == OPT_R_L_GOLD) {
												select_running_option.left = OPT_R_L_LIFE;
											}
											else {
												select_running_option.left--;
											}
											break;
										
										case BOTTOM_MENU:
											//Do nothing. Need this to not show warning on Windows.
											break;
											
										case GAME_AREA:
											//Move left mouse cursor from GAME_AREA
											if(active_clicked) {//Equals to mouse clicked previously
												if (selected_left) {
													if (select_running_option.game_area.left == OPT_R_A_L_TOWER_1) {
														select_running_option.game_area.left = OPT_R_A_L_TOWER_3;
													}
													else {
														select_running_option.game_area.left--;
													}
												}
												else {
													if (select_running_option.game_area.right == OPT_R_A_R_MINION_1) {
														select_running_option.game_area.right = OPT_R_A_R_MINION_3;
													}
													else {
														select_running_option.game_area.right--;
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
									switch(running_option.current_tab) {
										case TOP_MENU:
											//Move right top menu
											select_running_option.top = (select_running_option.top + 1) % 2;
											break;
										
										case LEFT_MENU:
											//Move right left menu
											select_running_option.left = (select_running_option.left + 1) % 3;
											break;
										
										case BOTTOM_MENU:
											//Do nothing. Need this to not show warning on Windows.
											break;
											
										case GAME_AREA:
											//Move right mouse cursor from GAME_AREA
											if(active_clicked){
												if (selected_left) {
													select_running_option.game_area.left = (select_running_option.game_area.left + 1) % 3;
												}
												else {
													select_running_option.game_area.right = (select_running_option.game_area.right + 1) % 3;
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
									switch(running_option.current_tab) {
										case TOP_MENU:
											//Move down top menu
											select_running_option.top = (select_running_option.top + 1) % 2;
											break;
										
										case LEFT_MENU:
											//Move down left menu
											select_running_option.left = (select_running_option.left + 1) % 3;
											break;
											
										case BOTTOM_MENU:
											//Do nothing. Need this to not show warning on Windows.
											break;
										case GAME_AREA:
											//Move down mouse cursor from GAME_AREA
											if(active_clicked){
												if (selected_left) {
													select_running_option.game_area.left = (select_running_option.game_area.left + 1) % 3;
													
												}
												else {
													select_running_option.game_area.right = (select_running_option.game_area.right + 1) % 3;
												}
											}
											else {
											//Move down mouse cursor from GAME_AREA. 
												select_grid = (select_grid + 1) % max_grid;
											}
											break;
									}
									break;
							}
						
						//Mouse motion
						case SDL_MOUSEMOTION:
							//Move mouse selector over game area
							if (!active_clicked && get_touched_grid_address(event.motion.x, event.motion.y, grid_clicked)){
								select_grid = get_grid_address_linear(grid_clicked[0], grid_clicked[1], 17);
								printf("grid selected x%d y%d\n", grid_clicked[0], grid_clicked[1]);
								printf("linear %d\n", select_grid);
							}
							break;
						//Handle mouse event
						case SDL_MOUSEBUTTONUP:
							if(event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT){
								left_click = event.button.button == SDL_BUTTON_LEFT;
								
								//Game area
								if(get_touched_grid_address(event.motion.x, event.motion.y, grid_clicked)){
									current_position[0] = grid_clicked[0];
									current_position[1] = grid_clicked[1];
								
									select_grid = get_grid_address_linear(grid_clicked[0], grid_clicked[1], 17);								
									
									//convert select grid
									get_grid_address_matriz(select_grid, 17, grid_clicked);
									//Get center of clicked block on grid.
									get_center_position_to_grid(grid_clicked[0], grid_clicked[1], grid_clicked);
									
									if (!active_clicked){
										click_grid = select_grid;
										center_clicked[0] = grid_clicked[0];
										center_clicked[1] = grid_clicked[1];
										active_clicked = true;
										selected_left = left_click;
									}
									//Check where was clicked.
									else if (get_touched_menu_address(event.motion.x, event.motion.y, grid_clicked, &select_grid_option, selected_left)){
										printf("touch %d\n", select_grid_option);
										if (selected_left) {
											add_tower = select_grid_option;
										}
										else {
											add_minion = select_grid_option;
										}
										active_clicked = false;
									}
									else {
										active_clicked = false;
										printf("Not clicked");
									}
								}

								//Top menu
								else if(left_click && event.motion.y >= 0 && event.motion.y <= BUTTON_MENU_HEIGHT){
									if (event.motion.x >= BUTTON_MENU_HEIGHT && event.motion.x <= BUTTON_MENU_HEIGHT * 2){
										//Pause game
										game_paused = !game_paused;
										//Not sure if must use current_screen to show the pause screen or just change status to make network request.
										current_screen = GAME_PAUSED;
									}
								}
								//Left menu
								else if(left_click && event.motion.x >= window_width - 10 - BUTTON_MENU_HEIGHT && event.motion.y <= window_width - 10) {
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
							break;
					}
					break;
				
				case GAME_PAUSED:
					switch (event.type) {
						//Quit
						case SDL_QUIT:
							quit = true;
							break;
							
						//Case Keyboard pressed and release
						case SDL_KEYUP:
							switch(event.key.keysym.sym){
								//Backspace
								case SDLK_BACKSPACE:
									//Dont break here and only below.
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
										case OPT_P_CREDITS:
											current_screen = CREDITS;
											previous_screen = GAME_PAUSED;
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
										case OPT_P_CREDITS:	
											current_screen = CREDITS;
											previous_screen = GAME_PAUSED;
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
									select_pause_option = (select_pause_option + 1) % OPT_P_NONE;
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
									select_pause_option = (select_pause_option + 1) % OPT_P_NONE;
									break;
							}
						
						//Handle mouse event
						case SDL_MOUSEBUTTONUP:
							if(event.button.button == SDL_BUTTON_LEFT){
								pause_option = OPT_P_NONE;
								
								//Check if location selected is a valid one
								if (event.motion.x >= 515 && event.motion.x <= 765) {
									temp_option = (event.motion.y - 270) / BUTTON_MENU_HEIGHT;
									if (temp_option <= OPT_P_CREDITS && temp_option >= 0) {
										pause_option = temp_option;
									}
								}
							}
							break;
						//Handle mouse moved event.
                        case SDL_MOUSEMOTION:
							if (event.motion.x >= 515 && event.motion.x <= 765) {
								temp_option = (event.motion.y - 270)/ BUTTON_MENU_HEIGHT;
								if (temp_option <= OPT_P_CREDITS && temp_option >= 0){
									select_pause_option = temp_option;
								}
							}
							break;
						
					}
                    
                case CREDITS:
                    switch (event.type) {
                        case SDL_QUIT:
                            quit = true;
                            break;
                            
                        case SDL_KEYUP:
                            if(event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
                                return_to_previous_screen = true;
							//Add handle for button go back
							
							
                            break;
                            
                        case SDL_MOUSEBUTTONUP:
                            if(event.button.button == SDL_BUTTON_LEFT){
                                if(event.motion.x >= 595 && event.motion.x <= 595 + BUTTON_MENU_WIDTH && event.motion.y >= 650 && event.motion.y <= 650 + BUTTON_MENU_HEIGHT){
                                    return_to_previous_screen = true;
                                    select_return_to_previous_screen = true;
                                }
                            }
                            break;
                            
                        case SDL_MOUSEMOTION:
                            if(event.motion.x >= 595 && event.motion.x <= 595 + BUTTON_MENU_WIDTH && event.motion.y >= 650 && event.motion.y <= 650 + BUTTON_MENU_HEIGHT)
                                select_return_to_previous_screen = true;
                            else
                                select_return_to_previous_screen = false;
                            
                        default:
                            break;
                    }
                    
                    break;
                    
                case SCORE:
                    switch (event.type) {
                        case SDL_QUIT:
                            quit = true;
                            break;
                            
                        case SDL_KEYUP:
                            if(event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
                                return_to_previous_screen = true;
							//Add handle of buttons.
							
                            break;
                            
                        case SDL_MOUSEBUTTONUP:
                            if(event.button.button == SDL_BUTTON_LEFT){
                                if(event.motion.x >= 595 && event.motion.x <= 595 + BUTTON_MENU_WIDTH && event.motion.y >= 650 && event.motion.y <= 650 + BUTTON_MENU_HEIGHT){
                                    return_to_previous_screen = true;
                                    select_return_to_previous_screen = true;
                                }
								
								
								//Add handle of buttons.
                            }
                            break;
                            
                        case SDL_MOUSEMOTION:
                            if(event.motion.x >= 595 && event.motion.x <= 595 + BUTTON_MENU_WIDTH && event.motion.y >= 650 && event.motion.y <= 650 + BUTTON_MENU_HEIGHT)
                                select_return_to_previous_screen = true;
                            else
                                select_return_to_previous_screen = false;
                            
                        default:
                            break;
                    }

                    break;
                    
                case GAME_OVER:
                    switch (event.type) {
                        case SDL_QUIT:
                            quit = true;
                            break;
                            
                        case SDL_KEYUP:
                            switch (event.key.keysym.sym) {
                                case SDLK_ESCAPE:
                                    quit = true;
                                    break;
                                    
                                case SDLK_KP_ENTER:
                                    if(select_game_over_option != GO_NONE)
                                        game_over_option = select_game_over_option;
                                    break;
                                    
                                case SDLK_RETURN:
                                    if(select_game_over_option != GO_NONE)
                                        game_over_option = select_game_over_option;
                                    break;
                                    
                                case SDLK_UP:
                                    if(select_game_over_option == GO_RETRY)
                                        select_game_over_option = GO_QUIT;
                                    else
                                        select_game_over_option++;
                                    break;
                                    
                                case SDLK_DOWN:
                                    if(select_game_over_option == GO_QUIT)
                                        select_game_over_option = GO_RETRY;
                                    else
                                        select_game_over_option--;
                                    break;
                                    
                                case SDLK_RIGHT:
                                    if(select_game_over_option == GO_RETRY)
                                        select_game_over_option = GO_QUIT;
                                    else
                                        select_game_over_option++;
                                    break;
                                    
                                case SDLK_LEFT:
                                    if(select_game_over_option == GO_QUIT)
                                        select_game_over_option = GO_RETRY;
                                    else
                                        select_game_over_option--;
                                    break;
                                    
                                default:
                                    break;
                            }
                            break;
                            
                        case SDL_MOUSEBUTTONUP:
                            if(event.button.button == SDL_BUTTON_LEFT){
                                game_over_option = GO_NONE;
                                
                                if (event.motion.x >= 515 && event.motion.x <= 765) {
                                    temp_option = (event.motion.y - 360) / BUTTON_MENU_HEIGHT;
                                    if (temp_option <= GO_QUIT && temp_option >= 0) {
                                        game_over_option = temp_option;
                                    }
                                }
                            }
                            break;
                            
                        case SDL_MOUSEMOTION:
                            if (event.motion.x >= 515 && event.motion.x <= 765) {
                                temp_option = (event.motion.y - 360) / BUTTON_MENU_HEIGHT;
                                if (temp_option <= GO_QUIT && temp_option >= 0) {
                                    select_game_over_option = temp_option;
                                }
                            }
                            break;
                            
                        default:
                            break;
                    }
                    
                    break;
            }
        }
		
		
        //Timer handling
		////////////////////////////////////////////////////////
		frame++;
		if (frame == FRAMES_PER_SEC) {
			frame = 0;
			
			//One second timer
			show_timer++;
			
			
			//Game timer.
			if(game_started && !game_paused) {
				//One second timer
				timer_count++;
					
				//Add more gold 1
				gold++;
				//Add more mana 1
				mana++;
					
				//Wave spawning.
				if(pending_wave_number > 0) {
					add_minion = (add_minion + 1) % get_minion_avaliable();
					pending_wave_number--;
				}
                
				/*
				//Timer, use this if to run code for each 2 seconds.
				if (is_time(timer_count, 2)) {
					
				}
				//Timer, use this if to run code for each  3 seconds.			
				if (is_time(timer_count, 3)) {
					
				}
				//Timer, use this if to run code for each 5 seconds.			
				if (is_time(timer_count, 5)) {
			
				}
				//Timer, use this if to run code for each  7 seconds.			
				if (is_time(timer_count, 7)) {
				
				}
				//Timer, use this if to run code for each 11 seconds.			
				if (is_time(timer_count, 11)) {
					
				}			
				//Timer, use this if to run code for each  13 seconds.			
				if (is_time(timer_count, 13)) {
			
				}			
				//Timer, use this if to run code for each 17 seconds.			
				if (is_time(timer_count, 17)) {
				
				}*/		
					

				//Timer, use this if to run code for each timer_minion seconds, for minions.
				if (is_time(timer_count, timer_minion)){
					//New wave
					pending_wave_number = monsterSpawner[spawn_minion];
					timer_minion = pending_wave_number + 20;
					spawn_minion++;
					
					if (spawn_minion > 0 && spawn_minion < 10) {
						spawn_minion = 0;
						timer_minion = 20;
					}
				}
                
			}
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
					case OPT_EXIT:
						quit = true;
						break;
					case OPT_PLAY:
						printf("Current screen OPT_PLAY");
						game_started = true;
						//Generate Map and resources to use on Game_Running
						
					
						//Change current screen.
						current_screen = GAME_RUNNING;
						
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
                main_option = OPT_NONE;
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
                        get_config();
						break;
					case AUDIO_MUSIC:
						//Trogle ambience music
						if (config->audio_music == true) {
							config->audio_music = false;
						}
						else {
							config->audio_music = true;
						}
                        get_config();
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
                
                config_option = NONE;
				break;
			
            
            case GAME_RUNNING:
				if (!game_paused){
					if (add_tower > 0){
						//Add tower
                        if(gold > 100){
                            //Check if position already has tower
                            if(occupyGrid(current_position[0], current_position[1])){
                                //Check if turret == Snorlax
                                if(!perform_path_verification(16, 5)){ // Blocking path.
                                    freeGrid(current_position[0], current_position[1]);
                                    perform_path_verification(16, 5);
                                }
                                else{ // SUCCESS
                                    new_turret = init_turret(add_tower, current_position[0], current_position[1]);
                                    add_turret_to_list(turrets, new_turret);
                                    gold -= 100;
                                }
                            }
                        }
						//Reset tower
						add_tower = 0;
					}
                    
					if (add_minion > 0){
						//Add minion

                        new_minion = init_minion(add_minion);     //minion_id not used
                        add_minion_to_list(minions, new_minion);
                        new_minion->node->xPos = 150;
                        new_minion->node->yPos = 600;
                        
                        
						//Reset minion
						add_minion = 0;
					}
		
                    // DEM ROUTINES, YO!

                    // Minion movement, Projectile movement, and projectile colision/dealocation.
                    list_minion* enemy = minions;
                    while(enemy && enemy->e){
                        int minion_pos_value = move_minion(enemy->e);
                        list_projectile *shoot = enemy->e->targetted_projectils;
                        if(minion_pos_value == 1){
                            enemy->e->HP = 0;
                            health --;
                        }
                        
                        while (shoot && shoot->e) {
                            if(move_bullet(enemy->e, shoot->e)){ // The movement is made in the if call.
                                enemy->e->HP -= shoot->e->damage;
                                list_projectile *temp_lp = shoot;
                                
                                shoot = shoot->next;
                                
                                enemy->e->targetted_projectils = remove_projectile_from_list(enemy->e->targetted_projectils, temp_lp->e);
                                temp_lp->e = NULL;
                            }
                            
                            else
                                shoot = shoot->next;
                        }
                        
                        if(enemy->e->HP <= 0){ // Death of minions
                            list_minion *temp_minion = enemy;
                            enemy = enemy->next;
                            
                            gold += 12;
                            
                            minions = remove_minion_from_list(minions, temp_minion->e);
                            temp_minion->e = NULL;
                            temp_minion = NULL;
                        }
                        
                        else
                            enemy = enemy->next;
                    }
                    
                    list_turret *turret = turrets;
                    while (turret && turret->e) {
                        turret->e->timeUntilNextAttack -= 0.017;
                        if(turret->e->timeUntilNextAttack <= 0){
                            enemy = minions;
                            minion *target = NULL;
                            
                            while(target == NULL && enemy && enemy->e){
                                minion *temp = enemy->e;

                                if((temp->node->xPos - turret->e->node->xPos) * (temp->node->xPos - turret->e->node->xPos) + (temp->node->yPos - turret->e->node->yPos) * (temp->node->yPos - turret->e->node->yPos) < 22500) // If within range 150px
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
                    
                    if(health <= 0){
                        current_screen = GAME_OVER;
                        game_started = false;
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
                        reset_game_data();
                        break;
					case OPT_P_CONFIG:
						current_screen = CONFIG;
						previous_screen = GAME_PAUSED;
                        break;
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
                
                pause_option = OPT_P_NONE;
				break;
                
            case CREDITS:
                if(return_to_previous_screen)
                    current_screen = previous_screen;
                return_to_previous_screen = false;
                break;
                
            case SCORE:
                if(return_to_previous_screen)
                    current_screen = previous_screen;
                return_to_previous_screen = false;
                break;
                
            case GAME_OVER:
                switch (game_over_option) {
                    case GO_RETRY:
                        reset_game_data();
                        show_timer = 0;
                        current_screen = GAME_RUNNING;
                        game_started = true;
                        break;
                        
                    case GO_MAIN:
                        reset_game_data();
                        current_screen = MAIN;
                        game_started = false;
                        break;
                        
                    case GO_QUIT:
                        quit = true;
                        break;
                        
                    default:
                        break;
                }
                
                game_over_option = GO_NONE;
                break;
            
		}
        
        //Clear render
        SDL_RenderClear(renderer);
		
		//Scene Renderer 
		/////////////////////////////////////////////////////
        SDL_Texture *screen_surfaces = NULL;
        
        switch (current_screen) {
            case CONFIG:
				//select_config_option
                draw_screen_config(renderer, config_menu_assets, config_menu_rects, config_menu_assets_count, select_config_option);
                break;
            
            case CREDITS:
				//get control of select. < TODO.

                draw_screen_credits(renderer, credits_menu_assets, credits_menu_rects, credits_menu_assets_count, select_return_to_previous_screen);
                break;
                
            case GAME_PAUSED:
				//select_pause_option
				draw_screen_game_paused(renderer, pause_interface_assets, pause_interface_rects, pause_interface_assets_count, select_pause_option);
				break;
                
            case GAME_RUNNING:
				select_running_option.current_tab = running_option.current_tab;
				//active_clicked
				//select_grid
				//selected_left
				
                draw_screen_game_running(main_Surface, map_Surface, minions, turrets);
                
                screen_surfaces = SDL_CreateTextureFromSurface(renderer, main_Surface);
                SDL_RenderCopy(renderer, screen_surfaces, NULL, &(SDL_Rect){0, 0, 1280, 720});
			
                draw_screen_game_interface(renderer, game_interface_assets, game_interface_rects, game_interface_assets_count);
                
				display_mouse(renderer, active_clicked, selected_left, click_grid, select_grid, center_clicked, select_running_option);
				
                display_health(renderer, health, font);
                display_mana(renderer, mana, font);
                display_gold(renderer, gold, font);
                
                break;
                
            case MAIN:
				//select_option
                draw_screen_main(renderer, main_menu_assets, main_menu_rects, main_menu_assets_count, select_option);
                break;
                
            case SCORE:
				//get control of select. < TODO.
				
				
                draw_screen_score(renderer, score_menu_assets, score_menu_rects, score_menu_assets_count, select_return_to_previous_screen);
                break;
                
            case GAME_OVER:
                draw_screen_game_over(renderer, game_over_interface_assets, game_over_interface_rects, game_over_interface_assets_count, select_game_over_option);
                break;
                
            default:
                break;
        }
		
        //Update render and surfaces
        SDL_RenderPresent(renderer);
		
		//Destroy
		if (screen_surfaces){
			SDL_DestroyTexture(screen_surfaces);
			screen_surfaces = NULL;
        }
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
    for(int i = 0; i < main_menu_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        //Set texts and rects
        switch (i) {
            case 0:
                text = "PI-6 Tower Defense";
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1: case 2:
                text = "Play";
                rect = (SDL_Rect){980, 480, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3: case 4:
                text = "Config";
                rect = (SDL_Rect){980, 480 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 5: case 6:
                text = "Score";
                rect = (SDL_Rect){980, 480 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 7: case 8:
                text = "Credits";
                rect = (SDL_Rect){30, 480 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 9: case 10:
                text = "Exit";
                rect = (SDL_Rect){980, 480 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        main_menu_rects[i] = rect;
        
        SDL_Surface *surface;
        
        if(i%2 == 0 && i > 0)
            surface = TTF_RenderText_Solid(font, text, red);
        else
            surface = TTF_RenderText_Solid(font, text, black);
        
        if(!surface){
            printf("(Main)Text not rendered! %s\n", TTF_GetError());
            return false;
        }
        
        main_menu_assets[i] = SDL_CreateTextureFromSurface(renderer, surface);
        
        if(!main_menu_assets[i]){
            printf("(Main)Text texture not rendered! %s\n", SDL_GetError());
            return false;
        }
        
        SDL_FreeSurface(surface);
    }
	
	//Init game running screen texts
    get_config();
	
	//Init game paused screen texts
    for(int i = 0; i < pause_interface_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        switch(i){
            case 0:
                rect = (SDL_Rect){0, 0, 1280, 720};
                break;
                
            case 1: case 2:
                text = "Resume";
                rect = (SDL_Rect){515, 270, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3: case 4:
                text = "Config";
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 5: case 6:
                text = "Score";
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 7: case 8:
                text = "Exit";
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 9: case 10:
                text = "Main Menu";
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 11: case 12:
                text = "Credits";
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 5, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        pause_interface_rects[i] = rect;
        
        SDL_Surface *surface;
        if(i == 0){
            surface = IMG_Load("../images/Pause Menu.png");
            if(!surface){
                printf("(Game pause)Erro ao carregar pause menu! %s\n", IMG_GetError());
                return false;
            }
        }
        
        else{
            if(i%2 == 0 && i > 0)
                surface = TTF_RenderText_Solid(font, text, red);
            else
                surface = TTF_RenderText_Solid(font, text, white);
            
            if(!surface){
                printf("(Game pause)Text not rendered! %s\n", TTF_GetError());
                return false;
            }
        }
        
        pause_interface_assets[i] = SDL_CreateTextureFromSurface(renderer, surface);
        
        if(!pause_interface_assets[i]){
            printf("(Game pause)Erro ao criar textura! %s\n", SDL_GetError());
            return false;
        }
        
        SDL_FreeSurface(surface);
    }
	
	//Init game credits screen texts
    for(int i = 0; i < credits_menu_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        switch (i) {
            case 0:
                text = "Credits";
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1:
                text = "Made by:";
                rect = (SDL_Rect){640 - BUTTON_MENU_WIDTH, 360 - BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT * 2};
                break;
                
            case 2:
                text = "    Danilo Makoto Ikuta";
                rect = (SDL_Rect){640 - BUTTON_MENU_WIDTH, 360 - BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH * 2, BUTTON_MENU_HEIGHT * 2};
                break;
                
            case 3:
                text = "    Gabriel Fontenelle";
                rect = (SDL_Rect){640 - BUTTON_MENU_WIDTH, 360, BUTTON_MENU_WIDTH * 2, BUTTON_MENU_HEIGHT * 2};
                break;
                
            case 4:
                text = "    Gabriel Nopper";
                rect = (SDL_Rect){640 - BUTTON_MENU_WIDTH, 360 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH * 2, BUTTON_MENU_HEIGHT * 2};
                break;
                
            case 5: case 6:
                text = "Back";
                rect = (SDL_Rect){595, 650, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        credits_menu_rects[i] = rect;
        
        SDL_Surface *surface;
        if(i == 6)
            surface = TTF_RenderText_Solid(font, text, red);
        else
            surface = TTF_RenderText_Solid(font, text, black);
        
        if(!surface){
            printf("(Credits)Text not rendered! %s\n", TTF_GetError());
            return false;
        }
        
        credits_menu_assets[i] = SDL_CreateTextureFromSurface(renderer, surface);
        if(!credits_menu_assets[i]){
            printf("(Credits)Erro ao criar textura! %s\n", SDL_GetError());
            return false;
        }
        
        SDL_FreeSurface(surface);
    }
	
	//Init game score screen texts
    for(int i = 0; i < score_menu_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        switch (i) {
            case 0:
                text = "Score";
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1: case 2:
                text = "Back";
                rect = (SDL_Rect){595, 650, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        score_menu_rects[i] = rect;
        
        SDL_Surface *surface;
        if(i == 2)
            surface = TTF_RenderText_Solid(font, text, red);
        else
            surface = TTF_RenderText_Solid(font, text, black);
        
        if(!surface){
            printf("(Score)Text not rendered! %s\n", TTF_GetError());
            return false;
        }
        
        score_menu_assets[i] = SDL_CreateTextureFromSurface(renderer, surface);
        if(!score_menu_assets[i]){
            printf("(Score)Erro ao criar textura! %s\n", SDL_GetError());
            return false;
        }
        
        SDL_FreeSurface(surface);
    }
    
    //Init game over screen texts
    for(int i = 0; i < game_over_interface_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        switch (i) {
            case 0:
                rect = (SDL_Rect){0, 0, 1280, 720};
                break;
                
            case 1: case 2:
                text = "Retry";
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3: case 4:
                text = "Main Menu";
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
            
            case 5: case 6:
                text = "Quit";
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 5, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        game_over_interface_rects[i] = rect;
        
        SDL_Surface *surface;
        if(i == 0){
            surface = IMG_Load("../images/Game Over.png");
            if(!surface){
                printf("(Game over)Erro ao carregar game over menu! %s\n", IMG_GetError());
                return false;
            }
        }
        
        else{
            if(i%2 == 0 && i > 0)
                surface = TTF_RenderText_Solid(font, text, red);
            else
                surface = TTF_RenderText_Solid(font, text, white);
            
            if(!surface){
                printf("(Game over)Text not rendered! %s\n", TTF_GetError());
                return false;
            }
        }
        
        game_over_interface_assets[i] = SDL_CreateTextureFromSurface(renderer, surface);
        if(!game_over_interface_assets[i]){
            printf("(Game over)Erro ao criar textura! %s\n", SDL_GetError());
            return false;
        }
        
        SDL_FreeSurface(surface);
    }
	
	//Init map
    map_Surface = init_map();
    if(!map_Surface){
        printf("Falha ao inicializar mapa!\n");
        return false;
    }
    
    //Init stats
    reset_game_data();
    
    //Init interface static assets(Pause button and right bar)
    SDL_Surface *pause_surface = IMG_Load("../images/Pause.png");
    if(!pause_surface){
        printf("Falha ao carregar botão de pause! %s\n", IMG_GetError());
        return false;
    }
    
    game_interface_assets[0] = SDL_CreateTextureFromSurface(renderer, pause_surface);
    if(!game_interface_assets[0]){
        printf("Falha ao criar textura de botão de pause! %s\n", SDL_GetError());
        return false;
    }
    
    SDL_FreeSurface(pause_surface);
    
    game_interface_rects[0] = (SDL_Rect){BUTTON_MENU_HEIGHT, 0, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT};
    
    SDL_Surface *right_bar_surface = IMG_Load("../images/Right Bar.png");
    if(!right_bar_surface){
        printf("Falha ao carregar right bar! %s\n", IMG_GetError());
        return false;
    }
    
    game_interface_assets[1] = SDL_CreateTextureFromSurface(renderer, right_bar_surface);
    if(!game_interface_assets[1]){
        printf("Falha ao criar textura de right bar");
        return false;
    }
    
    SDL_FreeSurface(right_bar_surface);
    
    game_interface_rects[1] = (SDL_Rect){1095, 0, 185, 720};
    
    return true;
}

//Encerra SDL
void main_quit(){
    //Close fonts
    if(font)
        TTF_CloseFont(font);
    
    //Destroy textures
    for(int i = 0; i < main_menu_assets_count; i++){
        if(main_menu_assets[i])
            SDL_DestroyTexture(main_menu_assets[i]);
    }
    
    for(int i = 0; i < config_menu_assets_count; i++){
        if(config_menu_assets[i])
            SDL_DestroyTexture(config_menu_assets[i]);
    }
    
    for(int i = 0; i < game_interface_assets_count; i++){
        if(game_interface_assets[i])
            SDL_DestroyTexture(game_interface_assets[i]);
    }
    
    for(int i = 0; i < pause_interface_assets_count; i++){
        if(pause_interface_assets[i])
            SDL_DestroyTexture(pause_interface_assets[i]);
    }
    
    for(int i = 0; i < credits_menu_assets_count; i++){
        if(credits_menu_assets[i])
            SDL_DestroyTexture(credits_menu_assets[i]);
    }
    
    for(int i = 0; i < score_menu_assets_count; i++){
        if(score_menu_assets[i])
            SDL_DestroyTexture(score_menu_assets[i]);
    }
    
    for(int i = 0; i < game_over_interface_assets_count; i++){
        if(game_over_interface_assets[i])
            SDL_DestroyTexture(game_over_interface_assets[i]);
    }
    
    //Free surfaces
    if(map_Surface)
        SDL_FreeSurface(map_Surface);
    
    //Free window
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(main_Window)
        SDL_DestroyWindow(main_Window);
    
    //Quit SDL, TTF, IMG
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
	
	//Free config
	//free(config->language);
    
    if(config)
        free(config);
    
    reset_game_data();
}

//Carrega textos do menu de configurações
void get_config(){
    for(int i = 0; i < config_menu_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        //Set texts and rects
        switch(i){
            case 0:
                text = "Config";
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1: case 2:
                if(config->audio_sfx)
                    text = "Sound Effects:  ON";
                else
                    text = "Sound Effects:  OFF";
                
                rect = (SDL_Rect){595, 150, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3: case 4:
                if(config->audio_music)
                    text = "Music:  ON";
                else
                    text = "Music:  OFF";
                
                rect = (SDL_Rect){595, 150 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 5: case 6:
                text = "Language";
                
                rect = (SDL_Rect){595, 150 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 7: case 8:
                text = "Back";
                
                rect = (SDL_Rect){595, 150 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        config_menu_rects[i] = rect;
        
        SDL_Surface *surface;
        if(i%2 == 0 && i > 0)
            surface = TTF_RenderText_Solid(font, text, red);
        else
            surface = TTF_RenderText_Solid(font, text, black);
        
        if(!surface){
            printf("(Config)Text not rendered! %s\n", TTF_GetError());
            return;
        }
        
        config_menu_assets[i] = SDL_CreateTextureFromSurface(renderer, surface);
        
        if(!config_menu_assets[i]){
            printf("(Config)Text texture not rendered! %s\n", SDL_GetError());
            return;
        }
        
        SDL_FreeSurface(surface);
    }
}

//Reseta listas, e dados do jogo
void reset_game_data(){
    //Free lists
    if(minions)
        free_list_minion(minions);
    if(turrets)
        free_list_turret(turrets);
    /*if(projectiles)
     free_list_projectile(projectiles);*/
    
    //Init lists
    minions = init_list_minion();
    if(!minions){
        printf("Erro em init_list_minion!\n");
    }
    
    turrets = init_list_turret();
    if(!turrets){
        printf("Erro em init_list_turret!\n");
    }
    
    /*projectiles = init_list_projectile();
     if(!projectiles){
     printf("Erro em init_list_projectiles!\n");
     return false;
     }*/
    
    health = 5;
    gold = 1000;
    mana = 0;
}