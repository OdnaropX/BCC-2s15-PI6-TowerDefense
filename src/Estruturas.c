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
    SDL_FreeSurface(node->sprite);
    free(node);
}

//Minion
minion *init_minion(int minionID){
    // USAR MINIONID para diferentes minions dps.
    minion *new_minion = malloc(sizeof(minion));
    new_minion->node = *init_node("../images/Minion.png", 0, 400);
    new_minion->HP = 5;
    new_minion->speed = 3;
    
    new_minion->targetted_projectils = init_list_projectile();
    
    return new_minion;
}

void remove_minion(minion *mium){
    free_node(&mium->node);
    free(mium);
}

//Turret
turret *init_turret(int turretID, int gridX, int gridY){
    turret *new_turret = malloc(sizeof(turret));
    new_turret->node = *init_node("../images/Turret.png", gridX * 17, gridY * 13);
    new_turret->timeUntilNextAttack = 5.0; // Sempre começa sem atacar.
    new_turret->turretType = turretID;
    new_turret->radius = 200;
    
    return new_turret;
}

void remove_turret(turret *turret){
    free_node(&turret->node);
    free(turret);
}

//Projectile
projectile *init_projectile(int projectileID, turret* shooter){
    projectile *new_projectile = malloc(sizeof(projectile));
    new_projectile->node = *init_node("../images/Projectile.png", shooter->node.xPos, shooter->node.yPos);
    new_projectile->speed = 4;
    new_projectile->damage = 2;
    
    return new_projectile;
}

void remove_projectile(projectile *proj){
    free_node(&proj->node);
    free(proj);
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
    
    while (aux->next){
        remove_projectile(aux->e);
        list_projectile *rmv = aux;
        aux = aux->next;
        free(rmv);
    }
    
    free(aux);
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

void remove_projectile_from_list(list_projectile *list, projectile *projectile){
    list_projectile *remove = NULL;
    
    while(list){
        if(list->e == projectile){
            remove = list;
            
            if(list->next){
                list_projectile *aux = list->next;
                list->next = list;
                list = aux;
            }
        }
    }
    
    if(remove){
        remove_projectile(remove->e);
        free(remove);
    }
    
    else
        printf("Your princess is in another castle!\n");
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
    
    while (aux->next){
        remove_minion(aux->e);
        list_minion *rmv = aux;
        aux = aux->next;
        free(rmv);
    }
    
    free(aux);
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

void remove_minion_from_list(list_minion *list, minion *minion){
    list_minion *remove = NULL;
    
    while(list){
        if(list->e == minion){
            remove = list;
            
            if(list->next){
                list_minion *aux = list->next;
                list->next = list;
                list = aux;
            }
        }
    }
    
    if(remove){
        remove_minion(remove->e);
        free(remove);
    }
    
    else
        printf("Your princess is in another castle!\n");
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
    
    while (aux->next){
        remove_turret(aux->e);
        list_turret *rmv = aux;
        aux = aux->next;
        free(rmv);
    }
    
    free(aux);
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

void remove_turret_from_list(list_turret *list, turret *turret){
    list_turret *remove = NULL;
    
    while(list){
        if(list->e == turret){
            remove = list;
            
            if(list->next){
                list_turret *aux = list->next;
                list->next = list;
                list = aux;
            }
        }
    }
    
    if(remove){
        remove_turret(remove->e);
        free(remove);
    }
    
    else
        printf("Your princess is in another castle!\n");
}