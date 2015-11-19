#ifndef __Tower_Defense__Estruturas__Minion__
	#define __Tower_Defense__Estruturas__Minion__

	//Includes
	///////////////////////////////////////////////////////////////////////
	//#include "Misc.h" //Projectile already includes Misc.h
	#include "Projectile.h"
	
	//Structs
	///////////////////////////////////////////////////////////////////////
	
	typedef struct _minion minion;
	typedef struct _minions minion_avaliable;//Change to minion_type
	typedef struct _list_minion list_minion;
	typedef struct _list_minion_avaliable list_minion_avaliable;
	
	//Minion
	struct _minion {
		int HP; // Não é necessário um maxHP, considerando a inexistencia de cura.
		float speed; // Movimento por ciclo.
		node *node;
		int minionType;
		//Linked list of Shoot
		list_projectile *targetted_projectils;
	};

	struct _minions {
		node *thumbnail;
		int HP;
		float speed;
		float gold_per_second_bonus;//change from int
		int cost;
		int gold_drop;
	};

	// List minion
	struct _list_minion {
		minion *e;
		struct _list_minion *next;
	};
	
	struct _list_minion_avaliable {
		int type;
		minion_avaliable *e;
		struct _list_minion_avaliable *next;
	};
	
	//Allocation Functions
	///////////////////////////////////////////////////////////////////////
	
	minion *init_minion(list_minion_avaliable *list, int minionID);
	minion_avaliable *init_avaliable_minion(char *image_file, int hp, float speed, int cost, float gold_p_s, int gold_drop);
	void remove_minion(minion *mium);
	void remove_avaliable_minion(minion_avaliable *mium);

	//List Functions
	///////////////////////////////////////////////////////////////////////
	
	/*Funções comuns às listas
	 Inits iniciam um elemento com *e nulo.
	 Frees removem TODA a lista
	 Adds adicionam no final da lista
	 Removes removem o elemento especificado
	*/

	list_minion *init_list_minion();
	void free_list_minion(list_minion *list);
	void add_minion_to_list(list_minion *list, minion *minion);
	list_minion *remove_minion_from_list(list_minion *list, minion *minion);

	list_minion_avaliable *init_avaliable_list_minion();
	void free_avaliable_list_minion(list_minion_avaliable *list);
	void add_minion_to_avaliable_list(list_minion_avaliable *list, minion_avaliable *minion, int type);
	list_minion_avaliable *load_minions(char const *file_name);

	//Get Functions
	///////////////////////////////////////////////////////////////////////
	
	int get_minion_avaliable(list_minion_avaliable *list);
	minion_avaliable *get_minion_from_avaliable_list(list_minion_avaliable *list, int type);
	
#endif