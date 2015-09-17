//
//  GameScene.c
//  
//
//  Created by Gabriel Nopper on 17/09/15.
//
//

#include "GameScene.h"
#include "Estruturas"







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
        
        
        
        
        
        // Updates
        SDL_UpdateWindowSurface(window);
    }
    
    
    
    
    
    return 1;
}









int Draw_Node(SDL_Surface *screen, SDL_Window *window, node *drawn_node){
    
    
    
}

