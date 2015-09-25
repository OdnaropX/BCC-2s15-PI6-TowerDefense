//
//  Estruturas.h
//
//  Created by Gabriel Nopper on 29/08/15.
//  Copyright (c) 2015 Gabriel Nopper. All rights reserved.
//

#ifndef __Tower_Defense__Renderer__
#define __Tower_Defense__Renderer__

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>

#define block_width 64
#define block_height 43
#define map_array_width 17
#define map_array_height 13
#define window_width 1280
#define window_height 720
#define BUTTON_MENU_HEIGHT 30
#define BUTTON_MENU_WIDTH 250

//Renderer options, like text display, middle width calculation, etc...

//Function used to get the center of a object on a position, given a space and object.
int get_center(int space_size, int object_size);

void drawn_phrase();

void drawn_text();

void drawn_screen_main();

void drawn_screen_config();

void drawn_screen_game_running();

void drawn_screen_game_paused();

//Screen used while initiating game and loading required data.
void drawn_screen_sprite();


#endif /* defined(__Tower_Defense__Renderer__) */