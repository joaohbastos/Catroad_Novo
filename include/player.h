#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

#define PLAYER_HAS_SPRITE 1

typedef struct {
    Rectangle box;
    int linha;
#ifdef PLAYER_HAS_SPRITE
    Texture2D sprite;
#endif

    int **matrizAnim;
    int linhasAnim;
    int colunasAnim;
} Jogador;

Jogador *Player_Create(Vector2 startPos, float size);

void Player_Destroy(Jogador *p);
void Player_Update(Jogador *p, float tile, int screenW, int screenH);
void personagem(const Jogador *p, Vector2 cameraOffset);
void Player_SaveHighscore(int linha);
int Player_LoadHighscore(void);

#endif
