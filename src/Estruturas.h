//
//  Estruturas.h
//
//  Created by Gabriel Nopper on 29/08/15.
//  Copyright (c) 2015 Gabriel Nopper. All rights reserved.
//

#ifndef __Tower_Defense__Estruturas__
#define __Tower_Defense__Estruturas__

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>


#define block_width 64
#define block_height 43
#define map_array_width 17
#define map_array_height 13
#define window_width 1280
#define window_height 720


typedef struct _list_projectile list_projectile;


typedef struct node
{
    double xPos;
    double yPos;
    SDL_Surface sprite;
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

typedef struct _projectile{
	int speed;
    int damage;
    node node;
};

// Linked lists
// List Shoots

typedef struct _list_projectile {
 	projectile *e;
 	struct _list_projectile *next;
} list_projectile;

// List minion

typedef struct _list_minion {
	minion *e;
	struct _list_minion *next;
} list_minion;

// List Tower

typedef struct _list_turret {
	turret *e;
	struct _list_turret *next;
} list_minion;

// É importante notar que não temos como nos assegurar de que o monstro existira para o tiro. Eventualmente teremos de, ou fazer o tiro mudar de alvo se o monstro morrer, ou fazer o monstro esperar até não ter tiros mirando nele.
// Favor importar as libs de SDL. Não tenho os arquivos da aula e não sei o que será necessário importar.


#endif /* defined(__Tower_Defense__Estruturas__) */