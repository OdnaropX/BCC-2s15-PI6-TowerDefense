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

/**
 Moves target bullet. It returns 1 if it hit the target, and 0 if it still not got there.
 **/
int move_bullet(minion *target, projectile *shoot);

void get_cartesian_from_grid_number(int grid_number, int *array_position, int limit_x);

void get_grid_address_matriz(int grid_number, int limit_x, int *matriz);

int is_time(int timer, int second);



int perform_path_verification(int targetX, int targetY);

void occupyGrid(int targetX, int targetY);

void freeGrid(int targetX, int targetY);


#endif /* defined(____GameScene__) */
