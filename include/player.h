#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct jogador {
    Rectangle box;
    int linha;
    int maxLinha;
    int ponto;
    float moveCd;
} Jogador;

// MUDANÇAS AQUI: As funções agora usam "Jogador"
void Player_Init(Jogador *p, Vector2 startPos, float size);
void Player_Update(Jogador *p, float dt, float tile, int screenW, int screenH);
void Player_Draw(const Jogador *p, Vector2 cameraOffset);

#endif