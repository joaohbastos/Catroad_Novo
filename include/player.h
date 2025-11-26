#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

struct Mundo;
typedef struct {
    Rectangle box;
    int linha;
    Texture2D sprite;
    int linhas;
    int colunas;
    int **matriz;
} Jogador;

Jogador *criargato(Vector2 startPos, float size);
void movimentacao(Jogador *p, float tile, int largura, int altura);
void personagem(const Jogador *p, Vector2 cameraOffset);
void salvar_recorde(int linha);
int carregar_recorde(void);
void atualizar_radar(Jogador *p, struct Mundo *m, float tile_size);

#endif