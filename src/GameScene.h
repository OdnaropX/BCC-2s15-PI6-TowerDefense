//
//  GameScene.h
//  
//
//  Created by Gabriel Nopper on 17/09/15.
//
//

#ifndef ____GameScene__
#define ____GameScene__

#include <stdio.h>
#include <stdbool.h>
#include "Estruturas.h"

/**
 Fills sent array with array adress for touched grid squares.
 Returns:
 1 if success.
 0 if out of grid.
 **/
int get_touched_grid_address(int x_touch, int y_touch, int array[]);

int get_touched_menu_address(int x_touch, int y_touch, int center_position[], int tower);

/**
 Fills sent array[2] with x and y positions for screen, as center of the rect.
 **/
void get_center_position_to_grid(int x, int y, int array[]);

//Use this on main init(or when loading a map)
SDL_Surface *init_map();

/***
  Get linear address from array address.
  use example: get_grid_address_linear(1, 1, 16) --> 17
**/
int get_grid_address_linear(int grid_x, int grid_y, int limit_x);

/**
 Moves target minion a frame. He will go towards the center of the next grid to be moved into.
 **/
void move_minion(minion *enemy);


#endif /* defined(____GameScene__) */
