//
//  GameScene.h
//  
//
//  Created by Gabriel Nopper on 17/09/15.
//
//

#ifndef ____GameScene__
	#define ____GameScene__

	//Includes
	///////////////////////////////////////////////////////////////////////
	
	#include "Estruturas/Control.h"
	#include "Estruturas/Minion.h"
	#include "Estruturas/Turret.h"
	#include "Estruturas/Misc.h"
	#include "Estruturas/Projectile.h"

	//Constants
	///////////////////////////////////////////////////////////////////////
	
	#ifdef _WIN32
		#define MAP_IMAGE "images/Mapa.png"
		#define MAP_GRID "config/Map1.txt"
		#define MINION_FILE "config/MinionList.txt"
		#define TURRET_FILE "config/TurretList.txt"
		#define PROJECTILE_FILE "config/ProjectileList.txt"
		#define CONFIG_FILE "config/Config.txt"
	#else
		#define MAP_IMAGE "../images/Mapa.png"
		#define MAP_GRID "Map1.txt"
		#define MINION_FILE "MinionList.txt"
		#define TURRET_FILE "TurretList.txt"
		#define PROJECTILE_FILE "ProjectileList.txt"
		#define CONFIG_FILE "Config.txt"
	#endif
	
	//Use this on main init(or when loading a map)
	SDL_Surface *init_map();

	int is_time(int timer, int second);

	int perform_path_verification(int targetX, int targetY);

	void freeGrid(int targetX, int targetY);
	
	//Move Functions
	///////////////////////////////////////////////////////////////////////
	
	/**
	 Moves target minion a frame. He will go towards the center of the next grid to be moved into.
	 Returns position value.
	 **/
	int move_minion(minion *enemy);

	/**
	 Moves target bullet. It returns 1 if it hit the target, and 0 if it still not got there.
	 **/
	int move_bullet(minion *target, projectile *shoot);

	//Check Functions
	///////////////////////////////////////////////////////////////////////
	
	bool occupyGrid(int targetX, int targetY);
	
	//Get Functions
	///////////////////////////////////////////////////////////////////////
	
	void get_cartesian_from_grid_number(int grid_number, int *array_position, int limit_x);
	void get_grid_address_matriz(int grid_number, int limit_x, int *matriz);

	/**
	 Fills sent array[2] with x and y positions for screen, as center of the rect.
	 **/
	void get_center_position_to_grid(int x, int y, int array[]);

	/***
	  Get linear address from array address.
	  use example: get_grid_address_linear(1, 1, 17) --> 17
	**/
	int get_grid_address_linear(int grid_x, int grid_y, int limit_x);

	/**
	 Fills sent array with array adress for touched grid squares.
	 Returns:
	 1 if success.
	 0 if out of grid.
	 **/
	int get_touched_grid_address(int x_touch, int y_touch, int array[]);

	int get_touched_menu_address(int x_touch, int y_touch, int center_position[], int selected_option[], int tower, list_minion_avaliable *list_m, list_turret_avaliable *list_t);

#endif /* defined(____GameScene__) */