//
//  Estruturas.c
//
//  Created by Gabriel Nopper on 29/08/15.
//  Copyright (c) 2015 Gabriel Nopper. All rights reserved.
//

#include "Estruturas.h"


minion *init_minion(int minionID){
    
    // USAR MINIONID para diferentes minions dps.
    minion *new_minion = malloc(sizeof(minion));
    node *minionNode = malloc(sizeof(node));
    new_minion.node = minionNode; //Redundante?
    new_minion.HP = 5;
    new_minion.speed = 3;
    list_projectile *list = NULL;
    
    new_minion.node.xPos = 0;
    new_minion.node.yPos = 400;
    new_minion.node.sprite = IMG_Load("Minion.png");
    
    return new_minion;
}


void remove_minion(minion *mium){
    free(mium.sprite)
    free(mium);
}

turret *init_turret(int turretID, int gridX, int gridY){
    turret *new_turret = malloc(sizeof(turret));
    node *turretNode = malloc(sizeof(node));
    new_turret.node = turretNode;
    new_turret.timeUntilNextAttack = 5.0; // Sempre começa sem atacar.
    new_turret.turretType = turretID;
    new_turret.radius = 200;
    
    
    new_turret.node.xPos = gridX * block_width;
    new_turret.node.yPos = gridY * block_height;
    new_turret.node.sprite = IMG_Load("Turret.png");
    
    return new_turret;
}

void remove_turret(turret *turret){
    free(turret);
}

projectile *init_projectile(int projectileID, turret* shooter){
    projectile *new_projectile = malloc(sizeof(projectile));
    node *projectileNode = malloc(sizeof(node));
    
    new_projectile.node = projectileNode;
    new_projectile.speed = 4;
    new_projectile.damage = 2;
    
    new_projectile.node.xPos = shooter.xPos;
    new_projectile.node.yPos = shooter.yPos;
    new_projectile.node.sprite = IMG_Load("Projectile.png");
    
    return new_projectile;
}

void remove_projectile(projectile *proj){
    free(proj);
}



