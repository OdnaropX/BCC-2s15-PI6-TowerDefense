#ifndef __Tower_Defense__Estruturas__Projectile__
	#define __Tower_Defense__Estruturas__Projectile__

	//Includes
	///////////////////////////////////////////////////////////////////////
	//#include "Misc.h" // Turret already includes Misc.
	#include "Turret.h"
	
	//Structs
	///////////////////////////////////////////////////////////////////////
	
	typedef struct _list_projectile list_projectile;
	typedef struct _list_projectile_avaliable list_projectile_avaliable;
	typedef struct _projectile projectile;
	typedef struct _projectiles projectile_avaliable;
	
	//Projectile
	struct _projectile {
		int speed;
		int damage;
		node *node;
	};
	
	struct _projectiles {
		node *thumbnail;
		char thumbnail_file[255];
		float speed;
		int damage;
	};
	
	// List Shoots
	struct _list_projectile {
		projectile *e;
		list_projectile *next;
	};
	
	struct _list_projectile_avaliable {
		projectile_avaliable *e;
		int type;
		list_projectile_avaliable *next;
	};
	
	
	//Allocation Functions
	///////////////////////////////////////////////////////////////////////
	
	projectile *init_projectile(int projectileID, turret* shooter);
	projectile_avaliable *init_avaliable_projectile(char *image_file, float speed, int damage);
	void remove_projectile(projectile *proj);
	void remove_avaliable_projectile(projectile_avaliable *proj);
	
	//List Functions
	///////////////////////////////////////////////////////////////////////
	
	/*Funções comuns às listas
	 Inits iniciam um elemento com *e nulo.
	 Frees removem TODA a lista
	 Adds adicionam no final da lista
	 Removes removem o elemento especificado
	*/

	list_projectile *init_list_projectile();
	void free_list_projectile(list_projectile *list);
	void add_projectile_to_list(list_projectile *list, projectile *projectile);
	list_projectile *remove_projectile_from_list(list_projectile *list, projectile *projectile);
	
	list_projectile_avaliable *init_avaliable_list_projectile();
	void free_avaliable_list_projectile(list_projectile_avaliable *list);
	void add_projectile_to_avaliable_list(list_projectile_avaliable *list, projectile_avaliable *projectile, int type);
	list_projectile_avaliable *load_projectiles(char const *file_name);
	
	//Get Functions
	///////////////////////////////////////////////////////////////////////
	int get_projectile_avaliable(list_projectile_avaliable *list);
	projectile_avaliable *get_projectile_from_avaliable_list(list_projectile_avaliable *list, int type);
	
#endif