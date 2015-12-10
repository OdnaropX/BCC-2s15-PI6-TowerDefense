//
//  Renderer.c
//  PI6 - TowerDefense
//
//  Created by Danilo Makoto Ikuta on 25/09/15.
//  Copyright © 2015 Danilo Makoto Ikuta. All rights reserved.
//

#include "Renderer.h"

void drawn_dashed_retangle(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color color){
	SDL_Rect line = {x,y,width,height};
	//Vertical
	for(int i = 0 ; i < 10 ; i++){
		SDL_RenderFillRect(renderer,&line);
		line.y += 30; //So actually a line with holes in it
	}
	//Horizontal
}


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
        rect.y = drawn_node->yPos + (rect.w/2);
        rect.x = drawn_node->xPos + 2;

    }
    
    SDL_BlitSurface(drawn_node->sprite, NULL, screen, &rect);
}

void draw_thumbnail(SDL_Renderer *renderer, node *drawn_node, int x_init, int y_init){
	SDL_Texture *texture;
	texture = SDL_CreateTextureFromSurface(renderer, drawn_node->sprite);
	SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){x_init, y_init, MENU_ICON, MENU_ICON});
	SDL_DestroyTexture(texture);
}

void draw_turret_avaliable(SDL_Renderer *renderer, list_turret_avaliable *list, int per_row, int position[]){
	list_turret_avaliable *temp = list;
	int x_init, y_init;
	int line = 0;
	
	x_init =  position[0];
	y_init = position[1];
	
	while(temp != NULL && temp->e != NULL) {
		draw_thumbnail(renderer, temp->e->thumbnail, x_init, y_init);
		line++;
		temp = temp->next;
		if(line == per_row) {
			x_init = position[0];
			y_init += MENU_ICON;
			line = 0;
		}
		else {
			x_init += MENU_ICON;
		}
	}
}

void draw_minion_avaliable(SDL_Renderer *renderer, list_minion_avaliable *list, int per_row, int position[]){
	list_minion_avaliable *temp = list;
	int x_init, y_init;
	int line = 0;
	
	x_init =  position[0];
	y_init = position[1];
	
	while(temp != NULL && temp->e != NULL) {
		draw_thumbnail(renderer, temp->e->thumbnail, x_init, y_init);
		line++;
		temp = temp->next;
		if(line == per_row) {
			x_init = position[0];
			y_init += MENU_ICON;
			line = 0;
		}
		else {
			x_init += MENU_ICON;
		}
	}
}

void draw_screen_main(SDL_Renderer *renderer, SDL_Texture **texts, SDL_Rect *rectangles, int count, main_options select_option){
    for(int i = 0; i < count; i++){
        int sel = 0;
        
        switch (select_option) {
            case OPT_PLAY:
                sel = 2;
                break;
                
            case OPT_MULTIPLAYER:
                sel = 4;
                break;
                
            case OPT_CONFIG:
                sel = 6;
                break;
                
            case OPT_SCORE:
                sel = 8;
                break;
            
            case OPT_CREDIT:
                sel = 10;
                break;
                
            case OPT_EXIT:
                sel = 12;
                break;
                
            default:
                break;
        }
        
        if(i == 0 || i == sel || i%2 == 1)
            SDL_RenderCopy(renderer, texts[i], NULL, &rectangles[i]);
    }
}

void draw_screen_config(SDL_Renderer *renderer, SDL_Texture **texts, SDL_Rect *rectangles, int count, config_options select_config_option){
    for(int i = 0; i < count; i++){
        int sel = 0;
        
        switch (select_config_option) {
            case AUDIO_SFX:
                sel = 2;
                break;
                
            case AUDIO_MUSIC:
                sel = 4;
                break;
                
            case LANGUAGE:
                sel = 6;
                break;
                
            case BACK:
                sel = 8;
                break;
                
            default:
                break;
        }
        
        if(i == 0 || i == sel || i%2 == 1)
            SDL_RenderCopy(renderer, texts[i], NULL, &rectangles[i]);
    }
}

void draw_screen_game_running(SDL_Surface *screen, SDL_Surface *map, list_minion *minions, list_turret *turrets){
	//Draw map
    SDL_Rect map_Rect = {0, 0, map->w, map->h};
    SDL_BlitSurface(map, NULL, screen, &map_Rect);
    
    //Draw turrets
    list_turret *t = turrets;
    
    while (t && t->e) {
        turret *target = t->e;
		if(target->node)
			draw_node(screen, target->node, true);
        
        t = t->next;
    }
    
    //Draw minions
    list_minion *m = minions;

    while (m && m->e) {
        minion *target = m->e;
		if(target->node)
			draw_node(screen, target->node, false);
        list_projectile *p = m->e->targetted_projectils;

        //Draw projectiles
        while (p && p->e){
			if(p->e->node)//Dont known why p->e != NULL and p->e->node == NULL.
				draw_node(screen, p->e->node, false);
            p = p->next;
        }
        m = m->next;
    }
	
}

void draw_screen_game_interface(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, target_select_options select_target_option, bool multiplayer){
    int sel = 0;
    
    for(int i = 0; i < count - 1; i++){
        if(multiplayer){
            switch (select_target_option) {
                case TSO_PREVIOUS_PAGE:
                    sel = 3;
                    break;
                    
                case TSO_TGT_1:
                    sel = 5;
                    break;
                    
                case TSO_TGT_2:
                    sel = 7;
                    break;
                    
                case TSO_TGT_3:
                    sel = 9;
                    break;
                    
                case TSO_TGT_4:
                    sel = 11;
                    break;
                    
                case TSO_NEXT_PAGE:
                    sel = 13;
                    break;
                    
                default:
                    break;
            }
        }
		
        if(i%2 == 0 || i < 2 || i == sel){
			SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
		}
        
        if(assets[i] && i > 3 && i < 12)
            SDL_DestroyTexture(assets[i]);
        }
    
    SDL_RenderCopy(renderer, assets[0], NULL, &rectangles[0]);
}

void draw_screen_game_paused(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, pause_options select_pause_option){
    for(int i = 0; i < count; i++){
        int sel = 0;
        
        switch (select_pause_option) {
            case OPT_P_RESUME:
                sel = 2;
                break;
                
            case OPT_P_CONFIG:
                sel = 4;
                break;
                
            case OPT_P_SCORE:
                sel = 6;
                break;
                
            case OPT_P_EXIT:
                sel = 8;
                break;
                
            case OPT_P_MAIN:
                sel = 10;
                break;
                
            case OPT_P_CREDITS:
                sel = 12;
                break;
                
            default:
                break;
        }
        
        if(i == 0 || i == sel || i%2 == 1)
            SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
    }
}

void draw_screen_credits(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, bool back){
    for(int i = 0; i < count; i++){
        if(i != 6)
            SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
        else if(i == 6 && back)
            SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
    }
}

void draw_screen_score(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, bool back){
    for(int i = 0; i < count; i++){
        if(i != 2)
            SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
        else if(i == 2 && back)
            SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
    }
}

void draw_screen_end_game(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, end_game_options select_end_game_option){
    for(int i = 0; i < count; i++){
        int sel = 0;
        switch(select_end_game_option){
            case EG_NEW_GAME:
                sel = 2;
                break;
            case EG_MAIN:
                sel = 4;
                break;
            case EG_QUIT:
                sel = 6;
                break;
            default:
                break;
        }
        
        if(i == 0 || i == sel || i%2 == 1)
            SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
		
    }
	if(assets[7])
        SDL_DestroyTexture(assets[7]);
}

void draw_screen_multiplayer_menu(SDL_Renderer *renderer, SDL_Texture **assets, SDL_Rect *rectangles, int count, multiplayer_menu_options select_multiplayer_option){
    for(int i = 0; i < count; i++){
        int sel = 0;
        switch (select_multiplayer_option) {
            case MP_CREATE_ROOM: case MP_START: case MP_TOGGLE_READY:
                sel = 2;
                break;
                
            case MP_SEARCH_ROOM: case MP_LEAVE:
                sel = 4;
                break;
                
            case MP_BACK_TO_MAIN:
                sel = 6;
                break;
                
            case MP_PREVIOUS_PAGE:
                sel = 8;
                break;
                
            case MP_ROOM_BTN_1:
                sel = 10;
                break;
                
            case MP_ROOM_BTN_2:
                sel = 12;
                break;
                
            case MP_ROOM_BTN_3:
                sel = 14;
                break;
                
            case MP_ROOM_BTN_4:
                sel = 16;
                break;
                
            case MP_NEXT_PAGE:
                sel = 18;
                break;
                
            default:
                break;
        }
        
        if((i == 0 || i == sel || i % 2 == 1 || i > 18) && assets[i])
            SDL_RenderCopy(renderer, assets[i], NULL, &rectangles[i]);
        
        if(assets[i])
            SDL_DestroyTexture(assets[i]);
    }
}

void get_menu_size_tower(int size[], list_turret_avaliable *list, int *per_row){
	int correction, number, row, columns;
	
	number = get_tower_avaliable(list);
	
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
	
	size[0] = row * MENU_ICON;
	size[1] = columns * MENU_ICON;
	
	*per_row = row;
}

void get_menu_size_minion(int size[], list_minion_avaliable *list, int *per_row){
	int correction, number, row, columns;
	
	number = get_minion_avaliable(list);
	
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
	
	size[0] = row * MENU_ICON;
	size[1] = columns * MENU_ICON;
	
	*per_row = row;
}

void display_mouse(SDL_Renderer *renderer, SDL_Texture *select, bool active_clicked, bool selected_left, int select_grid, int grid_over, int center_clicked[], Game_Running_Options running_option, list_minion_avaliable *list_m, list_turret_avaliable *list_t, bool multiplayer) {
	int cartesian[] = {0,0};
	//bool display_over = false;
    
	get_cartesian_from_grid_number(grid_over, cartesian, 17);
	
	//Display grid_over
	SDL_RenderCopy(renderer, select, NULL, &(SDL_Rect){cartesian[0] - 35, cartesian[1] - 23 - ((cartesian[1]/block_height) * 0.3), block_width, block_height});
	
	
	if(active_clicked){
		int size[] = {0,0};
		int position[] = {0,0};
		int per_row = 0;
		int padding = 10;
		
		if(selected_left){
			//Get tower menu size
			get_menu_size_tower(size, list_t, &per_row);
		}
		else {
			//Get Minion menu size
			get_menu_size_minion(size, list_m, &per_row);
		}

		position[0] = center_clicked[0] - size[0] /2;
		position[1] = center_clicked[1] - size[1] / 2;
		
		//Render menu
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);  //Enable transparency
		SDL_SetRenderDrawColor(renderer, 146, 147, 52, 128); // the rect color (solid red)
		SDL_RenderFillRect(renderer, &(SDL_Rect){position[0] - padding, position[1] - padding, size[0] + padding *2, size[1] + padding *2});
		
		//Render minions or turrets
		if(selected_left){
			draw_turret_avaliable(renderer, list_t, per_row, position);
		}
		else {
			draw_minion_avaliable(renderer, list_m, per_row, position);
		}
		
		
	}
}

void display_health(SDL_Renderer *renderer, int value, TTF_Font *font){
    char str[5];
	printf("Foi 3.1\n");
    sprintf(str, "%d", value);
	printf("Foi 3.2 %s\n", str);
	if(!font){
		printf("Null font\n");
	}
    SDL_Surface *s = TTF_RenderText_Blended(font, str, (SDL_Color){255, 255, 255, 255});
	printf("Foi 3.3\n");
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, s);
	printf("Foi 3.4\n");
    
    SDL_FreeSurface(s);
	printf("Foi 3.5\n");
    
    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){1185, 40, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT});
	printf("Foi 3.6\n");
	SDL_DestroyTexture(texture);
	printf("Foi 3.7\n");
}

void display_mana(SDL_Renderer *renderer, int value, TTF_Font *font){
    char str[5];
    sprintf(str, "%d", value);
    SDL_Surface *s = TTF_RenderText_Blended(font, str, (SDL_Color){255, 255, 255, 255});
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, s);
    
    SDL_FreeSurface(s);
    
    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){1185, 130, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT});
	SDL_DestroyTexture(texture);
}

void display_gold(SDL_Renderer *renderer, int value, TTF_Font *font){
    char str[5];
    sprintf(str, "%d", value);
    SDL_Surface *s = TTF_RenderText_Blended(font, str, (SDL_Color){255, 255, 255, 255});
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, s);
    
    SDL_FreeSurface(s);
    
    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){1185, 240, BUTTON_MENU_HEIGHT, BUTTON_MENU_HEIGHT});
	SDL_DestroyTexture(texture);
}

