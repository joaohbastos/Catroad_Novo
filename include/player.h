#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#define PLAYER_HAS_SPRITE 1

typedef struct {
    Rectangle box;
    int linha;
#ifdef PLAYER_HAS_SPRITE
    Texture2D sprite;
#endif
} Jogador;

void Player_Init(Jogador *p, Vector2 startPos, float size);
void Player_Update(Jogador *p, float tile, int screenW, int screenH);
void personagem(const Jogador *p, Vector2 cameraOffset);

#endif