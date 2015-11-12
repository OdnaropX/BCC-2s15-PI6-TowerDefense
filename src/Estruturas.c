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
    //In order: img hp speed GoldPerSecondBonus cost GoldDrop

    char liner[100];
    FILE *file = fopen("MinionList.txt", "r");
    int i;
    for(i = 0; i <= minionID; i++)
        fgets(liner, 100, file);
    
    char imagename[20];
    int GpS;
    int drop;
    int cost;
    sscanf(liner, "%s %d %f %d %d %d", imagename, &new_minion->HP, &new_minion->speed, &GpS, &cost, &drop);
    char imageDir[30];
    sprintf(imageDir, "../images/%s", imagename);
    new_minion->node = init_node(imageDir, 0, 400);

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

//Turret
turret *init_turret(int turretID, int gridX, int gridY){
    turret *new_turret = malloc(sizeof(turret));
    //In order: img atkspeed radius cost

    char liner[100];
    FILE *file = fopen("TurretList.txt", "r");
    int i;
    for(i = 0; i <= turretID; i++)
        fgets(liner, 100, file);
    
    char imagename[20];
    int cost;
    sscanf(liner, "%s %f %f %d", imagename, &new_turret->timeUntilNextAttack, &new_turret->radius, &cost);
    char imageDir[30];
    sprintf(imageDir, "../images/%s", imagename);
    
    new_turret->node = init_node(imageDir, gridX * block_width, gridY * block_height);
    new_turret->turretType = turretID;
    
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
    //In order: img movespeed dmg

    char liner[100];
    FILE *file = fopen("TurretList.txt", "r");
    int i;
    for(i = 0; i <= projectileID; i++)
        fgets(liner, 100, file);
    
    char imagename[20];
    sscanf(liner, "%s %d %d", imagename, &new_projectile->speed, &new_projectile->damage);
    char imageDir[30];
    sprintf(imageDir, "../images/%s", imagename);
    
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

int get_tower_avaliable() {
	//Get from tower files. < TODO
	
	
	
	return 3;
}
int get_minion_avaliable() {
	//Get from minion files. < TODO
	
	
	
	return 3;
}

int getTurretCost(int turretID){
    char liner[100];
    FILE *file = fopen("TurretList.txt", "r");
    int i;
    for(i = 0; i <= turretID; i++)
        fgets(liner, 100, file);
    
    char filltext[20];
    int cost;
    float fill;
    sscanf(liner, "%s %f %f %d", filltext, &fill, &fill, &cost);
    
    return cost;
}

float getTurretCooldown(int turretID){
    char liner[100];
    FILE *file = fopen("TurretList.txt", "r");
    int i;
    for(i = 0; i <= turretID; i++)
        fgets(liner, 100, file);
    
    char filltext[20];
    int fill;
    float cooldown, ffill;
    sscanf(liner, "%s %f %f %d", filltext, &cooldown, &ffill, &fill);
    
    return cooldown;
}

int minionGoldDrop(int minionID){
    
    char liner[100];
    FILE *file = fopen("MinionList.txt", "r");
    int i;
    for(i = 0; i <= minionID; i++)
        fgets(liner, 100, file);
    
    char imagename[20];
    int fill;
    float ffill;
    int drop;
    sscanf(liner, "%s %d %f %d %d %d", imagename, &fill, &ffill, &fill, &fill, &drop);
    return drop;
}

int minionCost(int minionID){
    
    char liner[100];
    FILE *file = fopen("MinionList.txt", "r");
    int i;
    for(i = 0; i <= minionID; i++)
        fgets(liner, 100, file);
    
    char imagename[20];
    int fill;
    float ffill;
    int cost;
    sscanf(liner, "%s %d %f %d %d %d", imagename, &fill, &ffill, &fill, &cost, &fill);
    return cost;
}









