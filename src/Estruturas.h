//
//  Estruturas.h
//
//  Created by Gabriel Nopper on 29/08/15.
//  Copyright (c) 2015 Gabriel Nopper. All rights reserved.
//

#ifndef __Tower_Defense__Estruturas__
#define __Tower_Defense__Estruturas__

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#define TOP_LAYER_SPACING 83

#define block_width 64
#define block_height 43
#define map_array_width 17
#define map_array_height 13

typedef struct _list_projectile list_projectile;

//Node
typedef struct _node
{
    double xPos;
    double yPos;
    SDL_Surface *sprite;
} node;

node *init_node(char *sprite_filename, double xPos, double yPos);
void free_node(node *node);

//Minion
typedef struct _minion {
    int HP; // Não é necessário um maxHP, considerando a inexistencia de cura.
    float speed; // Movimento por ciclo.
    node node;
	//Linked list of Shoot
	list_projectile *targetted_projectils;
} minion;

minion *init_minion(int minionID);
void remove_minion(minion *mium);

//Turret
typedef struct _turret{
    float timeUntilNextAttack; // Tempo de espera até atirar novamente.
    float turretType; // isto define o tipo de tiro. Alguma função usará este valor para escolher qual tiro é gerado.
    float radius; // alcance da torre.
    node node;
} turret;

turret *init_turret(int turretID, int gridX, int gridY);
void remove_turret(turret *turret);

//Projectile
typedef struct _projectile {
	int speed;
    int damage;
    node node;
} projectile;

projectile *init_projectile(int projectileID, turret* shooter);
void remove_projectile(projectile *proj);

// Linked lists
/*Funções comuns às listas
 Inits iniciam um elemento com *e nulo.
 Frees removem TODA a lista
 Adds adicionam no final da lista
 Removes removem o elemento especificado
*/

// List Shoots
typedef struct _list_projectile {
 	projectile *e;
 	struct _list_projectile *next;
} list_projectile;

list_projectile *init_list_projectile();
void free_list_projectile(list_projectile *list);
void add_projectile_to_list(list_projectile *list, projectile *projectile);
void remove_projectile_from_list(list_projectile *list, projectile *projectile);

// List minion
typedef struct _list_minion {
	minion *e;
	struct _list_minion *next;
} list_minion;

list_minion *init_list_minion();
void free_list_minion(list_minion *list);
void add_minion_to_list(list_minion *list, minion *minion);
void remove_minion_from_list(list_minion *list, minion *minion);

// List Tower
typedef struct _list_turret {
	turret *e;
	struct _list_turret *next;
} list_turret;

#Define MAIN_SCREEN

list_turret *init_list_turret();
void free_list_turret(list_turret *list);
void add_turret_to_list(list_turret *list, turret *turret);
void remove_turret_from_list(list_turret *list, turret *turret);


typedef enum _screens {MAIN, CONFIG, GAME_RUNNING, GAME_PAUSED, CREDITS, SCORE} screen;

	 
typedef enum _tab_location{TOP_MENU, LEFT_MENU, BOTTOM_MENU, GAME_AREA} tab;

typedef enum _main_options {OPT_PLAY, OPT_CONFIG, OPT_SCORE, OPT_EXIT, OPT_CREDIT, OPT_NONE} main_options;

typedef enum _pause_options {OPT_P_RESUME, OPT_P_CONFIG, OPT_P_SCORE, OPT_P_EXIT, OPT_P_MAIN, OPT_P_NONE} pause_options;

typedef enum _config_options{MUSIC_EFFECT, MUSIC_AMBIENCE, LANGUAGE, BACK, NONE} config_options;

typedef struct _configuration {
	bool music_effect;
	bool music_ambience;
	char *language;
} CONFIGURATION;


// É importante notar que não temos como nos assegurar de que o monstro existira para o tiro. Eventualmente teremos de, ou fazer o tiro mudar de alvo se o monstro morrer, ou fazer o monstro esperar até não ter tiros mirando nele.
// Favor importar as libs de SDL. Não tenho os arquivos da aula e não sei o que será necessário importar.


#endif /* defined(__Tower_Defense__Estruturas__) */