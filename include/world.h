#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"

#define MAXFAIXAS 350

typedef struct Carro {
    int indice;
    struct Carro *proximo;
} Carro;

typedef enum { estrada, calcada } TipoRua;

typedef struct {
    Rectangle rect;
    Color cor;
    TipoRua tipo;
    bool temcarro;
    float posicaocarroX;
    float velocidade;
    int contagemcarros;
    float espacoentrecarros;
    Carro *listaCarros;
} Rua;

typedef struct Mundo {
    Rua *faixas;
    int **mapa;
    int quantidadelinhas;
    int capacidade;
    int colunasmatriz;
    float tamanho;
    float dificuldadeatual;
} Mundo;

void liberarmundo(Mundo *m);
void criarmundo(Mundo *m, int largura, int altura, float tamanho);
void atualizar_mundo(Mundo *m, float frametime, int largura, float dificuldade);
void planodefundo(const Mundo *m, Vector2 cam);
bool checarcolisao(const Mundo *m, Rectangle player);

#endif