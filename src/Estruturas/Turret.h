#ifndef __Tower_Defense__Estruturas__Turret__
	#define __Tower_Defense__Estruturas__Turret__

	//Includes
	///////////////////////////////////////////////////////////////////////
	#include "Misc.h"
	
	//Structs
	///////////////////////////////////////////////////////////////////////
	
	typedef struct _turret turret;
	typedef struct _list_turret list_turret;
	typedef struct _list_turret_avaliable list_turret_avaliable;
	
	//Turret
	struct _turret{
		float timeUntilNextAttack; // Tempo de espera até atirar novamente.
		float turretType; // isto define o tipo de tiro. Alguma função usará este valor para escolher qual tiro é gerado.
		float radius; // alcance da torre.
		node *node;
	};
	
	// List Tower
	struct _list_turret {
		turret *e;
		struct _list_turret *next;
	};

	//List turret avaliable
	struct _list_turret_avaliable {
		turret *e;
		int type;
		struct _list_turret *next;
	};

	//Allocation Functions
	///////////////////////////////////////////////////////////////////////
	
	turret *init_turret(int turretID, int gridX, int gridY);
	void remove_turret(turret *turret);

	//List Functions
	///////////////////////////////////////////////////////////////////////

	list_turret *init_list_turret();
	void free_list_turret(list_turret *list);
	void add_turret_to_list(list_turret *list, turret *turret);
	list_turret *remove_turret_from_list(list_turret *list, turret *turret);

	//Get Functions
	///////////////////////////////////////////////////////////////////////
	
	int get_tower_avaliable(list_turret_avaliable *list);
	
#endif