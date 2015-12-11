#include "Minion.h"
	
//Allocation Functions
///////////////////////////////////////////////////////////////////////
	
//Minion
minion *init_minion(list_minion_avaliable *list, int minionID){
	
	minion_avaliable *avaliable = NULL;
	avaliable = get_minion_from_avaliable_list(list, minionID);
	
	//printf("Type returned\n");
	if (avaliable == NULL){
		return NULL;
	}	

    // USAR MINIONID para diferentes minions dps.
    minion *new_minion = NULL;
    new_minion = calloc(1, sizeof(minion));
    
	//printf("New minion allocated\n");
	//printf("New minion allocated %s\n", avaliable->thumbnail_file);
	new_minion->node = init_node(avaliable->thumbnail_file, 0, 50);
    new_minion->HP = avaliable->HP;
    new_minion->speed = avaliable->speed;
    new_minion->minionType = minionID;
    
	new_minion->targetted_projectils = NULL;
    new_minion->targetted_projectils = init_list_projectile();
    return new_minion;
}

//Avaliable minion
minion_avaliable *init_avaliable_minion(char *image_file, int hp, float speed, int cost, float gold_p_s, int gold_drop){
    // USAR MINIONID para diferentes minions dps.
    minion_avaliable *new_minion = malloc(sizeof(minion_avaliable));
    new_minion->thumbnail = init_node(image_file, 0, 400);
	
	if(new_minion->thumbnail == NULL) {
		free(new_minion);
		new_minion = NULL;
		return NULL;
	}
	
	
    new_minion->HP = hp;
	strcpy(new_minion->thumbnail_file, image_file);
    new_minion->speed = speed;
    new_minion->gold_per_second_bonus = gold_p_s;
    new_minion->cost = cost;
    new_minion->gold_drop = gold_drop;
    
    return new_minion;
}

void remove_minion(minion **mium){
    if(*mium){
        free_node((*mium)->node);
        (*mium)->node = NULL;
        free_list_projectile((*mium)->targetted_projectils);
        (*mium)->targetted_projectils = NULL;
        free(*mium);
        *mium = NULL;
    }
}

/**
	Function to remove a minion avaliable on the software exit.
*/
void remove_avaliable_minion(minion_avaliable *mium){
    if(mium){
        free_node(mium->thumbnail);
        mium->thumbnail = NULL;
        free(mium);
        mium = NULL;
    }
}

//List Functions
///////////////////////////////////////////////////////////////////////

//List Minions
list_minion *init_list_minion(){
    list_minion *new_list = malloc(sizeof(list_minion));
    new_list->e = NULL;
    new_list->next = NULL;
    
    return new_list;
}


void free_list_minion(list_minion *list){
    list_minion *aux = list;
    
    while (aux && aux->next){
        remove_minion(&aux->e);
        aux->e = NULL;
        list_minion *rmv = aux;
        aux = aux->next;
        free(rmv);
        rmv = NULL;
    }
    if(aux){
		free(aux);
		aux = NULL;		
	}
	return;
}

void add_minion_to_list(list_minion *list, minion *minion){
    //Caso a lista esteja vazia ainda
    if(!list->e){
        list->e = minion;
        return;
    }
    
    while(list->next)
        list = list->next;
    
    list_minion *new_element = init_list_minion();
    new_element->e = minion;
    list->next = new_element;
}

void remove_minion_from_list(list_minion *list, minion **minion){
    list_minion *temp = list;
	
	while(temp){
        if(temp->e == *minion){
			remove_minion(minion);
			temp->e = NULL;
            if(temp->next){
                list_minion *aux = temp->next;
                temp->e = aux->e;
				temp->next = aux->next;
            }
            break;
        }
        temp = temp->next;
    }
    return;
}


//List Avaliable minions
list_minion_avaliable *init_avaliable_list_minion(){
    list_minion_avaliable *new_list = malloc(sizeof(list_minion_avaliable));
    new_list->e = NULL;
    new_list->type = 0;
    new_list->next = NULL;

    return new_list;
}

void free_avaliable_list_minion(list_minion_avaliable *list){
    list_minion_avaliable *aux = list;
    
    while (aux && aux->next){
        remove_avaliable_minion(aux->e);
        aux->e = NULL;
        list_minion_avaliable *rmv = aux;
        aux = aux->next;
        free(rmv);
        rmv = NULL;
    }
    
    free(aux);
    aux = NULL;
	return;
}

void add_minion_to_avaliable_list(list_minion_avaliable *list, minion_avaliable *minion, int type){
    //Caso a lista esteja vazia ainda
    if(!list->e){
		list->type = type;
        list->e = minion;
        return;
    }
    
    while(list->next)
        list = list->next;
    
    list_minion_avaliable *new_element = init_avaliable_list_minion();
    new_element->e = minion;
    new_element->type = type;
    list->next = new_element;
}

list_minion_avaliable *load_minions(char const *file_name){
	char *file = load_file(file_name);
	if(file == NULL) {
		return NULL;
	}
	
	int readed, len;
	char name[40];
	int hp, cost, gold_drop;
	float speed, gold_per_second;
	int type = 1;
	bool first = true;
	
	hp = gold_per_second = cost = gold_drop = 0;
	speed = gold_per_second = 0;
	
	
	//Read first line
	len = get_next_line(file);
	
	//Init avaliable minion list
	list_minion_avaliable *avaliables = init_avaliable_list_minion();
	while(1) {
		for(int i=0; i < 40; i++) {
			name[i] = '\0'; 
		}
		//Set point to next line
		file += len + 1;
		readed = sscanf(file, "%s %d %f %f %d %d\n", name, &hp, &speed, &gold_per_second, &cost, &gold_drop);
		if (readed != 6){
			break;
		}

		len = get_next_line(file);
		
		if (first) {
			first = false;
		}
		
		//Create minion and alloc.
		minion_avaliable *minion = init_avaliable_minion(name, hp, speed, cost, gold_per_second, gold_drop);
		if (minion != NULL){
			//Add to list	
			add_minion_to_avaliable_list(avaliables, minion, type);
			type++;
		}
	}
	
	if(first){
		//Unalloc list.
		free_avaliable_list_minion(avaliables);
		return NULL;
	}
	return avaliables;
}

//Get Functions
///////////////////////////////////////////////////////////////////////
	
int get_minion_avaliable(list_minion_avaliable *list) {
	//Get from tower files. < TODO
	list_minion_avaliable *temp = list;
	int avaliable = 0;
	
	while(temp != NULL){
		temp = temp->next;
		avaliable++;
	}
	
	return avaliable;
}

int get_minion_price(list_minion_avaliable *list, int turretID){
    minion_avaliable * available = get_minion_from_avaliable_list(list, turretID);
    
    return available->cost;
}

float get_minion_bonus(list_minion_avaliable *list, int turretID){
    minion_avaliable * available = get_minion_from_avaliable_list(list, turretID);
    
    return available->gold_per_second_bonus;
}

int get_minion_drop(list_minion_avaliable *list, int turretID){
    minion_avaliable * available = get_minion_from_avaliable_list(list, turretID);
    
    return available->gold_drop;
}

minion_avaliable *get_minion_from_avaliable_list(list_minion_avaliable *list, int type){
	list_minion_avaliable *temp_list = NULL;
	
	temp_list = list;
	while(temp_list->type != type && temp_list->next) {
		temp_list = temp_list->next;
	}
	
	if (temp_list != NULL && temp_list->e) {
		//printf("Returning type\n");
		return temp_list->e;
	}
	return NULL;
}
