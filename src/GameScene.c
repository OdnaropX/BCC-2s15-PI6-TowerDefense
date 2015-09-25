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


/*
 Retorna 0 em sucesso e 1 em caso de colisão.
 */
int move_bullet(minion *target, projectile *shoot){
    
    // 1 - Get required properties.
    int diff_x = target.node.xPos - shoot.node.xPos;
    int diff_y = target.node.yPos - shoot.node.yPos;
    diff_x = abs(diff_x);
    diff_y = abs(diff_y);
    int moveX = projectile.speed;
    int moveY = projectile.speed;
    
    if(diff_x - moveX < 0)
        moveX = diff_x;
    if(diff_y - moveY < 0)
        moveY = diff_y;
    
    // 2 - Sees position, then sums value
    if(target.node.xPos < shoot.node.xPos){ // Tiro à direita, inverte sinal
        moveX = -moveX;
    }
    if(target.node.yPos < shoot.node.yPos){ // Tiro abaixo, inverte sinal
        moveY = -moveY;
    }
    
    shoot.node.xPos += moveX;
    shoot.node.yPos += moveY;

    // 3 - Finishing touches, verifies collision.
    int diff_x = target.node.xPos - shoot.node.xPos;
    int diff_y = target.node.yPos - shoot.node.yPos;
    diff_x = abs(diff_x);
    diff_y = abs(diff_y);
    
    if(diff_x < 5 && diff_y < 5)
        return 1;
    else
        return 0;
}




/*
 typedef struct node
 {
 double xPos;
 double yPos;
 SDL_Surface *sprite;
 } node;
 
 
 typedef struct _minion {
 int HP; // Não é necessário um maxHP, considerando a inexistencia de cura.
 float speed; // Movimento por ciclo.
 node node;
	//Linked list of Shoot
	list_projectile *targetted_projectils;
 } minion;
 
 typedef struct _turret{
 float timeUntilNextAttack; // Tempo de espera até atirar novamente.
 float turretType; // isto define o tipo de tiro. Alguma função usará este valor para escolher qual tiro é gerado.
 float radius; // alcance da torre.
 node node;
 } turret;
 
 typedef struct _projectile {
	int speed;
 int damage;
 node node;
 } projectile;

 */

/**
 Draws a node considering it's x and y points at the center of the drawn point.
 1- The screen is where to draw.
 2- The drawn_node is the node you want to draw.
 3- Boolean value tower limits draw position, so that towers are placed into slots properly.
 **/
int Draw_Node(SDL_Surface *screen, node *drawn_node, bool tower){
    SDL_Rect rect;
    rect.w = drawn_node->sprite->w;
    rect.h = drawn_node->sprite->h;
    rect.x = drawn_node->xPos - (rect.w/2);
    rect.y = drawn_node->yPos - (rect.h/2);
    
    if(tower){
        rect.y = drawn_node->yPos - rect.h + (rect.w/2);
    }
    
    SDL_BlitSurface(drawn_node->sprite, NULL, screen, &rect);
    
    return 0;
}



/**
 Fills sent array with array adress for touched grid squares. 
 Returns:
 1 if success.
 0 if out of grid.
 **/
int getTouchedGridAddress(int x_touch, int y_touch, int array[]){
    int newY = y_touch - TOP_LAYER_SPACING;
    if(newY <= 0){
        return 0; // Out of bounds
    }
    
    int xAdd = x_touch/block_width;
    int yAdd = newY/block_height;
    
    if(xAdd > 17 || yAdd > 13){
        return 0; // Out of bounds
    }
    
    array[0] = xAdd;
    array[1] = yAdd;
    
    return 1; // Success.
}

/**
 Fills sent array[2] with x and y positions for screen, as center of the rect.
 **/
void getCenterPositionToGrid(int x, int y, int array[]){
    array[0] = x*block_width + block_width/2;
    array[1] = TOP_LAYER_SPACING + y*block_height + block_height/2;
}


