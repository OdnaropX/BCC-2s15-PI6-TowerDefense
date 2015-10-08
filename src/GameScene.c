//
//  GameScene.c
//  
//
//  Created by Gabriel Nopper on 17/09/15.
//
//

#include "GameScene.h"
#include "Estruturas.h"
#include <math.h>

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "Renderer.h"

int grid[17][13];



//Use this on main init(or when loading a map)
SDL_Surface *init_map(){
    //Load map image
    SDL_Surface *map_Image = IMG_Load("../images/Mapa.png");
    if(!map_Image){
        printf("Imagem do mapa não encontrada! %s\n", IMG_GetError());
        return NULL;
    }
    
    //Load map grid
    FILE *mapGrid = fopen("Map1.txt", "r");
    if(!mapGrid){
        printf("Txt de grid do mapa não encontrado!\n");
        return NULL;
    }
    
    for(int w = 0; w < 17; w++){
        for(int h = 0; h < 13; h++){
            fscanf(mapGrid, "%d ", &grid[w][h]);
        }
        fscanf(mapGrid, "\n");
    }
    
    return map_Image;
}


/*
 Retorna 0 em sucesso e 1 em caso de colisão.
 */
int move_bullet(minion *target, projectile *shoot){
    
    // 1 - Get required properties.
    int diff_x = target->node.xPos - shoot->node.xPos;
    int diff_y = target->node.yPos - shoot->node.yPos;
    diff_x = abs(diff_x);
    diff_y = abs(diff_y);
    int moveX = shoot->speed;
    int moveY = shoot->speed;
    
    if(diff_x - moveX < 0)
        moveX = diff_x;
    if(diff_y - moveY < 0)
        moveY = diff_y;
    
    // 2 - Sees position, then sums value
    if(target->node.xPos < shoot->node.xPos){ // Tiro à direita, inverte sinal
        moveX = -moveX;
    }
    if(target->node.yPos < shoot->node.yPos){ // Tiro abaixo, inverte sinal
        moveY = -moveY;
    }
    
    shoot->node.xPos += moveX;
    shoot->node.yPos += moveY;

    // 3 - Finishing touches, verifies collision.
    diff_x = target->node.xPos - shoot->node.xPos;
    diff_y = target->node.yPos - shoot->node.yPos;
    diff_x = abs(diff_x);
    diff_y = abs(diff_y);
    
    if(diff_x < 5 && diff_y < 5)
        return 1;
    else
        return 0;
}

void move_minion(minion *enemy){
    
    // 1 - Find target center to move.
    int square[2];
    get_touched_grid_address(enemy->node.xPos, enemy->node.yPos, square);
    int currentX = square[0];
    int currentY = square[1];
    int currentValue = grid[currentX][currentY];
    int targetX = currentX; // possible X
    int targetY = currentY; // possible Y
    int targetValue = currentValue; // Candidate value
    if(currentX > 0)
        if(grid[currentX - 1][currentY] < currentValue && grid[currentX - 1][currentY] > 0){
            currentValue = grid[currentX - 1][currentY];
            targetX = currentX - 1;
            targetY = targetY;
        }
    if(currentX < 16)
        if(grid[currentX + 1][currentY] < currentValue && grid[currentX + 1][currentY] > 0){
            currentValue = grid[currentX + 1][currentY];
            targetX = currentX + 1;
            targetY = targetY;
        }
    if(currentY > 0)
        if(grid[currentX][currentY - 1] < currentValue && grid[currentX][currentY - 1] > 0){
            currentValue = grid[currentX][currentY - 1];
            targetX = currentX;
            targetY = targetY - 1;
        }
    if(currentY < 12)
        if(grid[currentX][currentY + 1] < currentValue && grid[currentX][currentY + 1] > 0){
            currentValue = grid[currentX][currentY + 1];
            targetX = currentX;
            targetY = targetY + 1;
        }
    
    
    // 2 - Now you have target center. Move.
    int moveSquare[2];
    get_center_position_to_grid(targetX, targetY, moveSquare);
    
    int newX = moveSquare[0];
    int newY = moveSquare[1];
    
    int diff_x = enemy->node.xPos - newX;
    int diff_y = enemy->node.yPos - newY;
    diff_x = abs(diff_x);
    diff_y = abs(diff_y);
    int moveX = enemy->speed;
    int moveY = enemy->speed;
    
    if(diff_x - moveX < 0)
        moveX = diff_x;
    if(diff_y - moveY < 0)
        moveY = diff_y;
    
    if(enemy->node.xPos < newX){
        moveX = -moveX;
    }
    if(enemy->node.yPos < newY){
        moveY = -moveY;
    }
    
    enemy->node.xPos += moveX;
    enemy->node.yPos += moveY;
}

/**
 Fills sent array with array[2] adresses for touched grid squares.
 Returns:
 1 if success.
 0 if out of grid.
 **/
int get_touched_grid_address(int x_touch, int y_touch, int array[]){
    int newY = y_touch - TOP_LAYER_SPACING;
    if(newY <= 0){
        return 0; // Out of bounds
    }
    
    int xAdd = x_touch/block_width;
    int yAdd = newY/block_height;
    
    if(xAdd >= 17 || yAdd >= 13){
        return 0; // Out of bounds
    }
    
    array[0] = xAdd;
    array[1] = yAdd;
    
    return 1; // Success.
}

/**
 Fills sent array[2] with x and y positions for screen, as center of the rect.
 x and y are values returned on get_touched_grid_address.
 **/
void get_center_position_to_grid(int x, int y, int array[]){
    array[0] = x * block_width + block_width /2;
    array[1] = TOP_LAYER_SPACING + y*block_height + block_height/2;
}

/***
  Get linear address from array address.
  use example: get_grid_address_linear(1, 1, 17) --> 17
  0, 0, 17 = 0
  0, 1, 17 = 16
  1, 0, 17 = 1
  3, 4, 17 = 67
**/
int get_grid_address_linear(int grid_x, int grid_y, int limit_x){
	return grid_y * (limit_x - 1) + grid_x;
}

void get_grid_address_matriz(int grid_number, int limit_x, int *matriz) {
	int grid_x, grid_y, limit;
	
	limit = limit_x - 1;
	grid_x = grid_number % limit;
	grid_y = (grid_number - grid_x) / limit;
	
	matriz[0] = grid_x;
	matriz[1] = grid_y;
}

int get_touched_menu_address(int x_touch, int y_touch, int center_position[], int tower){
	int number;
	int row, width, height, columns, correction;
	
	if (tower) {
		number = 3; //Get from file or some other means.
	}
	else {
		number = 3; //Get from file or some other means.
	}
	
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
	
	width = columns * BUTTON_MENU_HEIGHT;
	height = row * BUTTON_MENU_HEIGHT;
	
	if (x_touch < center_position[0] - width / 2 && x_touch > center_position[0] + width / 2) {
		return 0;
	}
	if (y_touch < center_position[1] - height / 2 && x_touch > center_position[1] + height / 2) {
		return 0;
	}
	
	x_touch = x_touch - center_position[0] - width / 2;
	y_touch = y_touch - center_position[1] - height / 2;
	
	int xAdd = x_touch/BUTTON_MENU_HEIGHT;
	int yAdd = y_touch/BUTTON_MENU_HEIGHT;
	
	center_position[0] = xAdd;
	center_position[1] = yAdd;
	
	return 1;
}

void get_cartesian_from_grid_number(int grid_number, int *array_position, int limit_x){
	//Get grid coordenate.
	get_grid_address_matriz(grid_number, limit_x, array_position);
	
	//Get pixel central from grid coordenate.
	get_center_position_to_grid(array_position[0], array_position[1], array_position);	
}

int perform_path_verification(int targetX, int targetY){
    return setShortestPaths(grid, 17, 13, targetX, targetY);
}

void occupyGrid(int targetX, int targetY){
    grid[targetX][targetY] = -1;
}

void freeGrid(int targetX, int targetY){
    grid[targetX][targetY] = 0;
}

//Check relapse time in seconds
int is_time(int timer, int second){
	return timer / second > 0 && timer % second == 0;
}