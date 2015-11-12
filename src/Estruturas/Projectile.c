#include "Projectile.h"
	
//Allocation Functions
///////////////////////////////////////////////////////////////////////

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

//List Functions
///////////////////////////////////////////////////////////////////////
	
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
