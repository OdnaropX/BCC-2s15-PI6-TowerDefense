#include "Turret.h"

//Allocation Functions
///////////////////////////////////////////////////////////////////////

turret *init_turret(list_turret_avaliable *list, int turretID, int gridX, int gridY){
	turret_avaliable *avaliable = get_turret_from_avaliable_list(list, turretID);

	if (avaliable == NULL){
		return NULL;
	}	
	
    turret *new_turret = malloc(sizeof(turret));
	new_turret->node = init_node(avaliable->thumbnail_file, gridX * block_width + 10, gridY * block_height - block_height/2 + TOP_LAYER_SPACING + 4);
    new_turret->timeUntilNextAttack = avaliable->atack_speed; // Sempre começa sem atacar.
    new_turret->turretType = turretID;
    new_turret->radius = avaliable->atack_radius;
    
    return new_turret;
}

turret_avaliable *init_avaliable_turret(char *image_file, float atack_speed, float atack_radius, int cost){
    turret_avaliable *new_turret = malloc(sizeof(turret_avaliable));
    new_turret->thumbnail = init_node(image_file, block_width/2, block_height/2);
	
	if(new_turret->thumbnail == NULL) {
		free(new_turret);
		new_turret = NULL;
		return NULL;
	}
	strcpy(new_turret->thumbnail_file, image_file);
    new_turret->atack_speed = atack_speed;
    new_turret->atack_radius = atack_radius;
    new_turret->cost = cost;
    
    return new_turret;
}

void remove_turret(turret *turret){
    if(turret){
        free_node(turret->node);
        turret->node = NULL;
        free(turret);
        turret = NULL;
    }
}

void remove_avaliable_turret(turret_avaliable *turret){
	if(turret){
        free_node(turret->thumbnail);
        turret->thumbnail = NULL;
        free(turret);
        turret = NULL;
    }
}

//List Functions
///////////////////////////////////////////////////////////////////////

list_turret *init_list_turret(){
    list_turret *new_list = malloc(sizeof(list_turret));
    new_list->e = NULL;
    new_list->next = NULL;
    
    return new_list;
}

void free_list_turret(list_turret *list){
    list_turret *aux = list;
    
    while (aux && aux->next){
        remove_turret(aux->e);
        aux->e = NULL;
        
        list_turret *rmv = aux;
        aux = aux->next;
        free(rmv);
        rmv = NULL;
    }
    
    free(aux);
    aux = NULL;
}

void add_turret_to_list(list_turret *list, turret *turret){
    //Caso a lista esteja vazia ainda
    if(!list->e){
        list->e = turret;
        return;
    }
    
    while(list->next)
        list = list->next;
    
    list_turret *new_element = init_list_turret();
    new_element->e = turret;
    list->next = new_element;
}

list_turret *remove_turret_from_list(list_turret *list, turret *turret){
    list_turret *remove = NULL;
    
    while(list){
        if(list->e == turret){
            remove = list;
            
            if(list->next){
                list_turret *aux = list->next;
                list->next = list;
                list = aux;
            }
            
            break;
        }
        
        list = list->next;
    }
    
    if(remove){
        remove_turret(remove->e);
        //remove_turret(remove->next->e);
        remove->e = NULL;
        remove->next = NULL;
        
        if(remove != list){
            free(remove);
            remove = NULL;
        }
    }
    
    else{
        printf("Your turret is in another castle!\n");
    }
    
    return list;
}

list_turret_avaliable *init_avaliable_list_turret(){
    list_turret_avaliable *new_list = malloc(sizeof(list_turret_avaliable));
    new_list->e = NULL;
    new_list->type = 0;
    new_list->next = NULL;

    return new_list;
}

void free_avaliable_list_turret(list_turret_avaliable *list){
    list_turret_avaliable *aux = list;
    
    while (aux && aux->next){
        remove_avaliable_turret(aux->e);
        aux->e = NULL;
        list_turret_avaliable *rmv = aux;
        aux = aux->next;
        free(rmv);
        rmv = NULL;
    }
    
    free(aux);
    aux = NULL;
}
void add_turret_to_avaliable_list(list_turret_avaliable *list, turret_avaliable *turret, int type){
    //Caso a lista esteja vazia ainda
    if(!list->e){
		list->type = type;
        list->e = turret;
        return;
    }
    
    while(list->next)
        list = list->next;
    
    list_turret_avaliable *new_element = init_avaliable_list_turret();
    new_element->e = turret;
    new_element->type = type;
    list->next = new_element;
}

list_turret_avaliable *load_turrets(char const *file_name){
	char *file = load_file(file_name);
	if(file == NULL) {
		return false;
	}
	
	int readed, len;
	char name[40];
	int cost;
	float speed, radius;
	int type = 1;
	bool first = true;
	
	//Get next line to be jumped
	len = get_next_line(file);
	
	//Init avaliable turret list
	list_turret_avaliable *avaliables = init_avaliable_list_turret();
	while(1) {
		for(int i=0; i < 40; i++) {
			name[i] = '\0'; 
		}
		//Set point to next line
		file += len + 1;
		readed = sscanf(file, "%s %f %f %d", name, &speed, &radius, &cost);
		if (readed != 4){
			break;
		}
		
		len = get_next_line(file);
		if (first) {
			first = false;
		}
		
		//Create turret and alloc.
		turret_avaliable *turret = init_avaliable_turret(name, speed, radius, cost);
		if (turret != NULL){
			//Add to list	
			add_turret_to_avaliable_list(avaliables, turret, type);
			type++;
		}
	}
	
	if(first){
		//Unalloc list.
		free_avaliable_list_turret(avaliables);
		return NULL;
	}
	return avaliables;
}

//Get Functions
///////////////////////////////////////////////////////////////////////

int get_tower_avaliable(list_turret_avaliable *list) {
	list_turret_avaliable *temp = list;
	int avaliable = 0;
	
	while(temp != NULL){
		temp = temp->next;
		avaliable++;
	}
	
	return avaliable;
}

int get_turret_price(list_turret_avaliable *list, int turretID){
    turret_avaliable * available = get_turret_from_avaliable_list(list, turretID);
    
    return available->cost;
}

turret_avaliable *get_turret_from_avaliable_list(list_turret_avaliable *list, int type){
	list_turret_avaliable *temp_list;
	
	temp_list = list;
	while(temp_list->type != type && temp_list->next) {
		temp_list = temp_list->next;
	}
	
	if (temp_list != NULL) {
		return temp_list->e;
	}
	return NULL;
}