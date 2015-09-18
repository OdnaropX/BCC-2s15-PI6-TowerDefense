//
//  GameScene.c
//  
//
//  Created by Gabriel Nopper on 17/09/15.
//
//

#include "GameScene.h"
#include "Estruturas.h"

#include <stdbool.h>

int grid[17][13];






int game_loop(SDL_Window *window, SDL_Surface *screenSurface){
    
    SDL_Event events;
    

    int game_is_active = 1;
    int lifes = 1;
    int gold = 0;
    
    
    
    
    while(game_is_active){
        
        // *************** //
        // USER INPUT AREA //
        // *************** //
        
        while(SDL_PollEvent(&events)){
            switch (events.type) {
                case SDL_KEYDOWN:{ // Keyboard detection
                    
                    break;
                }
                case SDL_BUTTON_LEFT:{ // Mouse detection
                    
                    break;
                }
                    
                default:
                    break;
            }
        }
        
        // Minion routine
        
        // Tower Routine
        
        
        
        
        // Updates
        SDL_UpdateWindowSurface(window);
    }
    
    
    
    
    
    return 1;
}




int getTouchedGridValue(int x, int y){
    value = grid[x/block_width][y/block_height];
    return value;
}



/**
 Draws a node considering it's x and y points at the center of the drawn point.
 1- The screen is where to draw.
 2- The drawn_node is the node you want to draw.
 3- Boolean value tower limits draw position, so that towers are placed into slots properly.
 **/
int Draw_Node_Normal(SDL_Surface *screen, node *drawn_node, bool tower){
    SDL_Rect rect;
    rect.w = drawn_node->sprite.w;
    rect.h = drawn_node->sprite.h;
    rect.x = drawn_node->xPos - (rect.w/2);
    rect.y = drawn_node->yPos - (rect.h/2);
    
    if(tower){
        rect.y = drawn_node->yPos - rect.h + (rect.w/2);
    }
    
    SDL_BlitSurface(&drawn_node->sprite, NULL, screen, &rect);
    
    return 0;
}



