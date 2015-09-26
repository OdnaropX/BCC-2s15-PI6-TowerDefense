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

/**
 Fills sent array[2] with x and y positions for screen, as center of the rect.
 **/
void get_center_position_to_grid(int x, int y, int array[]);


#endif /* defined(____GameScene__) */
