#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"

#define MAXFAIXAS 350

typedef struct Carro {
    int indice;
    struct Carro *proximo; 
} Carro;

typedef enum {
    calcada,
    estrada
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

    Carro *listaCarros;
} Rua;

typedef struct {
    Rua *faixas;

    int quantidadelinhas;
    int capacidade;  

    float tamanho;
    float dificuldadeatual;

    int **mapaTerreno;
    int colunasMatriz;
} Mundo;

void criarmundo(Mundo *mundo, int largura, int altura, float tamanho);
void atualizar_mundo(Mundo *mundo, float dt, int largura, float dificuldade);
void planodefundo(const Mundo *mundo, Vector2 deslocamentocamera);
bool checarcolisao(const Mundo *mundo, Rectangle retanguloJogador);
void liberarmundo(Mundo *mundo);
#endif
