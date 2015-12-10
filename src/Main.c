//
//  main.c
//  PI6 - TowerDefense
//
//  Created by Danilo Makoto Ikuta on 10/09/15.
//  Copyright (c) 2015 Danilo Makoto Ikuta. All rights reserved.
//

//Main Menu (Loads SDL and main menu assets)

#include "Renderer.h"
#include "GameScene.h"
#include "Connection.h"
#include "Language.h"

#ifdef _WIN32
	#include <windows.h>
	#include <Lmcons.h>
#else
	#include <unistd.h>
#endif

#define main_menu_assets_count 13
#define config_menu_assets_count 9
#define game_interface_assets_count 15
#define pause_interface_assets_count 13
#define credits_menu_assets_count 7
#define score_menu_assets_count 3
#define end_game_interface_assets_count 8
#define multiplayer_menu_assets_count 31

#define rooms_page_offset 4
#define players_page_offset 3

#define FRAMES_PER_SEC 60

//Global variable
///////////////////////////////////////////////////////////////////////
Threads *thread_control;
ShareData *data_shared;
Languages *lang;
Configuration *config;

//SDL stuff
SDL_Window *main_Window;
SDL_Surface *main_Surface;
SDL_Renderer *renderer;

//Media
TTF_Font *font;
TTF_Font *title;

SDL_Surface *map_Surface;
SDL_Texture *select_Texture;
SDL_Texture *background_Texture;

//Constants
SDL_Color black = {0, 0, 0, 255};
SDL_Color white = {255, 255, 255, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color red = {255, 0, 0, 255};

//Static texts
//Main menu order: title, play(+sel), multiplayer(+sel), config(+sel), score(+sel), credits(+sel), exit(+sel)
SDL_Texture *main_menu_assets[main_menu_assets_count];
SDL_Rect main_menu_rects[main_menu_assets_count];

//Config order: Config, sfx(+sel), music(+sel), language(+sel), back(+sel)
SDL_Texture *config_menu_assets[config_menu_assets_count];
SDL_Rect config_menu_rects[config_menu_assets_count];

//Game interface order: Pause button, Right bar, previous page, adversaries(4)(+sel), next page, quit(multiplayer)
SDL_Texture *game_interface_assets[game_interface_assets_count];
SDL_Rect game_interface_rects[game_interface_assets_count];

//Pause game interface order: overlay, resume(+sel), config(+sel), score(+sel), exit(+sel), main(+sel), credits(+sel)
SDL_Texture *pause_interface_assets[pause_interface_assets_count];
SDL_Rect pause_interface_rects[pause_interface_assets_count];

//Credits screen order: title, text(4 lines), text(6 lines), back(+sel)
SDL_Texture *credits_menu_assets[credits_menu_assets_count];
SDL_Rect credits_menu_rects[credits_menu_assets_count];

//Score screen order: title, back(+sel)
SDL_Texture *score_menu_assets[score_menu_assets_count];
SDL_Rect score_menu_rects[score_menu_assets_count];

//End Game order: overlay, retry(+sel), main menu(+sel), exit(+sel), msg type
SDL_Texture *end_game_interface_assets[end_game_interface_assets_count];
SDL_Rect end_game_interface_rects[end_game_interface_assets_count];

//Multiplayer screen order: title, create room/start/ready(+sel), search room/leave room(+sel), back(+sel),
//previous page(+sel), room list(4)(+sel), next page(+sel),
//rooms, players, ready?, player list(4), ready list(4), status
SDL_Texture *multiplayer_menu_assets[multiplayer_menu_assets_count];
SDL_Rect multiplayer_menu_rects[multiplayer_menu_assets_count];

//Pages index(Allow > 4 rooms/players)
int room_current_page = 0;
int players_current_page = 0;



list_minion *minions;
//list_projectile *projectiles;     Not used by now
list_turret *turrets;

int health = DEFAULT_PLAYERS_LIFE;
int gold = 1000;
int mana = 0;
float gold_per_second = 1;
int minions_left = 184;

//Avaliables
list_minion_avaliable *avaliable_minions;
list_turret_avaliable *avaliable_turrets;
list_projectile_avaliable *avaliable_projectiles;

//Cabeçalhos
bool main_init();
void main_quit();

void get_config_text();
void get_multiplayer_texts(multiplayer_status current_status, int page);
void set_end_game_status_text(end_game_status end_status);
void get_multiplayer_game_names(int page);
void reset_game_data();

//Socket structure
bool start_multiplay;
int players[4];

//Timer and spawn control
int show_timer = 0;
int timer_count = 0;
int spawn_minion = 0;

int main(int argc, char * argv[]) {
    bool quit = false;
    
    //Main init
    if(!main_init()){
        quit = true;
    }

	int i = 0, j = 0;
	time_t t;
	
	//Screen control
	screen current_screen = MAIN;
	screen previous_screen = MAIN;
	
	//Options control
	main_options main_option = OPT_NONE;
    config_options config_option = NONE;
	pause_options pause_option = OPT_P_NONE;
	Game_Running_Options running_option;
    end_game_options end_game_option = EG_NONE;
    multiplayer_menu_options multiplayer_option = MP_NONE;
    target_select_options target_option = TSO_NONE;
    
    multiplayer_status multiplayer_status = MPS_NONE;
    end_game_status end_status = EGS_NONE;
	
	running_option.current_tab = TOP_MENU;
	running_option.top = OPT_R_T_NONE;
	running_option.left = OPT_R_L_NONE;
	running_option.game_area.left = OPT_R_A_L_NONE;
	running_option.game_area.right = OPT_R_A_R_NONE;
	
	//Keyboard options control
	main_options select_option = OPT_PLAY;
	config_options select_config_option = AUDIO_SFX;
	pause_options select_pause_option = OPT_P_NONE;
	Game_Running_Options select_running_option;
    end_game_options select_end_game_option = EG_NONE;
    multiplayer_menu_options select_multiplayer_option = MP_NONE;
    target_select_options select_target_option = TSO_NONE;
	
	select_running_option.current_tab = TOP_MENU;
	select_running_option.top = OPT_R_T_NONE;
	select_running_option.left = OPT_R_L_NONE;
	select_running_option.game_area.left = OPT_R_A_L_NONE;
	select_running_option.game_area.right = OPT_R_A_R_NONE;
    select_running_option.multiplay.current_player = 0;
    select_running_option.multiplay.players = 0;
	
	//Game events
	SDL_Event event;
	bool game_started = false;
	bool game_paused = false;
	bool multiplayer = false;
	
	//Click control
	bool active_clicked = false;
    //bool run_action = false;
	bool left_click = true;
	
    bool return_to_previous_screen = false;     //For scores and credits menus
    bool select_return_to_previous_screen = false;     //For scores and credits menus
	
	
	//Game area control
    bool selected_left = false;//Right click equals to tower and left to minions.
	int add_tower = 0;
	int add_minion = 0;//Game add the minion.
	int send_minion = 0;//User send the minion.
	int ignore_next_command = 0;
	
	
	int current_position[] = {0,0};
	
	minion *new_minion;
	turret *new_turret;
	
	//Display control
	bool show_gold_info = false;
	bool show_mana_info = false;
	bool show_life_info = false;
	
	//Grid control   
	int select_grid = 0;
	int select_grid_option = 0;
	int click_grid = 0;
	int max_grid = 17 * 13;
	
	int grid_clicked[] = {0,0};
	int center_clicked[] = {0,0};
	int original_clicked[] = {0,0};
	
    // Following parts are only for first interation;
    int monsterSpawner[] = {6, 8, 12, 14, 17, 18, 25, 16, 18, 50};
	
	//Wave control - 20 seconds, after 
	int pending_wave_number = 0;
	int timer_minion = 20;
	
	int temp_option;
	
	//Thread
	char *thread_name = NULL;
	
	//Multiplayer
	Network network;
	network.searching = 0;
	network.searched = 0;
	network.connecting = 0;
	network.connection_failed = 0;
	network.servers = 0;
	network.choose_server = 0;
	network.server_choosed = -1;
	
	int player_adversary = 0;

	int ready_to_play = 0;
	
	//FPS and timer
    int t1, t2;
    int delay = 17; //Aprox. de 1000ms/60
	int frame = 0;
	
	int seed = 1;
	int total = 0;
	int not_started = 0;
	
	printf("%s\n", _("Here"));
	//int next = 0;  //unused
    //Main loop
    while(!quit){
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
									select_option = (select_option + 1) % 6;
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
									select_option = (select_option + 1) % 6;
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
								case SDLK_KP_ENTER: case SDLK_RETURN:
									//Check current selected option
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
									temp_option = (event.motion.y - 450) / BUTTON_MENU_HEIGHT;
									if (temp_option < OPT_CREDIT && temp_option >= 0) {
										main_option = temp_option;
									}
											
								}
								else if (event.motion.x >= 30 && event.motion.x <= 30 + BUTTON_MENU_WIDTH) {
									//Near credits 
									if(event.motion.y >= 450 + BUTTON_MENU_HEIGHT * 4 && event.motion.y <= 450 + BUTTON_MENU_HEIGHT * 5) {
										main_option = OPT_CREDIT;
									}
								}
							}
							break;
								
						case SDL_MOUSEMOTION:
							//Change select_option with mouse over data.
							if (event.motion.x >= 980 && event.motion.x <= 980 + BUTTON_MENU_WIDTH) {//Near main config
								temp_option = (event.motion.y - 450) / BUTTON_MENU_HEIGHT;
								if (temp_option < OPT_CREDIT && temp_option >= 0) {
									select_option = temp_option;
								}
							}
							else if (event.motion.x >= 30 && event.motion.x <= 30 + BUTTON_MENU_WIDTH) {
							//Near credits 
								if(event.motion.y >= 450 + BUTTON_MENU_HEIGHT * 4 && event.motion.y <= 450 + BUTTON_MENU_HEIGHT * 5) {
									select_option = OPT_CREDIT;
								}
							}			
							break;
					}
					break;
                    
                case GAME_MULTIPLAY_SERVER:
                    switch (event.type) {
                        case SDL_QUIT:
                            quit = true;
                            break;
                            
                        case SDL_KEYDOWN:
                            switch (event.key.keysym.sym) {
                                case SDLK_UP: case SDLK_LEFT:
                                    if(multiplayer_status == MPS_NONE){
                                        if(select_multiplayer_option == MP_CREATE_ROOM)
                                            select_multiplayer_option = MP_BACK_TO_MAIN;
                                        else if(select_multiplayer_option == MP_NONE)
                                            select_multiplayer_option = MP_BACK_TO_MAIN;
                                        else
                                            select_multiplayer_option --;
                                    }
                                    
                                    else if(data_shared->current_comm){
                                        if(data_shared->current_user->is_server){
                                            if(select_multiplayer_option == MP_START)
                                                select_multiplayer_option = MP_BACK_TO_MAIN;
                                            else if(select_multiplayer_option == MP_BACK_TO_MAIN)
                                                select_multiplayer_option = MP_LEAVE;
                                            else if(select_multiplayer_option == MP_LEAVE)
                                                select_multiplayer_option = MP_START;
                                        }
                                        
                                        else if(multiplayer_status == MPS_ENTERED_ROOM){
                                            if(select_multiplayer_option == MP_TOGGLE_READY)
                                                select_multiplayer_option = MP_BACK_TO_MAIN;
                                            else if(select_multiplayer_option == MP_BACK_TO_MAIN)
                                                select_multiplayer_option = MP_LEAVE;
                                            else if(select_multiplayer_option == MP_LEAVE)
                                                select_multiplayer_option = MP_TOGGLE_READY;
                                        }
                                        
                                        else if(multiplayer_status == MPS_SEARCHING_ROOM){
                                            if(select_multiplayer_option <= MP_NEXT_PAGE && select_multiplayer_option >= MP_ROOM_BTN_1)
                                                select_multiplayer_option --;
                                            else if(select_multiplayer_option == MP_PREVIOUS_PAGE)
                                                select_multiplayer_option = MP_BACK_TO_MAIN;
                                            else if(select_multiplayer_option == MP_BACK_TO_MAIN)
                                                select_multiplayer_option = MP_NEXT_PAGE;
                                            else
                                                select_multiplayer_option = MP_NEXT_PAGE;
                                        }
                                    }
                                    
                                    break;
                                    
                                case SDLK_DOWN: case SDLK_RIGHT:
                                    if(multiplayer_status == MPS_NONE){
                                        if(select_multiplayer_option == MP_BACK_TO_MAIN)
                                            select_multiplayer_option = MP_CREATE_ROOM;
                                        else if(select_multiplayer_option == MP_NONE)
                                            select_multiplayer_option = MP_CREATE_ROOM;
                                        else
                                            select_multiplayer_option ++;
                                    }
                                    
                                    else if(data_shared->current_comm){
                                        if(data_shared->current_user->is_server){
                                            if(select_multiplayer_option == MP_START)
                                                select_multiplayer_option = MP_LEAVE;
                                            else if(select_multiplayer_option == MP_BACK_TO_MAIN)
                                                select_multiplayer_option = MP_START;
                                            else if(select_multiplayer_option == MP_LEAVE)
                                                select_multiplayer_option = MP_BACK_TO_MAIN;
                                        }
                                        
                                        else if(multiplayer_status == MPS_ENTERED_ROOM){
                                            if(select_multiplayer_option == MP_TOGGLE_READY)
                                                select_multiplayer_option = MP_LEAVE;
                                            else if(select_multiplayer_option == MP_BACK_TO_MAIN)
                                                select_multiplayer_option = MP_TOGGLE_READY;
                                            else if(select_multiplayer_option == MP_LEAVE)
                                                select_multiplayer_option = MP_BACK_TO_MAIN;
                                        }
                                        
                                        else if(multiplayer_status == MPS_SEARCHING_ROOM){
                                            if(select_multiplayer_option <= MP_ROOM_BTN_4 && select_multiplayer_option >= MP_PREVIOUS_PAGE)
                                                select_multiplayer_option ++;
                                            else if(select_multiplayer_option == MP_NEXT_PAGE)
                                                select_multiplayer_option = MP_BACK_TO_MAIN;
                                            else if(select_multiplayer_option == MP_BACK_TO_MAIN)
                                                select_multiplayer_option = MP_PREVIOUS_PAGE;
                                            else
                                                select_multiplayer_option = MP_BACK_TO_MAIN;
                                        }
                                    }
                                    
                                    break;
                                    
                                default:
                                    break;
                            }
                            break;
                            
                        case SDL_KEYUP:
                            switch (event.key.keysym.sym) {
                                case SDLK_ESCAPE:
                                    quit = true;
                                    break;
                                    
                                case SDLK_KP_ENTER: case SDLK_RETURN:
                                    if(select_multiplayer_option != MP_NONE)
                                        multiplayer_option = select_multiplayer_option;
                                    break;
                                    
                                default:
                                    break;
                            }
                            break;
                            
                        case SDL_MOUSEBUTTONUP:
                            if(event.button.button == SDL_BUTTON_LEFT){
                                multiplayer_option = MP_NONE;
                                
                                if(event.motion.x >= 515 && event.motion.x <= 515 + BUTTON_MENU_WIDTH){
                                    temp_option = (event.motion.y - 150 - BUTTON_MENU_HEIGHT) / BUTTON_MENU_HEIGHT;
                                    if(temp_option <= MP_BACK_TO_MAIN && temp_option >= 0){
                                        if(multiplayer_status == MPS_NONE)
                                            multiplayer_option = temp_option;
                                        
                                        else if(data_shared->current_comm){
                                            if(data_shared->current_user->is_server){
                                                if(temp_option == 0)
                                                    multiplayer_option = MP_START;
                                            }
                                            
                                            else{
                                                if(temp_option == 0)
                                                    multiplayer_option = MP_TOGGLE_READY;
                                            }
                                            
                                            if(temp_option == 1)
                                                multiplayer_option = MP_LEAVE;
                                            else if(temp_option == 2)
                                                multiplayer_option = MP_BACK_TO_MAIN;
                                        }
                                    }
                                }
                                    
                                else if(event.motion.x >= 195 && event.motion.x <= 195 + BUTTON_MENU_WIDTH && multiplayer_status == MPS_SEARCHING_ROOM){
                                    temp_option = (event.motion.y - 300 - BUTTON_MENU_HEIGHT) / BUTTON_MENU_HEIGHT;
                                    switch(temp_option){
                                        case 0:
                                            multiplayer_option = MP_PREVIOUS_PAGE;
                                            break;
                                        case 1:
                                            multiplayer_option = MP_ROOM_BTN_1;
                                            break;
                                        case 2:
                                            multiplayer_option = MP_ROOM_BTN_2;
                                            break;
                                        case 3:
                                            multiplayer_option = MP_ROOM_BTN_3;
                                            break;
                                        case 4:
                                            multiplayer_option = MP_ROOM_BTN_4;
                                            break;
                                        case 5:
                                            multiplayer_option = MP_NEXT_PAGE;
                                            break;
                                        default:
                                            break;
                                    }
                                }
                                
                                
                            }
                            
                            break;
                            
                        case SDL_MOUSEMOTION:
                            if(event.motion.x >= 515 && event.motion.x <= 515 + BUTTON_MENU_WIDTH){
                                temp_option = (event.motion.y - 150 - BUTTON_MENU_HEIGHT) / BUTTON_MENU_HEIGHT;
                                if(temp_option <= MP_BACK_TO_MAIN && temp_option >= 0){
                                    if(multiplayer_status == MPS_NONE)
                                        select_multiplayer_option = temp_option;
                                    
                                    else if(data_shared->current_comm){
                                        if(data_shared->current_user->is_server){
                                            if(temp_option == 0)
                                                select_multiplayer_option = MP_START;
                                        }
                                        
                                        else{
                                            if(temp_option == 0)
                                                select_multiplayer_option = MP_TOGGLE_READY;
                                        }
                                        
                                        if(temp_option == 1)
                                            select_multiplayer_option = MP_LEAVE;
                                        else if(temp_option == 2)
                                            select_multiplayer_option = MP_BACK_TO_MAIN;
                                    }
                                }
                            }
                            
                            else if(event.motion.x >= 195 && event.motion.x <= 195 + BUTTON_MENU_WIDTH && multiplayer_status == MPS_SEARCHING_ROOM){
                                temp_option = (event.motion.y - 300 - BUTTON_MENU_HEIGHT) / BUTTON_MENU_HEIGHT;
                                switch(temp_option){
                                    case 0:
                                        select_multiplayer_option = MP_PREVIOUS_PAGE;
                                        break;
                                    case 1:
                                        select_multiplayer_option = MP_ROOM_BTN_1;
                                        break;
                                    case 2:
                                        select_multiplayer_option = MP_ROOM_BTN_2;
                                        break;
                                    case 3:
                                        select_multiplayer_option = MP_ROOM_BTN_3;
                                        break;
                                    case 4:
                                        select_multiplayer_option = MP_ROOM_BTN_4;
                                        break;
                                    case 5:
                                        select_multiplayer_option = MP_NEXT_PAGE;
                                        break;
                                        
                                    default:
                                        break;
                                }
                            }
                            
                            break;
                            
                        default:
                            break;
                    }
                    break;
                    
				case CONFIG:
					switch (event.type) {
						//Quit
						case SDL_QUIT:
							quit = true;
							break;
						case SDL_KEYDOWN:	
							switch(event.key.keysym.sym){
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
							break;
						//Handle keyboard event
						case SDL_KEYUP:
							switch(event.key.keysym.sym){
								//Backspace, Escape
                                case SDLK_BACKSPACE: case SDLK_ESCAPE:
									//Go back main screen or paused screem
									if (previous_screen == MAIN){
										current_screen = MAIN;
									}
									else {
										current_screen = GAME_PAUSED;
									}

								//Keypad enter
								case SDLK_KP_ENTER: case SDLK_RETURN:
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
							}
							break;
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
						case SDL_KEYDOWN:
							switch(event.key.keysym.sym){
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
												else if (multiplayer)  {
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
												select_grid-=17;
												if(select_grid < 0){
													select_grid = max_grid + select_grid;
												}
											}
											break;
										case ADVERSARY_MENU:
											if(select_target_option == TSO_PREVIOUS_PAGE)
                                                select_target_option = TSO_NEXT_PAGE;
                                            else if(select_target_option == TSO_NONE)
                                                select_target_option = TSO_NEXT_PAGE;
                                            else
                                                select_target_option--;
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
												else if (multiplayer) {
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
										case ADVERSARY_MENU:
                                            if(select_target_option == TSO_PREVIOUS_PAGE)
                                                select_target_option = TSO_NEXT_PAGE;
                                            else if(select_target_option == TSO_NONE)
                                                select_target_option = TSO_NEXT_PAGE;
                                            else
                                                select_target_option--;
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
												else if (multiplayer){
													select_running_option.game_area.right = (select_running_option.game_area.right + 1) % 3;
												}
											}
											else {
											//Move left mouse cursor from GAME_AREA. 
												select_grid = (select_grid + 1) % max_grid;
											}
											break;
										case ADVERSARY_MENU:
                                            if(select_target_option == TSO_NEXT_PAGE)
                                                select_target_option = TSO_PREVIOUS_PAGE;
                                            else if(select_target_option == TSO_NONE)
                                                select_target_option = TSO_PREVIOUS_PAGE;
                                            else
                                                select_target_option++;
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
												else if (multiplayer) {
													select_running_option.game_area.right = (select_running_option.game_area.right + 1) % 3;
												}
											}
											else {
											//Move down mouse cursor from GAME_AREA. 
												select_grid = (select_grid + 17) % max_grid;
											}
											break;
										case ADVERSARY_MENU:
                                            if(select_target_option == TSO_NEXT_PAGE)
                                                select_target_option = TSO_PREVIOUS_PAGE;
                                            else if(select_target_option == TSO_NONE)
                                                select_target_option = TSO_PREVIOUS_PAGE;
                                            else
                                                select_target_option++;
											break;
									}
									break;
							}
							break;
						//Handle keyboard release
						case SDL_KEYUP:
							switch(event.key.keysym.sym){
								//Backspace
								case SDLK_BACKSPACE:
									if(!multiplayer){
										current_screen = MAIN;
										previous_screen = GAME_RUNNING;
									}
									break;
								//Escape
								case SDLK_ESCAPE:
									//Show Game Pause screen with options
									if(!multiplayer){
										current_screen = GAME_PAUSED;
										select_pause_option = OPT_P_RESUME;
									}
                                    
                                    //Multiplayer quit
                                    else{
                                        if(thread_control){
                                            ignore_next_command = 1;
                                            SDL_AtomicLock(&thread_control->lock.control);
                                            if(thread_control->server.pointer || thread_control->server.alive){
                                                thread_control->server.terminate = 1;
                                                printf("Kill thread server %d %d!!\n", thread_control->server.pointer, thread_control->server.alive);
                                            }
                                            else if(thread_control->client.pointer || thread_control->client.alive){
                                                thread_control->client.terminate = 1;
                                                //printf("Kill thread client %d %d!!\n", thread_control->client.pointer, thread_control->client.alive);
                                            }
                                            else if(thread_control->udp.pointer || thread_control->udp.alive){
                                                thread_control->udp.terminate = 1;
                                                printf("Kill thread udp %d %d!!\n", thread_control->udp.pointer, thread_control->udp.alive);
                                            }
                                            else {
                                                printf("Threads killed!!\n");
                                                ignore_next_command = 0;
                                                multiplayer_status = MPS_NONE;
                                                
                                                current_screen = MAIN;
                                                previous_screen = GAME_RUNNING;
                                            }
                                            SDL_AtomicUnlock(&thread_control->lock.control);
                                        }
                                    }
                                    break;
								case SDLK_q:
									//printf("Key pressed: q\n");
									if (running_option.current_tab == GAME_AREA && !active_clicked){
										//Active click on current location
										active_clicked = true;
										selected_left = true;
									}
									break;
								case SDLK_e:
									//printf("Key pressed: e\n"); 
									if (running_option.current_tab == GAME_AREA && !active_clicked && multiplayer){
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
													if(!multiplayer){
														current_screen = GAME_PAUSED;
														game_paused = true;
													}
													break;
												case OPT_R_T_RESUME:
													if(!multiplayer){
														current_screen = GAME_RUNNING;
														game_paused = false;
													}
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
												else if (multiplayer) {
													send_minion = select_running_option.game_area.left + 1;
													get_cartesian_from_grid_number(select_grid, current_position, 17);
												}
											}
											break;
										case ADVERSARY_MENU:
                                            target_option = select_target_option;
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
													if(!multiplayer){
													//Dont do any action after this, only render the scene. 
														current_screen = GAME_PAUSED;
														game_paused = true;
													}
													break;
												case OPT_R_T_RESUME:
													if(!multiplayer){
														current_screen = GAME_RUNNING;
														game_paused = false;
													}
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
												else if (multiplayer) {
													send_minion = select_running_option.game_area.left + 1;
													get_cartesian_from_grid_number(select_grid, current_position, 17);
												}
											}
											break;
										case ADVERSARY_MENU:
                                            target_option = select_target_option;
											break;
									}
									break;
								case SDLK_TAB:
									//Move selector location
									if(!multiplayer) {
										running_option.current_tab = (running_option.current_tab + 1) % 4;
									}
									else {
										running_option.current_tab = (running_option.current_tab + 1) % 5;
									}
									break;
							}
							break;
						//Mouse motion
						case SDL_MOUSEMOTION:
							//Move mouse selector over game area
							if (!active_clicked && get_touched_grid_address(event.motion.x, event.motion.y, grid_clicked)){
								select_grid = get_grid_address_linear(grid_clicked[0], grid_clicked[1], 17);
							}
							
							//Move mouse selector over multiplayer users
                            if(event.motion.x >= 1097 && event.motion.x <= 1277){
                                temp_option = (event.motion.y - 350) / BUTTON_MENU_HEIGHT;
                                if(temp_option >= TSO_PREVIOUS_PAGE && temp_option <= TSO_NEXT_PAGE)
                                    select_target_option = temp_option;
                            }
							
							break;
                            
						//Handle mouse event
						case SDL_MOUSEBUTTONUP:
							if(event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT){
								left_click = event.button.button == SDL_BUTTON_LEFT;
								
								if (!left_click && !multiplayer){
									//printf("Do nothing\n");
								}
								//Game area
								else if(get_touched_grid_address(event.motion.x, event.motion.y, grid_clicked)){
									if (!active_clicked){
										current_position[0] = grid_clicked[0];
										current_position[1] = grid_clicked[1];
									}
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
									else if (get_touched_menu_address(event.motion.x, event.motion.y, center_clicked, &select_grid_option, selected_left, avaliable_minions, avaliable_turrets)){
										if (selected_left) {
											add_tower = select_grid_option;
											//*original_clicked = *center_clicked;
										}
										else {
											send_minion = select_grid_option;
											//*original_clicked = *center_clicked;
										}
										active_clicked = false;
									}
									else {
										active_clicked = false;
									}
								}

								//Top menu
								else if(left_click && event.motion.y >= 0 && event.motion.y <= BUTTON_MENU_HEIGHT){
									if (event.motion.x >= BUTTON_MENU_HEIGHT && event.motion.x <= BUTTON_MENU_HEIGHT * 2){
                                        if(!multiplayer){
                                            //Pause game
                                            game_paused = !game_paused;
                                            //Not sure if must use current_screen to show the pause screen or just change status to make network request.
                                            current_screen = GAME_PAUSED;
                                        }
										
                                        else{
                                            if(thread_control){
                                                ignore_next_command = 1;
                                                SDL_AtomicLock(&thread_control->lock.control);
                                                if(thread_control->server.pointer || thread_control->server.alive){
                                                    thread_control->server.terminate = 1;
                                                    printf("Kill thread server %d %d!!\n", thread_control->server.pointer, thread_control->server.alive);
                                                }
                                                else if(thread_control->client.pointer || thread_control->client.alive){
                                                    thread_control->client.terminate = 1;
                                                    //printf("Kill thread client %d %d!!\n", thread_control->client.pointer, thread_control->client.alive);
                                                }
                                                else if(thread_control->udp.pointer || thread_control->udp.alive){
                                                    thread_control->udp.terminate = 1;
                                                    printf("Kill thread udp %d %d!!\n", thread_control->udp.pointer, thread_control->udp.alive);
                                                }
                                                else {
                                                    printf("Threads killed!!\n");
                                                    ignore_next_command = 0;
                                                    multiplayer_status = MPS_NONE;
                                                    
                                                    current_screen = MAIN;
                                                    previous_screen = GAME_RUNNING;
                                                }
                                                SDL_AtomicUnlock(&thread_control->lock.control);
                                            }
                                        }
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
								//Outside game area menu active_clicked.
								else if(active_clicked){
									if (get_touched_menu_address(event.motion.x, event.motion.y, center_clicked, &select_grid_option, selected_left, avaliable_minions, avaliable_turrets)){
										if (selected_left) {
											add_tower = select_grid_option;
											*original_clicked = *center_clicked;
										}
										else {
											send_minion = select_grid_option;
											*original_clicked = *center_clicked;
										}
										active_clicked = false;
									}
								}
                                
                                //Click mouse selector over multiplayer users
                                if(event.motion.x >= 1097 && event.motion.x <= 1277){
                                    temp_option = (event.motion.y - 350) / BUTTON_MENU_HEIGHT;
                                    if(temp_option >= TSO_PREVIOUS_PAGE && temp_option <= TSO_NEXT_PAGE)
                                        target_option = temp_option;
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
							
						case SDL_KEYDOWN:
							switch(event.key.keysym.sym){
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
								case SDLK_KP_ENTER: case SDLK_RETURN:
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
							}
							break;
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
					break;
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
                    
                case END_GAME:
                    switch (event.type) {
                        case SDL_QUIT:
                            quit = true;
                            break;
                            
                        case SDL_KEYUP:
                            switch (event.key.keysym.sym) {
                                case SDLK_ESCAPE:
                                    quit = true;
                                    break;
                                    
                                case SDLK_KP_ENTER: case SDLK_RETURN:
                                    if(select_end_game_option != EG_NONE)
                                        end_game_option = select_end_game_option;
                                    break;
                                    
                                case SDLK_UP: case SDLK_RIGHT:
                                    if(select_end_game_option == EG_NEW_GAME)
                                        select_end_game_option = EG_QUIT;
                                    else
                                        select_end_game_option++;
                                    break;
                                    
                                case SDLK_DOWN: case SDLK_LEFT:
                                    if(select_end_game_option == EG_QUIT)
                                        select_end_game_option = EG_NEW_GAME;
                                    else
                                        select_end_game_option--;
                                    break;
                                    
                                default:
                                    break;
                            }
                            break;
                            
                        case SDL_MOUSEBUTTONUP:
                            if(event.button.button == SDL_BUTTON_LEFT){
                                end_game_option = EG_NONE;
                                
                                if (event.motion.x >= 515 && event.motion.x <= 765) {
                                    temp_option = (event.motion.y - 360) / BUTTON_MENU_HEIGHT;
                                    if (temp_option <= EG_QUIT && temp_option >= 0) {
                                        end_game_option = temp_option;
                                    }
                                }
                            }
                            break;
                            
                        case SDL_MOUSEMOTION:
                            if (event.motion.x >= 515 && event.motion.x <= 765) {
                                temp_option = (event.motion.y - 360) / BUTTON_MENU_HEIGHT;
                                if (temp_option <= EG_QUIT && temp_option >= 0) {
                                    select_end_game_option = temp_option;
                                }
                            }
                            break;
                            
                        default:
                            break;
                    }
                    
                    break;
				case MULTIPLAYER_CHOOSE_ROOM:
					//Need to do.
					break;
				
				case MULTIPLAYER_ROOM:
					//Need to do.
					
					break;
                default:
                    
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
				//printf("Game started\n");
				//One second timer
				timer_count++;
					
				//Add more gold 1
				gold += gold_per_second;
				//Add more mana 1
				mana++;
					
				//Wave spawning. Only to single player.
				if(pending_wave_number > 0 && !multiplayer) {
					srand((unsigned) time(&t));
					total = get_minion_avaliable(avaliable_minions);

					if(seed > total){
						seed = total;
					}

					add_minion = rand() % seed;
					printf("Seed %d\n", seed);
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
				if (is_time(timer_count, timer_minion) && !multiplayer){
					//New wave
					pending_wave_number = monsterSpawner[spawn_minion];
					timer_minion = pending_wave_number + 20;
					spawn_minion++;
					seed++;
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
			
		//Network Thread Status Checker
		/////////////////////////////////////////////////////
		if(multiplayer) {
			SDL_AtomicLock(&thread_control->lock.comm);
			//-- Change, dont need this, use the global variable instead.
			if(data_shared->current_comm){
				if(data_shared->current_comm->server->searching){
					network.searching = 1;
				}
				if(data_shared->current_comm->server->searching_finished){
					network.searched = 1;
				}
				if(data_shared->current_comm->server->search_result > 0){
					if(data_shared->current_comm->server->connecting){
						network.connecting = 1;
					}
					network.servers = data_shared->current_comm->server->search_result;
					for(i = 0; i < network.servers; i++){
						strncpy(network.server_name[i], data_shared->current_comm->server->host[i].name, SERVER_NAME);
					}
				}
				if(data_shared->current_comm->server->choosing) {
					network.choose_server = 1;
				}
				
				if(data_shared->current_comm->server->connection_failed || data_shared->current_comm->connection_lost) {
					//Set current screen
					current_screen = END_GAME;
					//network.connection_failed = 1;
					printf("Lost comm\n");
					if(data_shared->current_comm->server->connection_failed){
						end_status = EGS_DC;
						printf("Connection failed\n");
					}
					else {
						printf("Connection lost\n");
						end_status = EGS_OPLEFT;
					}
					game_started = false;
					
					//Reset multiplay in back to menu or other.
					
				}
				
				if(data_shared->current_comm->match->can_start && !not_started){
					not_started = 1;
					game_started = true;
					current_screen = GAME_RUNNING;
				}
				
				if(data_shared->current_comm->match->finished) {
					not_started = 0;
					
					//Set screen
					
					
					//data_shared->current_comm->match->winner_id;
				}
			}
			SDL_AtomicUnlock(&thread_control->lock.comm);
			
			//Set minions to send.
			if(send_minion > 0) {
                int price = get_minion_price(avaliable_minions, send_minion);
                if(gold >= price){
                    gold -= price;
                    gold_per_second += get_minion_bonus(avaliable_minions, send_minion);
                
                
				//Get adversary id
				SDL_AtomicLock(&thread_control->lock.comm);
				int adversary_id_to_use =  data_shared->current_comm->adversary[player_adversary % data_shared->current_comm->match->players].id;
				SDL_AtomicUnlock(&thread_control->lock.comm);
				
				SDL_AtomicLock(&thread_control->lock.user);
				if(data_shared->current_user->minions && data_shared->current_user->spawn_amount){
					int adversary_found = 0;
					//Realloc minions.
					for(i = 0; i < data_shared->current_user->spawn_amount;i++){
						if(data_shared->current_user->minions[i].client_id == adversary_id_to_use){
							//User found
							adversary_found = 1;
							//Add minion to adversary array. Realloc array.
							int *new_type = malloc(sizeof(int) * data_shared->current_user->minions[i].amount + 1);
							for(int j = 0; j < data_shared->current_user->minions[i].amount;j++){
								new_type[j] = data_shared->current_user->minions[i].type[j];//Need to check if this is right.
							}
							new_type[j] = send_minion;
							
							free(data_shared->current_user->minions[i].type);
							data_shared->current_user->minions[i].type = new_type;
							data_shared->current_user->minions[i].amount += 1;
							break;
						}
					}
					
					if(!adversary_found){
						SpawnMinion *new_spawn = malloc(sizeof(SpawnMinion) * data_shared->current_user->spawn_amount + 1);
						//Realloc minions.
						for(i = 0; i < data_shared->current_user->spawn_amount;i++){
							new_spawn[i] = data_shared->current_user->minions[i];
						}
						new_spawn[i].client_id = adversary_id_to_use;
						new_spawn[i].amount =  1;
						new_spawn[i].type = malloc(sizeof(int));
						
						new_spawn[i].type[0] = send_minion;
						free(data_shared->current_user->minions);
						data_shared->current_user->minions = new_spawn;
						data_shared->current_user->spawn_amount +=1;
					}				
				}
				else {
					data_shared->current_user->spawn_amount = 1;
					SpawnMinion *spawn_minium = malloc(sizeof(SpawnMinion));//Only one now.
					spawn_minium[0].amount = 1;
					spawn_minium[0].client_id = adversary_id_to_use;//Mod in case players amount changed.
					spawn_minium[0].type = malloc(sizeof(int));
					spawn_minium[0].type[0] = send_minion;
					
					if(data_shared->current_user->minions){
						free(data_shared->current_user->minions);
					}
					data_shared->current_user->minions = spawn_minium;
					}
					printf("Minions to send %d (amount user)\n", data_shared->current_user->spawn_amount);
					data_shared->current_user->process.message_minion++;
				}
				send_minion = 0;
				SDL_AtomicUnlock(&thread_control->lock.user);
			}
			//-- Change
			//Check if a connection running was failed. This will kill the thread.
			
			SDL_AtomicLock(&thread_control->lock.comm);
			if(data_shared->current_comm && data_shared->current_comm->server && data_shared->current_comm->server->connected){
                multiplayer_status = MPS_ENTERED_ROOM;
			}
			SDL_AtomicUnlock(&thread_control->lock.comm);
			
			/*
			if(data_shared->current_comm->server->connection_failed || data_shared->current_comm->connection_lost){
				
			}
			*/
			
			if(network.connection_failed){
				//Kill threads.
				SDL_AtomicLock(&thread_control->lock.control);
				thread_control->udp.terminate = 1;
				thread_control->server.terminate = 1;
				thread_control->client.terminate = 1;
				thread_control->client.pointer = NULL;
				thread_control->server.pointer = NULL;
				thread_control->udp.pointer = NULL;
				SDL_AtomicUnlock(&thread_control->lock.control);
				
				//After this the thread will be dead must use network.connection_failed to show message with renderer.
				//- TODO:
				// [ ] Change screen to connection failed.
				
				multiplayer_status = MPS_NONE;
				multiplayer = false;
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
                        break;
                        
                    case OPT_MULTIPLAYER:
                        current_screen = GAME_MULTIPLAY_SERVER;
                        previous_screen = MAIN;
                        break;
                        
					case OPT_NONE:
						//Do nothing.
						break;
				}
                main_option = OPT_NONE;
				break;
                
            case GAME_MULTIPLAY_SERVER:
                switch (multiplayer_option){
                    case MP_CREATE_ROOM:
						multiplayer_status = MPS_SEARCHING_ROOM;
						
						if(thread_control){
							if(thread_control->client.pointer || thread_control->client.alive){
								SDL_AtomicLock(&thread_control->lock.control);
								//Wait until thread is killed.
								thread_control->client.terminate = 1;
								ignore_next_command = 1;
								SDL_AtomicUnlock(&thread_control->lock.control);
							}
							//Init thread.
							else if(!thread_control->server.pointer && !thread_control->server.alive){
								multiplayer_status = MPS_WAIT_FOR_PLAYER;
								
								SDL_AtomicLock(&thread_control->lock.comm);
								if(!data_shared->current_comm) {
									printf("Initing\n");
									data_shared->current_comm = init_communication();
								}
								SDL_AtomicUnlock(&thread_control->lock.comm);
							
								//Create thread
								thread_control->server.pointer = SDL_CreateThread((SDL_ThreadFunction) run_server, "run_server", (void *)NULL);
								if (thread_control->server.pointer == NULL) {
									ignore_next_command = 1;
									multiplayer = false;
									//multiplayer_status = MPS_NONE;
									remove_communication();
								}
								else {
									ignore_next_command = 0;
									multiplayer = true;
									//multiplayer_status = MPS_WAIT_READY;
								}
								printf("Here problem\n");
							}
							//Loop again.
							else if(!ignore_next_command) {
								thread_control->server.terminate = 1;
								ignore_next_command = 1;
							}
						}
                        break;
                        
                    case MP_SEARCH_ROOM:
                        multiplayer_status = MPS_SEARCHING_ROOM;

						printf("search\n");
						if(thread_control){
							if(thread_control->server.pointer || thread_control->server.alive){
								SDL_AtomicLock(&thread_control->lock.control);
								//Wait until thread is killed.
								thread_control->server.terminate = 1;
								ignore_next_command = 1;
								SDL_AtomicUnlock(&thread_control->lock.control);
							}
							//Init thread.
							else if(!thread_control->client.pointer && !thread_control->client.alive){
								//multiplayer_status = MPS_WAIT_FOR_PLAYER;
								
								SDL_AtomicLock(&thread_control->lock.comm);
								if(!data_shared->current_comm) {
									printf("Initing\n");
									data_shared->current_comm = init_communication();
								}
								SDL_AtomicUnlock(&thread_control->lock.comm);
							
								//Create thread
								thread_control->client.pointer = SDL_CreateThread((SDL_ThreadFunction) run_client, "run_client", (void *)NULL);
								if (thread_control->client.pointer == NULL) {
									ignore_next_command = 1;
									multiplayer = false;
									multiplayer_status = MPS_NONE;
									remove_communication();
								}
								else {
									ignore_next_command = 0;
									multiplayer = true;
									//multiplayer_status = MPS_WAIT_READY;
								}
	
							}
							//Loop again.
							else if(!ignore_next_command) {
								thread_control->client.terminate = 1;
								ignore_next_command = 1;
							}
						}
                        break;

                    case MP_START:
                        // ready_to_play = 1;
						if(data_shared){
							multiplayer_status = MPS_STARTED_GAME;
							SDL_AtomicLock(&thread_control->lock.user);
							data_shared->current_user->process.message_status = data_shared->current_user->process.message_status + 1;
							data_shared->current_user->ready_to_play = 1;
							SDL_AtomicUnlock(&thread_control->lock.user);
                        }
						else {
							printf("Something that was not supose to happen happened.\n");
						}
						break;
                        
                    case MP_TOGGLE_READY:
						if(data_shared){
							SDL_AtomicLock(&thread_control->lock.user);
							data_shared->current_user->process.message_status = data_shared->current_user->process.message_status + 1;
							data_shared->current_user->ready_to_play = (data_shared->current_user->ready_to_play + 1) % 2;
							SDL_AtomicUnlock(&thread_control->lock.user);
						}
						else {
							printf("Something that was not supose to happen happened.\n");
						}
					    break;
                    
					case MP_BACK_TO_MAIN:
                        //Same as cancel option
                        
						if(thread_control){
							ignore_next_command = 1;
							SDL_AtomicLock(&thread_control->lock.control);
							if(thread_control->server.pointer || thread_control->server.alive){
								thread_control->server.terminate = 1;
								printf("Kill thread server %d %d!!\n", thread_control->server.pointer, thread_control->server.alive);
							}
							else if(thread_control->client.pointer || thread_control->client.alive){
								thread_control->client.terminate = 1;
								//printf("Kill thread client %d %d!!\n", thread_control->client.pointer, thread_control->client.alive);
							}
							else if(thread_control->udp.pointer || thread_control->udp.alive){
								thread_control->udp.terminate = 1;
								printf("Kill thread udp %d %d!!\n", thread_control->udp.pointer, thread_control->udp.alive);
							}
							else {
								printf("Threads killed!!\n");
								ignore_next_command = 0;
								multiplayer_status = MPS_NONE;
								
								current_screen = MAIN;
								previous_screen = GAME_MULTIPLAY_SERVER;
							}
							SDL_AtomicUnlock(&thread_control->lock.control);
						}
                        break;    
                    case MP_LEAVE:
                        //Leave room
						if(thread_control){
							ignore_next_command = 1;
							SDL_AtomicLock(&thread_control->lock.control);
							if(thread_control->server.pointer || thread_control->server.alive){
								thread_control->server.terminate = 1;
								//printf("Kill thread!!\n");
							}
							else if(thread_control->client.pointer || thread_control->client.alive){
								thread_control->client.terminate = 1;
								//printf("Kill thread!!\n");
							}
							else if(thread_control->udp.pointer || thread_control->udp.alive){
								thread_control->udp.terminate = 1;
								//printf("Kill thread!!\n");
							}
							else {
								printf("Threads killed!!\n");
								ignore_next_command = 0;
								multiplayer_status = MPS_NONE;
							}
							SDL_AtomicUnlock(&thread_control->lock.control);
						}
                        break;
                        
                    case MP_ROOM_BTN_1:
                        data_shared->current_comm->server->choosing = 0;
                        data_shared->current_comm->server->choosed = room_current_page * 4;
                        
                        break;
                        
                    case MP_ROOM_BTN_2:
                        data_shared->current_comm->server->choosing = 0;
                        data_shared->current_comm->server->choosed = room_current_page * 4 + 1;
                        
                        break;
                        
                    case MP_ROOM_BTN_3:data_shared->current_comm->server->choosing = 0;
                        data_shared->current_comm->server->choosed = room_current_page * 4 + 2;

                        break;
                        
                    case MP_ROOM_BTN_4:
                        data_shared->current_comm->server->choosing = 0;
                        data_shared->current_comm->server->choosed = room_current_page * 4 + 3;
                        
                       // if(data_shared->current_comm->server->connected)
                        //    multiplayer_status = MPS_ENTERED_ROOM;
                        break;
                        
                    case MP_PREVIOUS_PAGE:
                        if(room_current_page > 0)
                            room_current_page--;
                        break;
                        
                    case MP_NEXT_PAGE:
                        if(room_current_page < (MAX_SERVER - 1) / 4)
                            room_current_page++;
                        break;
                        
                    case MP_NONE:
                        break;
                        
                    default:
                        break;
                }
                
				//This will be used to show the same screen while thread not killed.
				if(!ignore_next_command)
					multiplayer_option = MP_NONE;
                
                //Get multiplayer menu assets
                get_multiplayer_texts(multiplayer_status, room_current_page);
                
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
                        get_config_text();
						break;
					case AUDIO_MUSIC:
						//Trogle ambience music
						if (config->audio_music == true) {
							config->audio_music = false;
						}
						else {
							config->audio_music = true;
						}
                        get_config_text();
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
				if(multiplayer){
					//if(data_shared->current_user->is_server)
					//	printf("SERVER\n");
					//else {
					//	printf("CLIENT\n");
						
					//}
					SDL_AtomicLock(&thread_control->lock.comm);
					//printf("Players %d\n",data_shared->current_comm->match->players);
					for(i = 0; i < data_shared->current_comm->match->players; i++){
						//printf("Pending minions %d\n", data_shared->current_comm->adversary[i].pending_minions);
						for(j = 0; j < data_shared->current_comm->adversary[i].pending_minions;j++){
							
							if(data_shared->current_comm->adversary[i].minions_sent[j] > 0){
								printf("Has minionnns %d\n", data_shared->current_comm->adversary[i].minions_sent[j]);
								//Add minion
								new_minion = init_minion(avaliable_minions, data_shared->current_comm->adversary[i].minions_sent[j]);     //minion_id not used
								if(new_minion != NULL){
									add_minion_to_list(minions, new_minion);
									new_minion->node->xPos = 150;
									new_minion->node->yPos = 600;
								}
							}
						}
						//free minions_sent
						
						if(data_shared->current_comm->adversary[i].minions_sent){
							free(data_shared->current_comm->adversary[i].minions_sent);
							data_shared->current_comm->adversary[i].minions_sent = NULL;
						}
						data_shared->current_comm->adversary[i].pending_minions = 0;
                        
                        switch (target_option) {
                            case TSO_PREVIOUS_PAGE:
                                if(players_current_page > 0)
                                    players_current_page--;
                                break;
                                
                            case TSO_TGT_1:
                                select_running_option.multiplay.current_player = players_current_page * 4;
                                break;
                                
                            case TSO_TGT_2:
                                select_running_option.multiplay.current_player = players_current_page * 4 + 1;
                                break;
                                
                            case TSO_TGT_3:
                                select_running_option.multiplay.current_player = players_current_page * 4 + 2;
                                break;
                                
                            case TSO_TGT_4:
                                select_running_option.multiplay.current_player = players_current_page * 4 + 3;
                                break;
                                
                            case TSO_NEXT_PAGE:
                                if(players_current_page < (MAX_CLIENT)/4)
                                    players_current_page++;
                                break;
                                
                            default:
                                break;
                        }
					}
					SDL_AtomicUnlock(&thread_control->lock.comm);
				}
				
				if (!game_paused){
					if (add_tower > 0){
						//Add tower
                        int price = get_turret_price(avaliable_turrets, add_tower);
                        if(gold > price){
                            //Check if position already has tower
                            if(occupyGrid(current_position[0], current_position[1])){
                                //Check if turret == Snorlax
                                if(!perform_path_verification(16, 5)){ // Blocking path.
                                    freeGrid(current_position[0], current_position[1]);
                                    perform_path_verification(16, 5);
                                }
                                else{ // SUCCESS
                                    new_turret = init_turret(avaliable_turrets, add_tower, current_position[0], current_position[1]);
                                    add_turret_to_list(turrets, new_turret);
                                    gold -= price;
                                }
                            }
                        }
						//Reset tower
						add_tower = 0;
					}
                    
					if (add_minion > 0){
						//Add minion
						new_minion = init_minion(avaliable_minions, add_minion);     //minion_id not used
						if(new_minion != NULL){
							add_minion_to_list(minions, new_minion);
                            new_minion->node->xPos = 150;
                            new_minion->node->yPos = 600;
						}
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
							health--;
							//Update player health if multiplayer
							if(multiplayer){
								SDL_AtomicLock(&thread_control->lock.user);
								data_shared->current_user->process.message_life++;
								data_shared->current_user->life = health;
								SDL_AtomicUnlock(&thread_control->lock.user);
							}
						}
						while (shoot && shoot->e) {
							if(move_bullet(enemy->e, shoot->e)){ // The movement is made in the if call.
								enemy->e->HP -= shoot->e->damage;
								remove_projectile_from_list(shoot, shoot->e);
								if(enemy->e->HP <= 0){
									minions_left--;
								}
							}
							else {
								shoot = shoot->next;
							}
						}
						
						if(enemy->e->HP <= 0){ // Death of minions
							if(enemy->e->targetted_projectils){
								//Remove list from minion.
								//free_list_projectile(enemy->e->targetted_projectils);
							}                            
							gold += get_minion_drop(avaliable_minions, enemy->e->minionType);
							remove_minion_from_list(enemy, &enemy->e);//This already get enemy->next
						}
						else {
							enemy = enemy->next;
						}
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
                                printf("Add targe\n");
								projectile *newShoot = NULL;
								newShoot = init_projectile(avaliable_projectiles, turret->e);
								printf("Created projectile\n");
								
                                if(newShoot)
									add_projectile_to_list(target->targetted_projectils, newShoot);
								
                                printf("Added projectile to list \n");

                                turret->e->timeUntilNextAttack = 1.0;
                            }
                        }
                        turret = turret->next;
                    }
                    
                    if(health <= 5){
                        current_screen = END_GAME;
                        game_started = false;
                        end_status = EGS_LOSE;
                    } 
					else if(!multiplayer){
						if(minions_left <= 0){
							current_screen = END_GAME;
							game_started = false;
							end_status = EGS_WIN;
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
                
            case END_GAME:
                switch (end_game_option) {
                    case EG_NEW_GAME:
                        reset_game_data();
						
						if(!multiplayer){
							current_screen = GAME_RUNNING;
							game_paused = false;
							game_started = true;
						}
						else {
							current_screen = GAME_MULTIPLAY_SERVER;
							game_started = false;
						}
                        break;
                        
                    case EG_MAIN:
                        reset_game_data();
                        current_screen = MAIN;
                        game_started = false;
                        break;
                        
                    case EG_QUIT:
                        quit = true;
                        break;
                        
                    default:
                        break;
                }
                
                set_end_game_status_text(end_status);
                
                end_game_option = EG_NONE;
                break;
            default:
                break;
            
		}		
        
		// Select the color for drawing.
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

        //Clear render
        SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, background_Texture, NULL, NULL);
		
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
                draw_screen_game_running(main_Surface, map_Surface, minions, turrets);
                
                screen_surfaces = SDL_CreateTextureFromSurface(renderer, main_Surface);
                SDL_RenderCopy(renderer, screen_surfaces, NULL, &(SDL_Rect){0, 0, 1280, 720});
			
                draw_screen_game_interface(renderer, game_interface_assets, game_interface_rects, game_interface_assets_count, select_running_option.multiplay.current_player, multiplayer);
                
				display_mouse(renderer, select_Texture, active_clicked, selected_left, click_grid, select_grid, center_clicked, select_running_option, avaliable_minions, avaliable_turrets, multiplayer);
				
                display_health(renderer, health, font);
                display_mana(renderer, mana, font);
                display_gold(renderer, gold, font);
                
                break;
                
            case MAIN:
				//select_option
                draw_screen_main(renderer, main_menu_assets, main_menu_rects, main_menu_assets_count, select_option);
                break;
                
            case GAME_MULTIPLAY_SERVER:
                draw_screen_multiplayer_menu(renderer, multiplayer_menu_assets, multiplayer_menu_rects, multiplayer_menu_assets_count, select_multiplayer_option);
                break;
                
            case SCORE:
				//get control of select. < TODO.
				
				
                draw_screen_score(renderer, score_menu_assets, score_menu_rects, score_menu_assets_count, select_return_to_previous_screen);
                break;
                
            case END_GAME:
                draw_screen_end_game(renderer, end_game_interface_assets, end_game_interface_rects, end_game_interface_assets_count, select_end_game_option);
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
		
		//FPS Handling
        t2 = SDL_GetTicks() - t1;
        if(t2 < delay){
            SDL_Delay(delay - t2);
        }
    }
    
	//Destroy thread naturally
	if(thread_control){
		if(thread_control->server.alive){
			thread_control->server.terminate = 1;
			if(thread_control->udp.alive){
				thread_control->udp.terminate = 1;
			}
		}
		if(thread_control->client.alive){
			thread_control->client.terminate = 1;
		}
	}
						
    //Quit
    main_quit();
    return 0;
}

//Init SDL, configs e menu principal
bool main_init(){
    //Read Settings
    FILE *settings = fopen(CONFIG_FILE, "r");
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
	//Language folder.
    fscanf(settings, "language = %s\n", language);
	
	if(!config){
		config = malloc(sizeof(Configuration));
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
	
	config->language_default = 1;
	config->language = 0;
	
	//Load languages.
	lang = init_languages(language);
    
    fclose(settings);
    
    //Init SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL_Init error: %s\n", SDL_GetError());
        return false;
    }
	
	//Init network
	if(SDLNet_Init() < 0){
		printf("SDLNet_Init error: %s\n", SDLNet_GetError());
        return false;
	}
    
    //Create window
    main_Window = SDL_CreateWindow("Grade Defender", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_Width, screen_Height, SDL_WINDOW_SHOWN);
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
    
	if(windows)
		//font = TTF_OpenFont("fonts/8bitOperatorPlus-Regular.ttf", 30);
		font = TTF_OpenFont("fonts/print_bold_tt.ttf", 140);
    else
		font = TTF_OpenFont("../fonts/print_bold_tt.ttf", 140);
		//font = TTF_OpenFont("../fonts/8bitOperatorPlus-Regular.ttf", 30);
	
	
    if(!font){
        printf("Font not loaded! %s\n", TTF_GetError());
        return false;
    }
	
	if(windows)
		title = TTF_OpenFont("fonts/Little Days Alt.ttf", 30);
    else
		title = TTF_OpenFont("../fonts/Little Days Alt.ttf", 30);
	
	
    if(!title){
        printf("Title font not loaded! %s\n", TTF_GetError());
        return false;
    }
	
    int i = 0;
	
    //Init main menu assets
    for(i = 0; i < main_menu_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        switch (i) {
            case 0:
                text = _("Grade Defender");
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1: case 2:
                text = _("Play");
                rect = (SDL_Rect){980, 450, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3: case 4:
                text = _("Multiplayer");
                rect = (SDL_Rect){980, 450 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 5: case 6:
                text = _("Config");
                rect = (SDL_Rect){980, 450 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 7: case 8:
                text = _("Score");
                rect = (SDL_Rect){980, 450 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 9: case 10:
                text = _("Credits");
                rect = (SDL_Rect){30, 450 + BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 11: case 12:
                text = _("Exit");
                rect = (SDL_Rect){980, 450 + BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        main_menu_rects[i] = rect;
        
        SDL_Surface *surface;
        
        if(i%2 == 0 && i > 0)
            surface = TTF_RenderText_Blended(font, text, red);
        else
            surface = TTF_RenderText_Blended(font, text, white);
        
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
	
	//Init config screen assets
    get_config_text();
	
	//Init game paused screen assets
    for(int i = 0; i < pause_interface_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        switch(i){
            case 0:
                rect = (SDL_Rect){0, 0, 1280, 720};
                break;
                
            case 1: case 2:
                text = _("Resume");
                rect = (SDL_Rect){515, 270, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3: case 4:
                text = _("Config");
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 5: case 6:
                text = _("Score");
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 7: case 8:
                text = _("Exit");
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 9: case 10:
                text = _("Main Menu");
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 11: case 12:
                text = _("Credits");
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 5, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        pause_interface_rects[i] = rect;
        
        SDL_Surface *surface;
        if(i == 0){
			if(windows)
				surface = IMG_Load("images/Pause Menu.png");
			else
				surface = IMG_Load("../images/Pause Menu.png");
			
            if(!surface){
                printf("(Game pause)Erro ao carregar pause menu! %s\n", IMG_GetError());
                return false;
            }
        }
        
        else{
            if(i%2 == 0 && i > 0)
                surface = TTF_RenderText_Blended(font, text, red);
            else
                surface = TTF_RenderText_Blended(font, text, white);
            
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
	
	//Init game credits screen assets
    for(int i = 0; i < credits_menu_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        switch (i) {
            case 0:
                text = _("Credits");
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1:
                text = _("Made by:");
                rect = (SDL_Rect){195, 360 - BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 2:
                text = "    Danilo Makoto Ikuta";
                rect = (SDL_Rect){195, 360 - BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3:
                text = "    Gabriel Fontenelle";
                rect = (SDL_Rect){195, 360, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 4:
                text = "    Gabriel Nopper";
                rect = (SDL_Rect){195, 360 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 5: case 6:
                text = _("Back");
                rect = (SDL_Rect){595, 650, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        credits_menu_rects[i] = rect;
        
        SDL_Surface *surface;
        if(i == 6)
            surface = TTF_RenderText_Blended(font, text, red);
        else
            surface = TTF_RenderText_Blended(font, text, white);
        
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
	
	//Init game score screen assets
    for(int i = 0; i < score_menu_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        switch (i) {
            case 0:
                text = _("Score");
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1: case 2:
                text = _("Back");
                rect = (SDL_Rect){595, 650, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        score_menu_rects[i] = rect;
        
        SDL_Surface *surface;
        if(i == 2)
            surface = TTF_RenderText_Blended(font, text, red);
        else
            surface = TTF_RenderText_Blended(font, text, white);
        
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
    
    //Init end game screen assets, except status
    for(int i = 0; i < end_game_interface_assets_count - 1; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        switch (i) {
            case 0:
                rect = (SDL_Rect){0, 0, 1280, 720};
                break;
                
            case 1: case 2:
                text = _("Retry");
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3: case 4:
                text = _("Main Menu");
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
            
            case 5: case 6:
                text = _("Quit");
                rect = (SDL_Rect){515, 270 + BUTTON_MENU_HEIGHT * 5, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
				
            default:
                break;
        }
        
        end_game_interface_rects[i] = rect;
        
        SDL_Surface *surface;
        if(i == 0){
			if(windows) {
				surface = IMG_Load("../images/End Game.png");
			}
			else {
				surface = IMG_Load("../images/End Game.png");
			}
				
            if(!surface){
                printf("(End game)Erro ao carregar end game menu! %s\n", IMG_GetError());
                return false;
            }
        }
        
        else{
            if(i%2 == 0 && i > 0)
                surface = TTF_RenderText_Blended(font, text, red);
            else
                surface = TTF_RenderText_Blended(font, text, white);
            
            if(!surface){
                printf("(End game)Text not rendered! %s\n", TTF_GetError());
                return false;
            }
        }
        
        end_game_interface_assets[i] = SDL_CreateTextureFromSurface(renderer, surface);
        if(!end_game_interface_assets[i]){
            printf("(End game)Erro ao criar textura! %s\n", SDL_GetError());
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
    
    //Init interface static assets(Pause button, right bar, quit multiplayer)
	SDL_Surface *pause_surface;
	if(windows) {
		pause_surface = IMG_Load("images/Pause.png");
	}
	else {
		pause_surface = IMG_Load("../images/Pause.png");
	}
		
		
	
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
    
	SDL_Surface *right_bar_surface;
	
	if(windows){
		right_bar_surface = IMG_Load("images/Right Bar.png");
	}
	else {
		right_bar_surface = IMG_Load("../images/Right Bar.png");
	}
		
		
	
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
	
	SDL_Surface *quit_surface;
    if(windows){
		quit_surface = IMG_Load("images/MP_Quit.png");
	}
	else {
		quit_surface = IMG_Load("../images/MP_Quit.png");
	}
		
	
    if(!quit_surface){
        printf("Falha ao carregar botão quit! %s\n", IMG_GetError());
        return false;
    }
    
    game_interface_assets[14] = SDL_CreateTextureFromSurface(renderer, quit_surface);
    if(!game_interface_assets[14]){
        printf("Falha ao criar textura de botão quit! %s\n", SDL_GetError());
        return false;
    }
    
    SDL_FreeSurface(quit_surface);
    
    game_interface_rects[14] = (SDL_Rect){BUTTON_MENU_HEIGHT, 0, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT};
    
	//Init minions avaliable
	avaliable_minions = load_minions(MINION_FILE);

	if(avaliable_minions == NULL) {
		printf("Falha ao carregar Minions");
		return false;
	}
	//Init turrets avaliable
	avaliable_turrets = load_turrets(TURRET_FILE);
	
	if(avaliable_turrets == NULL) {
		printf("Falha ao carregar Turrets");
		return false;
	}
	
	avaliable_projectiles = load_projectiles(PROJECTILE_FILE);
	
	if (avaliable_projectiles == NULL) {
		printf("Falha ao carregar projectiles");
		//return false;
	}

	//Init thread data
	data_shared = malloc(sizeof(ShareData));
	data_shared->current_comm = NULL;
	data_shared->current_user = NULL;

	//Init Current Player info
	data_shared->current_user = calloc(1, sizeof(User));
	
	#ifdef _WIN32
	data_shared->current_user->name = getenv("USERNAME");
	#else
	data_shared->current_user->name = getlogin();
	#endif

	if(!data_shared->current_user->name) {
		data_shared->current_user->name = malloc(sizeof(char) * 7);
		strncpy(data_shared->current_user->name, "Unknown", 7);
	}
	
	data_shared->current_user->life = DEFAULT_PLAYERS_LIFE;
	
	//Init shared data
	thread_control = calloc(1, sizeof(Threads));
	thread_control->udp.priority = SDL_THREAD_PRIORITY_HIGH;//Maybe normal
	thread_control->client.priority = SDL_THREAD_PRIORITY_LOW;
	thread_control->server.priority = SDL_THREAD_PRIORITY_LOW;
	thread_control->client.pointer = NULL;
	thread_control->server.pointer = NULL;
	thread_control->udp.pointer = NULL;
	
	SDL_Surface *select_Surface;
	//Selector
	if(windows){
		select_Surface = IMG_Load("images/select.png");
	}
	else {
		select_Surface = IMG_Load("../images/select.png");
	}
	
	select_Texture = SDL_CreateTextureFromSurface(renderer, select_Surface);
	
	SDL_FreeSurface(select_Surface);
	
	SDL_Surface *background_Surface;
	//Selector
	if(windows){
		background_Surface = IMG_Load("images/Main Fundo.png");
	}
	else {
		background_Surface = IMG_Load("../images/Main Fundo.png");
	}
	
	background_Texture = SDL_CreateTextureFromSurface(renderer, background_Surface);
	
	SDL_FreeSurface(background_Surface);
	
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
    
    for(int i = 0; i < end_game_interface_assets_count; i++){
        if(end_game_interface_assets[i])
            SDL_DestroyTexture(end_game_interface_assets[i]);
    }
    
    for(int i = 0; i < multiplayer_menu_assets_count; i++){
        if(multiplayer_menu_assets[i])
            SDL_DestroyTexture(multiplayer_menu_assets[i]);
    }
    
    //////free surfaces
    if(map_Surface)
        SDL_FreeSurface(map_Surface);
    
	if(select_Texture)
		SDL_DestroyTexture(select_Texture);
		
    //////free window
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(main_Window)
        SDL_DestroyWindow(main_Window);
    
    //Quit SDL, SDLNet, TTF, IMG
    IMG_Quit();
    TTF_Quit();
	SDLNet_Quit();
    SDL_Quit();
	
	//free config
	//free(config->language);

    if(config)
        free(config);
    
	if(avaliable_minions) 
		free_avaliable_list_minion(avaliable_minions);
		
	if(avaliable_turrets) 
		free_avaliable_list_turret(avaliable_turrets);
	
	if(data_shared){
		if(data_shared->current_comm){
			free(data_shared->current_comm);
			data_shared->current_comm = NULL;
		}
		
		if(data_shared->current_user){
			free(data_shared->current_user);
			data_shared->current_user = NULL;		
		}
		free(data_shared);
		data_shared = NULL;
	}
	
	if(thread_control){
		if(thread_control->client.pointer){
			free(thread_control->client.pointer);
			thread_control->client.pointer = NULL;
		}
		if(thread_control->server.pointer){
			free(thread_control->server.pointer);
			thread_control->server.pointer = NULL;
		}
		if(thread_control->udp.pointer){
			free(thread_control->udp.pointer);
			thread_control->udp.pointer = NULL;
		}
		
		free(thread_control);
		thread_control = NULL;
	}
	
	
    reset_game_data();
}

//Carrega textos do menu de configurações
void get_config_text(){
    for(int i = 0; i < config_menu_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        //Set texts and rects
        switch(i){
            case 0:
                text = _("Config");
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1: case 2:
                if(config->audio_sfx)
                    text = _("Sound Effects:  ON");
                else
                    text = _("Sound Effects:  OFF");
                
                rect = (SDL_Rect){515, 150, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3: case 4:
                if(config->audio_music)
                    text = _("Music:  ON");
                else
                    text = _("Music:  OFF");
                
                rect = (SDL_Rect){515, 150 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 5: case 6:
                text = _("Language");
                
                rect = (SDL_Rect){515, 150 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 7: case 8:
                text = _("Back");
                
                rect = (SDL_Rect){515, 150 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                break;
        }
        
        config_menu_rects[i] = rect;
        
        SDL_Surface *surface;
        if(i%2 == 0 && i > 0)
            surface = TTF_RenderText_Blended(font, text, red);
        else
            surface = TTF_RenderText_Blended(font, text, white);
        
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

//Carrega textos do menu de multiplayer
void get_multiplayer_texts(multiplayer_status current_status, int page){
	//printf("Current status %d\n", current_status);
    for(int i = 0; i < multiplayer_menu_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        //Setting texts and rects
        switch (i) {
            case 0:
                text = _("MULTIPLAYER");
                rect = (SDL_Rect){265, 0, 750, 150};
                break;
                
            case 1: case 2:
                if(current_status == MPS_NONE)
                    text = _("Create Room");
                else if(current_status == MPS_CAN_START)
                    text = _("Start Game");
                else if(current_status == MPS_ENTERED_ROOM)     //Or not ready
                    text = _("Get ready");
                
                rect = (SDL_Rect){515, 150 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 3: case 4:
                if(current_status == MPS_NONE)
                    text = _("Search Room");
                else if(current_status != MPS_NONE && current_status != MPS_STARTED_GAME && current_status != MPS_SEARCHING_ROOM)
                    text = _("Leave Room");
                
                rect = (SDL_Rect){515, 150 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 5: case 6:
                text = _("Back to Main");
                
                rect = (SDL_Rect){515, 150 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 7: case 8:
                if(current_status == MPS_SEARCHING_ROOM){
                    text = _("Previous page");
                    
                    rect = (SDL_Rect){195, 300 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                }
                break;
                
            //Room hosts
            case 9: case 10:
                if(current_status == MPS_SEARCHING_ROOM){
                    text = get_host_name(0 + page * 4);
                    
                    if(*text == '\0')
                        text = "----------";
                    
                    rect = (SDL_Rect){195, 300 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                }
                break;
                
            case 11: case 12:
                if(current_status == MPS_SEARCHING_ROOM){
                    text = get_host_name(1 + page * 4);
                    
                    if(*text == '\0')
                        text = "----------";
                    
                    rect = (SDL_Rect){195, 300 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                }
                break;
                
            case 13: case 14:
                if(current_status == MPS_SEARCHING_ROOM){
                    text = get_host_name(2 + page * 4);
                    
                    if(*text == '\0')
                        text = "----------";
                    
                    rect = (SDL_Rect){195, 300 + BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                }
                break;
                
            case 15: case 16:
                if(current_status == MPS_SEARCHING_ROOM){
                    text = get_host_name(3 + page * 4);
                    
                    if(*text == '\0')
                        text = "----------";
                    
                    rect = (SDL_Rect){195, 300 + BUTTON_MENU_HEIGHT * 5, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                }
                break;
                
            case 17: case 18:
                if(current_status == MPS_SEARCHING_ROOM){
                    text = _("Next page");
                    
                    rect = (SDL_Rect){195, 300 + BUTTON_MENU_HEIGHT * 6, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                }
                break;
            
            case 19:
                text = _("Rooms");
                
                rect = (SDL_Rect){195, 300, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 20:
                text = _("Players");
                
                rect = (SDL_Rect){515, 300, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            case 21:
                text = _("Ready?");
                
                rect = (SDL_Rect){835, 300, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                break;
                
            //Players in room
            case 22:
                SDL_AtomicLock(&thread_control->lock.comm);
                if(current_status != MPS_SEARCHING_ROOM && current_status != MPS_NONE && data_shared->current_comm->match->players > 0){
                    if(data_shared->current_comm->adversary[0].name)
                        text = data_shared->current_comm->adversary[0].name;
                    //text = "ALLAHU AKBAR";
                    
                    rect = (SDL_Rect){515, 300 + BUTTON_MENU_HEIGHT, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                }
                SDL_AtomicUnlock(&thread_control->lock.comm);
                break;
                
            case 23:
                SDL_AtomicLock(&thread_control->lock.comm);
                if(current_status != MPS_SEARCHING_ROOM && current_status != MPS_NONE && data_shared->current_comm->match->players > 1){
                    if(data_shared->current_comm->adversary[1].name)
                        text = data_shared->current_comm->adversary[1].name;
                    
                    rect = (SDL_Rect){515, 300 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                }
                SDL_AtomicUnlock(&thread_control->lock.comm);
                break;
                
            case 24:
                SDL_AtomicLock(&thread_control->lock.comm);
                if(current_status != MPS_SEARCHING_ROOM && current_status != MPS_NONE && data_shared->current_comm->match->players > 2){
                    if(data_shared->current_comm->adversary[2].name)
                        text = data_shared->current_comm->adversary[2].name;
                    
                    rect = (SDL_Rect){515, 300 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                }
                SDL_AtomicUnlock(&thread_control->lock.comm);
                break;
                
            case 25:
                SDL_AtomicLock(&thread_control->lock.comm);
                if(current_status != MPS_SEARCHING_ROOM && current_status != MPS_NONE && data_shared->current_comm->match->players > 3){
                    if(data_shared->current_comm->adversary[3].name)
                        text = data_shared->current_comm->adversary[3].name;
                    
                    rect = (SDL_Rect){515, 300 + BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT};
                }
                SDL_AtomicUnlock(&thread_control->lock.comm);
                break;
                
            //Ready?
            case 26:
                SDL_AtomicLock(&thread_control->lock.comm);
                if(current_status != MPS_SEARCHING_ROOM && current_status != MPS_NONE && data_shared->current_comm->match->players > 0){
                    if(data_shared->current_comm->adversary[0].ready_to_play)
                        text = _("Yes");
                    else
                        text = _("No");
                    
                    rect = (SDL_Rect){945, 300 + BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT};
                }
                SDL_AtomicUnlock(&thread_control->lock.comm);
                break;
                
            case 27:
                SDL_AtomicLock(&thread_control->lock.comm);
                if(current_status != MPS_SEARCHING_ROOM && current_status != MPS_NONE && data_shared->current_comm->match->players > 1){
                    if(data_shared->current_comm->adversary[1].ready_to_play)
                        text = _("Yes");
                    else
                        text = _("No");
                    
                    rect = (SDL_Rect){945, 300 + BUTTON_MENU_HEIGHT * 2, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT};
                }
                SDL_AtomicUnlock(&thread_control->lock.comm);
                break;
                
            case 28:
                SDL_AtomicLock(&thread_control->lock.comm);
                if(current_status != MPS_SEARCHING_ROOM && current_status != MPS_NONE && data_shared->current_comm->match->players > 2){
                    if(data_shared->current_comm->adversary[2].ready_to_play)
                        text = _("Yes");
                    else
                        text = _("No");
                    
                    rect = (SDL_Rect){945, 300 + BUTTON_MENU_HEIGHT * 3, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT};
                }
                SDL_AtomicUnlock(&thread_control->lock.comm);
                break;
                
            case 29:
                SDL_AtomicLock(&thread_control->lock.comm);
                if(current_status != MPS_SEARCHING_ROOM && current_status != MPS_NONE && data_shared->current_comm->match->players > 3){
                    if(data_shared->current_comm->adversary[3].ready_to_play)
                        text = _("Yes");
                    else
                        text = _("No");
                    
                    rect = (SDL_Rect){945, 300 + BUTTON_MENU_HEIGHT * 4, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT};
                }
                SDL_AtomicUnlock(&thread_control->lock.comm);
                break;
            
            //status
            case 30:
                if(current_status == MPS_NONE)
                    text = NULL;
                else if(current_status == MPS_WAIT_FOR_PLAYER)
                    text = _("Waiting for players...");
                else if(current_status == MPS_SEARCHING_ROOM)
                    text = _("Searching rooms...");
                else if(current_status == MPS_WAIT_READY)
                    text = _("Everyone must be ready to start");
                else if(current_status == MPS_CAN_START)
                    text = _("Everyone ready!");
                
                rect = (SDL_Rect){390, 150, BUTTON_MENU_WIDTH * 2, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                text = " ";
                break;
        }
        
        //Creating textures
        if(text){
            SDL_Surface *surface;
            if(i%2 == 0 && i > 0 && i <= 18)
                surface = TTF_RenderText_Blended(font, text, red);
            else
                surface = TTF_RenderText_Blended(font, text, white);
            
            if(!surface){
                printf("(Multiplayer)Text not rendered! %s\n", TTF_GetError());
                return;
            }
            
            multiplayer_menu_rects[i] = rect;
            
            multiplayer_menu_assets[i] = SDL_CreateTextureFromSurface(renderer, surface);
            
            if(!multiplayer_menu_assets[i]){
                printf("(Multiplayer)Text texture not rendered! %s\n", SDL_GetError());
                return;
            }
            
            SDL_FreeSurface(surface);
        }
        
        else
            multiplayer_menu_assets[i] = NULL;
    }
}

//Reseta listas, e dados do jogo
void reset_game_data(){
	int i;
    //////free lists
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
    
    health = DEFAULT_PLAYERS_LIFE;
    gold = 1000;
    mana = 0;
	
	minions_left = 184;
	show_timer = 0;
	timer_count = 0;
	spawn_minion = 0;
	 
	//Reset current user data used on network.
	if(thread_control){
		SDL_AtomicLock(&thread_control->lock.user);
		if(data_shared){
			if(data_shared->current_user){
				data_shared->current_user->id = 0;
				data_shared->current_user->is_server = 0;
				data_shared->current_user->life = DEFAULT_PLAYERS_LIFE;
				data_shared->current_user->ready_to_play = 0;
				data_shared->current_user->process.message_status = 0;
				data_shared->current_user->process.message_life = 0;
				data_shared->current_user->process.message_minion = 0;
				
				if(data_shared->current_user->minions){
					for(i = 0; i < data_shared->current_user->spawn_amount; i++){
						if(data_shared->current_user->minions[i].type) {
							free(data_shared->current_user->minions[i].type);
							data_shared->current_user->minions[i].type = NULL;
						}
					}
					free(data_shared->current_user->minions);
					data_shared->current_user->minions = NULL;
				}
				data_shared->current_user->spawn_amount = 0;
			}
		}
		SDL_AtomicUnlock(&thread_control->lock.user);
	}
	return;
}

void set_end_game_status_text(end_game_status end_status){
    char *text = NULL;
    
    switch (end_status) {
        case EGS_WIN:
            text = _("YOU WIN!");
            break;
            
        case EGS_LOSE:
            text = _("YOU LOST THE GAME");
            break;
            
        case EGS_DC:
            text = _("ERROR: DISCONNECTED");
            break;
            
        case EGS_OPLEFT:
            text = _("YOUR OPPONENT LEFT!");
            break;
            
        case EGS_NONE:
            text = "wAT?!";
            break;
            
        default:
            break;
    }
    
    end_game_interface_rects[7] = (SDL_Rect){265, 270, BUTTON_MENU_WIDTH * 3, BUTTON_MENU_HEIGHT * 3};
    
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, white);
    
    if(!surface){
        printf("(End game)Text not rendered! %s\n", TTF_GetError());
        return;
    }
    
    end_game_interface_assets[7] = SDL_CreateTextureFromSurface(renderer, surface);
    
    if(!end_game_interface_assets[7]){
        printf("(End game)Text texture not rendered! %s\n", SDL_GetError());
        return;
    }
    
    SDL_FreeSurface(surface);
    
}

void get_multiplayer_game_names(int page){
    for(int i = 2; i < multiplayer_menu_assets_count; i++){
        char *text = NULL;
        SDL_Rect rect;
        
        switch(i){
            case 2: case 3:
                text = _("Previous page");
                
                rect = (SDL_Rect){1097, 350, 180, BUTTON_MENU_HEIGHT};
                break;
                
            case 4: case 5:
                text = data_shared->current_comm->adversary[page * 4].name;
                
                if(*text == '\0')
                    text = "----------";
                
                rect = (SDL_Rect){1097, 350 + BUTTON_MENU_HEIGHT, 180, BUTTON_MENU_HEIGHT};
                break;
                
            case 6: case 7:
                text = data_shared->current_comm->adversary[page * 4 + 1].name;
                
                if(*text == '\0')
                    text = "----------";
                
                rect = (SDL_Rect){1097, 350 + BUTTON_MENU_HEIGHT * 2, 180, BUTTON_MENU_HEIGHT};
                break;
                
            case 8: case 9:
                text = data_shared->current_comm->adversary[page * 4 + 2].name;
                
                if(*text == '\0')
                    text = "----------";
                
                rect = (SDL_Rect){1097, 350 + BUTTON_MENU_HEIGHT * 3, 180, BUTTON_MENU_HEIGHT};
                break;
                
            case 10: case 11:
                text = data_shared->current_comm->adversary[page * 4 + 3].name;
                
                if(*text == '\0')
                    text = "----------";
                
                rect = (SDL_Rect){1097, 350 + BUTTON_MENU_HEIGHT * 4, 180, BUTTON_MENU_HEIGHT};
                break;
                
            case 12: case 13:
                text = _("Next page");
                
                rect = (SDL_Rect){1097, 350 + BUTTON_MENU_HEIGHT * 5, 180, BUTTON_MENU_HEIGHT};
                break;
                
            default:
                text = NULL;
                break;
        }
        
        //Creating textures
        if(text){
            SDL_Surface *surface;
            if(i%2 == 1)
                surface = TTF_RenderText_Blended(font, text, red);
            else
                surface = TTF_RenderText_Blended(font, text, white);
            
            if(!surface){
                printf("(Game Running)Text not rendered! %s\n", TTF_GetError());
                return;
            }
            
            game_interface_rects[i] = rect;
            
            game_interface_assets[i] = SDL_CreateTextureFromSurface(renderer, surface);
            
            if(!game_interface_assets[i]){
                printf("(Game Running)Text texture not rendered! %s\n", SDL_GetError());
                return;
            }
            
            SDL_FreeSurface(surface);
        }
        
        else
            game_interface_assets[i] = NULL;
    }
}