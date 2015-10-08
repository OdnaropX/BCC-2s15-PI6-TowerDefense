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
void draw_node(SDL_Surface *screen, node *drawn_node, bool tower){
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

void draw_screen_main(SDL_Renderer *renderer, SDL_Texture **texts, SDL_Rect *rectangles, int count, main_options select_option){
    for(int i = 0; i < count; i++){
        SDL_RenderCopy(renderer, texts[i], NULL, &rectangles[i]);
    }
}

void draw_screen_config(SDL_Renderer *renderer, SDL_Texture **texts, SDL_Rect *rectangles, int count, config_options select_config_option){
    for(int i = 0; i < count; i++){
        SDL_RenderCopy(renderer, texts[i], NULL, &rectangles[i]);
    }
}

void draw_screen_game_running(SDL_Surface *screen, SDL_Surface *map, list_minion *minions, list_projectile *projectiles, list_turret *turrets){
    //Draw map
    SDL_Rect map_Rect = {0, 0, map->w, map->h};
    SDL_BlitSurface(map, NULL, screen, &map_Rect);
    
    //Draw turrets
    list_turret *t = turrets;
    
    while (t && t->e) {
        turret *target = t->e;
        draw_node(screen, &target->node, true);
        
        t = t->next;
    }
    
    //Draw minions
    list_minion *m = minions;
    
    while (m && m->e) {
        minion *target = m->e;
        draw_node(screen, &target->node, false);
        
        m = m->next;
    }
    
    //Draw projectiles
    list_projectile *p = projectiles;
    
    while (p && p->e) {
        projectile *target = p->e;
        draw_node(screen, &target->node, false);
        
        p = p->next;
    }
}

void draw_screen_game_interface(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, bool active_clicked, bool selected_left, int select_grid, GAME_RUNNING_OPTIONS running_option){
    for(int i = 0; i < count; i++){
        SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
    }
}

void draw_screen_game_paused(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, pause_options select_pause_option){
    for(int i = 0; i < count; i++){
        SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
    }
}

void draw_screen_credits(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count){
    for(int i = 0; i < count; i++){
        SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
    }
}

void draw_screen_score(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count){
    for(int i = 0; i < count; i++){
        SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
    }
}

void get_menu_size_tower(int size[]){
	int correction, number, row, columns;
	
	number = get_tower_avaliable();
	
	if (number == 3) {
		row = 1;
	}
	else {
		row = (int) sqrt((double) number) + 1;//3
	}
	
	correction = 0;
	if (number % row > 0) {
		correction = 1;
	}
	
	columns = (number / row) + correction; //3
	
	size[0] = columns * MENU_ICON;
	size[1] = row * MENU_ICON;
}

void get_menu_size_minion(int size[]){
	int correction, number, row, columns;
	
	number = get_minion_avaliable();
	
	if (number == 3) {
		row = 1;
	}
	else {
		row = (int) sqrt((double) number) + 1;//3
	}
	
	correction = 0;
	if (number % row > 0) {
		correction = 1;
	}
	
	columns = (number / row) + correction; //3
	
	size[0] = columns * MENU_ICON;
	size[1] = row * MENU_ICON;
}


void display_health(SDL_Renderer *renderer, int value, TTF_Font *font){
    char str[5];
    sprintf(str, "%d", value);
    SDL_Surface *s = TTF_RenderText_Solid(font, str, (SDL_Color){255, 255, 255});
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, s);
    
    SDL_FreeSurface(s);
    
    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){1185, 40, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT});
}

void display_mana(SDL_Renderer *renderer, int value, TTF_Font *font){
    char str[5];
    sprintf(str, "%d", value);
    SDL_Surface *s = TTF_RenderText_Solid(font, str, (SDL_Color){255, 255, 255});
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, s);
    
    SDL_FreeSurface(s);
    
    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){1185, 130, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT});
}

void display_gold(SDL_Renderer *renderer, int value, TTF_Font *font){
    char str[5];
    sprintf(str, "%d", value);
    SDL_Surface *s = TTF_RenderText_Solid(font, str, (SDL_Color){255, 255, 255});
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, s);
    
    SDL_FreeSurface(s);
    
    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){1185, 240, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT});
}
