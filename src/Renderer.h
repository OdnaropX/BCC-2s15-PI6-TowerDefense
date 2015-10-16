//
//  Estruturas.h
//
//  Created by Gabriel Nopper on 29/08/15.
//  Copyright (c) 2015 Gabriel Nopper. All rights reserved.
//

#ifndef __Tower_Defense__Renderer__
#define __Tower_Defense__Renderer__

#include "Estruturas.h"

#define window_width 1280
#define window_height 720

#define BUTTON_MENU_HEIGHT 30
#define MENU_ICON 30
#define BUTTON_MENU_WIDTH 250

//Renderer options, like text display, middle width calculation, etc...

//Function used to get the center of a object on a position, given a space and object.
int get_center(int space_size, int object_size);

/* Function to get width and height of turret menu.
	Need a 2 size array, size[0] and size[1].
*/
void get_menu_size_tower(int size[]);

void get_menu_size_minion(int size[]);

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

void display_mouse(SDL_Renderer *renderer, bool active_clicked, bool selected_left, int select_grid, int grid_over, int center_clicked[], GAME_RUNNING_OPTIONS running_option);

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

#endif /* defined(__Tower_Defense__Renderer__) */