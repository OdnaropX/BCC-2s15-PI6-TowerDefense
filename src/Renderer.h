//
//  Estruturas.h
//
//  Created by Gabriel Nopper on 29/08/15.
//  Copyright (c) 2015 Gabriel Nopper. All rights reserved.
//

#ifndef __Tower_Defense__Renderer__
	#define __Tower_Defense__Renderer__

	//Includes
	///////////////////////////////////////////////////////////////////////
	
	#include "Estruturas/Control.h"
	#include "Estruturas/Minion.h"
	#include "Estruturas/Turret.h"
	#include "Estruturas/Misc.h"
	#include "Estruturas/Projectile.h"
	
	//Constants
	///////////////////////////////////////////////////////////////////////

	#define window_width 1280
	#define window_height 720

	#define BUTTON_MENU_HEIGHT 30
	#define MENU_ICON 50
	#define BUTTON_MENU_WIDTH 250

	//Draw Functions
	///////////////////////////////////////////////////////////////////////
	
	//Renderer options, like text display, middle width calculation, etc...
	void draw_phrase();

	void draw_text(SDL_Renderer *renderer, SDL_Texture *text);

	void draw_screen_main(SDL_Renderer *renderer, SDL_Texture **texts, SDL_Rect *rectangles, int count, main_options select_option);

	void draw_screen_config(SDL_Renderer *renderer, SDL_Texture **texts, SDL_Rect *rectangles, int count, config_options select_config_option);

	void draw_screen_game_running(SDL_Surface *screen, SDL_Surface *map, list_minion *minions, list_turret *turrets);

	void draw_screen_game_interface(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count);

	void draw_screen_game_paused(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, pause_options select_pause_option);

	void draw_screen_credits(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, bool back);

	void draw_screen_score(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, bool back);

	void draw_screen_game_over(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, game_over_options select_game_over_option);

	void display_mouse(SDL_Renderer *renderer, bool active_clicked, bool selected_left, int select_grid, int grid_over, int center_clicked[], GAME_RUNNING_OPTIONS running_option, list_minion_avaliable *list_m, list_turret_avaliable *list_t);

	void draw_screen_multiplayer_menu(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, multiplayer_menu_options select_multiplayer_option);

	void display_health(SDL_Renderer *renderer, int value, TTF_Font *font);

	void display_mana(SDL_Renderer *renderer, int value, TTF_Font *font);

	void display_gold(SDL_Renderer *renderer, int value, TTF_Font *font);

	/**
	 Draws a node considering it's x and y points at the center of the drawn point.
	 1- The screen is where to draw.
	 2- The drawn_node is the node you want to draw.
	 3- Boolean value tower limits draw position, so that towers are placed into slots properly.
	 **/
	void draw_node(SDL_Surface *screen, node *drawn_node, bool tower);
	void draw_turret_avaliable(SDL_Renderer *renderer, list_turret_avaliable *list, int per_row, int position[]);
	void draw_minion_avaliable(SDL_Renderer *renderer, list_minion_avaliable *list, int per_row, int position[]);
	
	//Get Functions
	///////////////////////////////////////////////////////////////////////
	
	//Function used to get the center of a object on a position, given a space and object.
	int get_center(int space_size, int object_size);

	/* Function to get width and height of turret menu.
		Need a 2 size array, size[0] and size[1].
	*/
	void get_menu_size_tower(int size[], list_turret_avaliable *list, int *per_row);

	void get_menu_size_minion(int size[], list_minion_avaliable *list, int *per_row);

#endif /* defined(__Tower_Defense__Renderer__) */