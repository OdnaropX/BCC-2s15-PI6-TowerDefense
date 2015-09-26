//
//  Renderer.c
//  PI6 - TowerDefense
//
//  Created by Danilo Makoto Ikuta on 25/09/15.
//  Copyright © 2015 Danilo Makoto Ikuta. All rights reserved.
//

#include "Renderer.h"

/**
 Draws a node considering it's x and y points at the center of the drawn point.
 1- The screen is where to draw.
 2- The drawn_node is the node you want to draw.
 3- Boolean value tower limits draw position, so that towers are placed into slots properly.
 **/
void draw_Node(SDL_Surface *screen, node *drawn_node, bool tower){
    SDL_Rect rect;
    rect.w = drawn_node->sprite->w;
    rect.h = drawn_node->sprite->h;
    rect.x = drawn_node->xPos - (rect.w/2);
    rect.y = drawn_node->yPos - (rect.h/2);
    
    if(tower){
        rect.y = drawn_node->yPos - rect.h + (rect.w/2);
    }
    
    SDL_BlitSurface(drawn_node->sprite, NULL, screen, &rect);
}

void draw_screen_main(SDL_Surface *screen){}

void draw_screen_config(SDL_Surface *screen){}

void draw_screen_game_running(SDL_Surface *screen, SDL_Surface *map, list_minion *minions, list_projectile *projectiles, list_turret *turrets){
    //Draw map
    SDL_Rect map_Rect = {map->w, map->h, 0, 0};
    SDL_BlitSurface(map, NULL, screen, &map_Rect);
    
    //Draw turrets
    list_turret *t = turrets;
    
    while (t) {
        turret *target = t->e;
        draw_Node(screen, &target->node, true);
        
        t = t->next;
    }
    
    //Draw minions
    list_minion *m = minions;
    
    while (m) {
        minion *target = m->e;
        draw_Node(screen, &target->node, false);
        
        m = m->next;
    }
    
    //Draw projectiles
    list_projectile *p = projectiles;
    
    while (p) {
        projectile *target = p->e;
        draw_Node(screen, &target->node, false);
        
        p = p->next;
    }
}

void draw_screen_game_paused(SDL_Surface *screen){}