//
//  Estruturas.h
//
//  Created by Gabriel Nopper on 29/08/15.
//  Copyright (c) 2015 Gabriel Nopper. All rights reserved.
//

#ifndef __Tower_Defense__Estruturas__
#define __Tower_Defense__Estruturas__

#include <stdio.h>
#include <SDL2/SDL.h>



typedef struct node
{
    double xPos;
    double yPos;
    SDL_Surface sprite;
} node;

typedef struct minion{
    int HP; // Não é necessário um maxHP, considerando a inexistencia de cura.
    float speed; // Movimento por ciclo.
    node node;
};

typedef struct turret{
    float timeUntilNextAttack; // Tempo de espera até atirar novamente.
    float turretType; // isto define o tipo de tiro. Alguma função usará este valor para escolher qual tiro é gerado.
    float radius; // alcance da torre.
    node node;
};

typedef struct projectile{
    minion target; // Para onde o tiro atualmente está indo.
    node node;
};


// É importante notar que não temos como nos assegurar de que o monstro existira para o tiro. Eventualmente teremos de, ou fazer o tiro mudar de alvo se o monstro morrer, ou fazer o monstro esperar até não ter tiros mirando nele.

// Favor importar as libs de SDL. Não tenho os arquivos da aula e não sei o que será necessário importar.



#endif /* defined(__Tower_Defense__Estruturas__) */
