#ifndef __Tower_Defense__Estruturas__Turret__
	#define __Tower_Defense__Estruturas__Turret__

	//Includes
	///////////////////////////////////////////////////////////////////////
	#include "Misc.h"
	
	//Structs
	///////////////////////////////////////////////////////////////////////
	
	typedef struct _turret turret;
	typedef struct _turrets turret_avaliable;
	typedef struct _list_turret list_turret;
	typedef struct _list_turret_avaliable list_turret_avaliable;
	
	//Turret
	struct _turret{
		float timeUntilNextAttack; // Tempo de espera até atirar novamente.
		float turretType; // isto define o tipo de tiro. Alguma função usará este valor para escolher qual tiro é gerado.
		float radius; // alcance da torre.
		node *node;
	};
	
	struct _turrets {
		node *thumbnail;
		float atack_speed;
		float atack_radius;
		int cost;
	};
	
	// List Tower
	struct _list_turret {
		turret *e;
		struct _list_turret *next;
	};

	//List turret avaliable
	struct _list_turret_avaliable {
		turret_avaliable *e;
		int type;
		struct _list_turret_avaliable *next;
	};

	//Allocation Functions
	///////////////////////////////////////////////////////////////////////
	
	turret *init_turret(int turretID, int gridX, int gridY);
	turret_avaliable *init_avaliable_turret(char *image_file, float atack_speed, float atack_radius, int cost);
	void remove_turret(turret *turret);
	void remove_avaliable_turret(turret_avaliable *turret);
	
	//List Functions
	///////////////////////////////////////////////////////////////////////

	list_turret *init_list_turret();
	void free_list_turret(list_turret *list);
	void add_turret_to_list(list_turret *list, turret *turret);
	list_turret *remove_turret_from_list(list_turret *list, turret *turret);

	list_turret_avaliable *init_avaliable_list_turret();
	void free_avaliable_list_turret(list_turret_avaliable *list);
	void add_turret_to_avaliable_list(list_turret_avaliable *list, turret_avaliable *turret, int type);
	list_turret_avaliable *load_turrets(char const *file_name);
	
	//Get Functions
	///////////////////////////////////////////////////////////////////////
	
	int get_tower_avaliable(list_turret_avaliable *list);
	turret_avaliable *get_turret_from_avaliable_list(list_turret_avaliable *list, int type);
	
#endif