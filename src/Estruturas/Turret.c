#include "Turret.h"

//Allocation Functions
///////////////////////////////////////////////////////////////////////

turret *init_turret(int turretID, int gridX, int gridY){
    turret *new_turret = malloc(sizeof(turret));
    new_turret->node = init_node("../images/Turret.png", gridX * block_width, gridY * block_height);
    new_turret->timeUntilNextAttack = 5.0; // Sempre começa sem atacar.
    new_turret->turretType = turretID;
    new_turret->radius = 200;
    
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

//Get Functions
///////////////////////////////////////////////////////////////////////

int get_tower_avaliable(list_turret_avaliable *list) {
	//Get from minion files. < TODO
	list_turret_avaliable *temp = list;
	int avaliable = 0;
	
	while(temp){
		temp = temp->next;
		avaliable++;
	}
	
	return avaliable;
}

#endif