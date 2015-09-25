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

typedef enum _screens {MAIN, CONFIGConfig = 2, GameRunning = 3, GamePaused = 4} screen;



/**
 Draws a node considering it's x and y points at the center of the drawn point.
 1- The screen is where to draw.
 2- The drawn_node is the node you want to draw.
 3- Boolean value tower limits draw position, so that towers are placed into slots properly.
 **/
int Draw_Node(SDL_Surface *screen, node *drawn_node, bool tower);

/**
 Fills sent array with array adress for touched grid squares.
 Returns:
 1 if success.
 0 if out of grid.
 **/
int getTouchedGridAddress(int x_touch, int y_touch, int array[]);

/**
 Fills sent array[2] with x and y positions for screen, as center of the rect.
 **/
void getCenterPositionToGrid(int x, int y, int array[]);


#endif /* defined(____GameScene__) */
