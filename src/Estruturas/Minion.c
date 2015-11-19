#include "Minion.h"
	
//Allocation Functions
///////////////////////////////////////////////////////////////////////
	
//Minion
minion *init_minion(list_minion_avaliable *list, int minionID){
	minion_avaliable *avaliable = get_minion_from_avaliable_list(list, minionID);
	
	if (avaliable == NULL){
		return NULL;
	}	

    // USAR MINIONID para diferentes minions dps.
    minion *new_minion = malloc(sizeof(minion));
    new_minion->node = avaliable->thumbnail;
    new_minion->HP = avaliable->HP;
    new_minion->speed = avaliable->speed;
    new_minion->minionType = minionID;
    
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
    new_minion->speed = speed;
    new_minion->gold_per_second_bonus = gold_p_s;
    new_minion->cost = cost;
    new_minion->gold_drop = gold_drop;
    
    return new_minion;
}

void remove_minion(minion *mium){
    if(mium){
        free_node(mium->node);
        mium->node = NULL;
        free_list_projectile(mium->targetted_projectils);
        mium->targetted_projectils = NULL;
        free(mium);
        mium = NULL;
    }
}

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
        remove_minion(aux->e);
        aux->e = NULL;
        list_minion *rmv = aux;
        aux = aux->next;
        free(rmv);
        rmv = NULL;
    }
    
    free(aux);
    aux = NULL;
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

list_minion *remove_minion_from_list(list_minion *list, minion *minion){
    list_minion *remove = NULL;
    
    while(list){
        if(list->e == minion){
            remove = list;
            
            if(list->next){
                list_minion *aux = list->next;
                list->next = list;
                list = aux;
            }
            
            break;
        }
        
        list = list->next;
    }
    
    if(remove){
        remove_minion(remove->e);
        //remove_minion(remove->next->e);
        remove->e = NULL;
        remove->next = NULL;
        
        if(remove != list){
            free(remove);
            remove = NULL;
        }
    }
    
    else{
        printf("Your minion is in another castle!\n");
    }
    
    return list;
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
	int type = 0;
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
	
minion_avaliable *get_minion_from_avaliable_list(list_minion_avaliable *list, int type){
	list_minion_avaliable *temp_list;
	
	temp_list = list;
	while(temp_list->type != type && temp_list->next) {
		temp_list = temp_list->next;
	}
	
	if (temp_list != NULL) {
		return temp_list->e;
	}
	return NULL;
}
