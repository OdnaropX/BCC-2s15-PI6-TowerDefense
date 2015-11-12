//
//  Estruturas.c
//
//  Created by Gabriel Nopper on 29/08/15.
//  Copyright (c) 2015 Gabriel Nopper. All rights reserved.
//

#include "Estruturas.h"

//Node
node *init_node(char *sprite_filename, double xPos, double yPos){
    node *new_node = malloc(sizeof(node));
    
    new_node->sprite = IMG_Load(sprite_filename);
    if(!new_node->sprite) {
        printf("Erro ao carregar sprite! %s\n", IMG_GetError());
        free(new_node);
        return NULL;
    }
    
    new_node->xPos = xPos;
    new_node->yPos = yPos;
    
    return new_node;
}

void free_node(node *node){
    if(node){
        if(node->sprite)
            SDL_FreeSurface(node->sprite);
        
        node->sprite = NULL;
        
        free(node);
        node = NULL;
    }
}

//Minion
minion *init_minion(int minionID){
    // USAR MINIONID para diferentes minions dps.
    minion *new_minion = malloc(sizeof(minion));
    new_minion->node = init_node("../images/Minion.png", 0, 400);
    new_minion->HP = 7;
    new_minion->speed = 4;
    new_minion->minionType = minionID;
    
    new_minion->targetted_projectils = init_list_projectile();
    
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

//Avaliable minion
minion_avaliable *init_avaliable_minion(char *image_file, int hp, float speed, int cost, int gold_p_s, int gold_drop){
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

void remove_avaliable_minion(minion_avaliable *mium){
    if(mium){
        free_node(mium->thumbnail);
        mium->thumbnail = NULL;
        free(mium);
        mium = NULL;
    }
}


//Turret
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

//Projectile
projectile *init_projectile(int projectileID, turret* shooter){
    projectile *new_projectile = malloc(sizeof(projectile));
    new_projectile->node = init_node("../images/Projectile.png", shooter->node->xPos, shooter->node->yPos);
    new_projectile->speed = 4;
    new_projectile->damage = 2;
    
    return new_projectile;
}

void remove_projectile(projectile *proj){
    if(proj){
        free_node(proj->node);
        proj->node = NULL;
        free(proj);
        proj = NULL;
    }
}

//List Projectiles
list_projectile *init_list_projectile(){
    list_projectile *new_list = malloc(sizeof(list_projectile));
    new_list->e = NULL;
    new_list->next = NULL;
    
    return new_list;
}

void free_list_projectile(list_projectile *list){
    list_projectile *aux = list;
    
    while (aux && aux->next){
        remove_projectile(aux->e);
        aux->e = NULL;
        
        list_projectile *rmv = aux;
        aux = aux->next;
        free(rmv);
        rmv = NULL;
    }
    
    free(aux);
    aux = NULL;
}

void add_projectile_to_list(list_projectile *list, projectile *projectile){
    //Caso a lista esteja vazia ainda
    if(!list->e){
        list->e = projectile;
        return;
    }
    while(list->next)
        list = list->next;
    
    list_projectile *new_element = init_list_projectile();
    new_element->e = projectile;
    list->next = new_element;
}

list_projectile *remove_projectile_from_list(list_projectile *list, projectile *projectile){
    list_projectile *first_node = list;
    list_projectile *remove = NULL;
    
    while(list->e && list != remove){
        
        if(list->e == projectile){
            remove = list;
            
            if(list->next){
                list_projectile *aux = list->next;
                list->next = list;
                list = aux;
            }
            
            break;
        }
        
        list = list->next;
    }
    
    if(remove){
        remove_projectile(remove->e);
		//remove_projectile(remove->next->e);//Need to check this for bug
        remove->e = NULL;
        remove->next = NULL;
        
        if(remove != first_node){
            free(remove);
            remove = NULL;
        }
    }
    
    else{
        printf("Your projectile is in another castle!\n");
    }
    
    return list;
}

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

//List Turrets
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

char* load_file(char const *file_name){
	SDL_RWops *rw = SDL_RWFromFile(file_name, "rb");
    if (rw == NULL) return NULL;

    Sint64 res_size = SDL_RWsize(rw);
    char* res = (char*) malloc(res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    char* buf = res;
    while (nb_read_total < res_size && nb_read != 0) {
		nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
	}
    SDL_RWclose(rw);
    if (nb_read_total != res_size) {
		free(res);
		return NULL;
    }

    res[nb_read_total] = '\0';
    return res;
}

list_minion_avaliable *load_minions(char const *file_name){
	char *file = load_file(file_name);
	if(file == NULL) {
		return false;
	}
	
	int readed = 6;
	char name[40];
	int hp, gold_per_second, cost, gold_drop;
	float speed;
	int type = 0;
	bool first = true;
	
	//Jump first line
	sscanf(file, "%*s\n", name);
	
	
	//Init avaliable minion list
	list_minion_avaliable *avaliables = init_avaliable_list_minion();
	while(1) {
		for(int i=0; i < 40; i++) {
			name[i] = '\0'; 
		}
		readed = sscanf(file, "%s %d %f %d %d %d", name, &hp, &speed, &gold_per_second, &cost, &gold_drop);
		if (readed != 6){
			break;
		}
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

minion_avaliable *get_minion_from_avaliable_list(list_minion_avaliable *list, int type){
	list_minion_avaliable *temp_list;
	
	temp_list = list;
	while(temp_list->type != type && temp_list->next) {
		temp_list = temp_list->next;
	}
	
	return temp_list;
}

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

int get_minion_avaliable(list_minion_avaliable *list) {
	//Get from tower files. < TODO
	list_minion_avaliable *temp = list;
	int avaliable = 0;
	
	while(temp){
		temp = temp->next;
		avaliable++;
	}
	
	return avaliable;
}