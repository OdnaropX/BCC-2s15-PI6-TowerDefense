#include "Projectile.h"
	
//Allocation Functions
///////////////////////////////////////////////////////////////////////

//Projectile
projectile *init_projectile(list_projectile_avaliable *list, turret* shooter){
    projectile_avaliable *available = get_projectile_from_avaliable_list(list, shooter->turretType);
    
    if (available == NULL){
        return NULL;
    }
    
    projectile *new_projectile = malloc(sizeof(projectile));
	
    new_projectile->node = init_node(available->thumbnail_file, shooter->node->xPos + 17, shooter->node->yPos + 34);
	
    new_projectile->speed = available->speed;
    new_projectile->damage = available->damage;
    
    return new_projectile;
}

/**
	Function to initiate a avaliable list of projectiles.
*/
projectile_avaliable *init_avaliable_projectile(char *image_file, float speed, int damage){
	projectile_avaliable *new_proj = malloc(sizeof(projectile_avaliable));
    new_proj->thumbnail = init_node(image_file, 0, 400);
	
	if(new_proj->thumbnail == NULL) {
		free(new_proj);
		new_proj = NULL;
		return NULL;
	}
	
	strncpy(new_proj->thumbnail_file, image_file, FILENAME_LIMIT);
    
	new_proj->speed = speed;
    new_proj->damage = damage;
    
    return new_proj;
}

/**
	Function to remove a previously allocated projectile.
*/
void remove_projectile(projectile *proj){
    if(proj){
        free_node(proj->node);
		proj->node = NULL;
		
        free(proj);
        proj = NULL;
    }
}

void remove_avaliable_projectile(projectile_avaliable *proj){
	if(proj){
        free_node(proj->thumbnail);
        proj->thumbnail = NULL;
        free(proj);
        proj = NULL;
    }
}


//List Functions
///////////////////////////////////////////////////////////////////////
	
//List Projectiles
list_projectile *init_list_projectile(){
	list_projectile *new_list = NULL;
	
    new_list = calloc(1, sizeof(list_projectile));

	if(new_list){
		new_list->e = NULL;
		new_list->next = NULL;
	}
	else {
		printf("Could not create list\n");
	}
	
    return new_list;
}

void free_list_projectile(list_projectile *list){
    list_projectile *aux = list;
    
    while (aux && aux->next){
        remove_projectile(aux->e);
        aux->e = NULL;
        
        list_projectile *rmv = aux;
        aux = aux->next;
        if(rmv){
            free(rmv);
        }
        rmv = NULL;
    }
    if(aux){
		free(aux);
		aux = NULL;
	}
	return;
}

void add_projectile_to_list(list_projectile *list, projectile *projectile){
    if(list){
		//Caso a lista esteja vazia ainda
		if(list->e == NULL){
			list->e = projectile;
			return;
		}
		while(list->next != NULL)
			list = list->next;
		
		list_projectile *new_element = NULL;
		new_element = init_list_projectile();
		
		if(new_element){
			new_element->e = projectile;
			list->next = new_element;
		}
		else {
			printf("There is no new list element created\n");
		}
	}
	else {
		printf("null list\n");
	}
}

list_projectile *remove_projectile_from_list(list_projectile *list, projectile *projectile){
	list_projectile *temp = list;
	while(temp) {
        if(temp->e == projectile){
			remove_projectile(temp->e);
			temp->e = NULL;
            if(temp->next){
                list_projectile *aux = temp->next;
                temp->e = aux->e;
				temp->next = aux->next;
            }
            break;
        }
        temp = temp->next;
    }
	return list;
}

list_projectile_avaliable *init_avaliable_list_projectile(){
    list_projectile_avaliable *new_list = malloc(sizeof(list_projectile_avaliable));
    new_list->e = NULL;
    new_list->type = 0;
    new_list->next = NULL;

    return new_list;
}

void free_avaliable_list_projectile(list_projectile_avaliable *list){
    list_projectile_avaliable *aux = list;
    
    while (aux && aux->next){
        remove_avaliable_projectile(aux->e);
        aux->e = NULL;
        list_projectile_avaliable *rmv = aux;
        aux = aux->next;
        free(rmv);
        rmv = NULL;
    }
    
    free(aux);
    aux = NULL;
}

void add_projectile_to_avaliable_list(list_projectile_avaliable *list, projectile_avaliable *projectile, int type){
    //Caso a lista esteja vazia ainda
    if(!list->e){
		list->type = type;
        list->e = projectile;
        return;
    }
    
    while(list->next)
        list = list->next;
    
    list_projectile_avaliable *new_element = init_avaliable_list_projectile();
    new_element->e = projectile;
    new_element->type = type;
    list->next = new_element;
}

list_projectile_avaliable *load_projectiles(char const *file_name){
	char *file = load_file(file_name);
	if(file == NULL) {
		return false;
	}
	
	int readed, len;
	char name[40];
	int damage = 0;
	float speed = 0;
	int type = 1;
	bool first = true;
	
	//Get next line to be jumped
	len = get_next_line(file);
	
	//Init avaliable turret list
	list_projectile_avaliable *avaliables = init_avaliable_list_projectile();
	while(1) {
		for(int i=0; i < 40; i++) {
			name[i] = '\0'; 
		}
		//Set point to next line
		file += len + 1;
		readed = sscanf(file, "%s %f %d\n", name, &speed, &damage);
		if (readed != 3){
			break;
		}
		
		len = get_next_line(file);
		if (first) {
			first = false;
		}
		
		//Create turret and alloc.
		projectile_avaliable *projectile = init_avaliable_projectile(name, speed, damage);
		if (projectile != NULL){
			//Add to list	
			add_projectile_to_avaliable_list(avaliables, projectile, type);
			type++;
		}
	}
	
	if(first){
		//Unalloc list.
		free_avaliable_list_projectile(avaliables);
		return NULL;
	}
	return avaliables;
}

int get_projectile_avaliable(list_projectile_avaliable *list) {
	list_projectile_avaliable *temp = list;
	int avaliable = 0;
	
	while(temp != NULL){
		temp = temp->next;
		avaliable++;
	}
	
	return avaliable;
}

projectile_avaliable *get_projectile_from_avaliable_list(list_projectile_avaliable *list, int type){
	list_projectile_avaliable *temp_list;
	
	temp_list = list;
	while(temp_list->type != type && temp_list->next) {
		temp_list = temp_list->next;
	}
	
	if (temp_list != NULL) {
		return temp_list->e;
	}
	return NULL;
}