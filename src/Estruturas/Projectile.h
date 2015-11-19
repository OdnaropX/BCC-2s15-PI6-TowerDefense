#ifndef __Tower_Defense__Estruturas__Projectile__
	#define __Tower_Defense__Estruturas__Projectile__

	//Includes
	///////////////////////////////////////////////////////////////////////
	//#include "Misc.h" // Turret already includes Misc.
	#include "Turret.h"
	
	//Structs
	///////////////////////////////////////////////////////////////////////
	
	typedef struct _list_projectile list_projectile;
	typedef struct _projectile projectile;

	//Projectile
	struct _projectile {
		int speed;
		int damage;
		node *node;
	};
	
	// List Shoots
	struct _list_projectile {
		projectile *e;
		list_projectile *next;
	};
	
	
	
	//Allocation Functions
	///////////////////////////////////////////////////////////////////////
	
	projectile *init_projectile(int projectileID, turret* shooter);
	void remove_projectile(projectile *proj);
	
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
	
#endif