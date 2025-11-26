#ifndef PLAYER_H
#define PLAYER_H
#include <stdbool.h>
#include "raylib.h"
#define GATO 1

typedef struct {
    Rectangle box;
    int linha;
#ifdef GATO
    Texture2D sprite;
#endif
    int **matriz;
    int linhas;
    int colunas;
} Jogador;

Jogador *criargato(Vector2 startPos, float size);

void movimentacao(Jogador *p, float tile, int largura, int altura);
void personagem(const Jogador *p, Vector2 cameraOffset);
void salvar_recorde(int linha);
int carregar_recorde(void);

#endif