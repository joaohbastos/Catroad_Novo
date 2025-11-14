#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"

#define MAXFAIXAS 350

typedef enum {
    calcada,
    estrada,
    rio
} TipoFaixa;

typedef struct {
    TipoFaixa tipo;
    Rectangle rect;
    bool temcarro;
    int contagemcarros;
    float posicaocarroX;
    float velocidade;
    float espacoentrecarros;
    Color cor;
} Rua;

typedef struct {
    Rua faixas[MAXFAIXAS];
    int quantidadelinhas;
    float tamanho;
    float dificuldadeatual;
} Mundo;

void criarmundo(Mundo *mundo, int largura, int altura, float tamanho);

void World_Update(Mundo *mundo, float dt, int largura, float dificuldade);
void planodefundo(const Mundo *mundo, Vector2 deslocamentocamera);
bool checarcolisao(const Mundo *mundo, Rectangle retanguloJogador);

#endif