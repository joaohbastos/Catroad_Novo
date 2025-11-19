#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

/* Ativa código de carregamento/descarregamento de sprite do jogador */
#define PLAYER_HAS_SPRITE 1

typedef struct {
    Rectangle box;
    int linha;
    int maxlinha;
    int ponto;
    float moveCd;
#ifdef PLAYER_HAS_SPRITE
    Texture2D sprite;
#endif
} Jogador;

/* protótipos */
void Player_Init(Jogador *p, Vector2 startPos, float size);
void Player_Update(Jogador *p, float dt, float tile, int screenW, int screenH);
void personagem(const Jogador *p, Vector2 cameraOffset);

#endif /* PLAYER_H */
