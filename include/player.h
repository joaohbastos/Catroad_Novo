#ifndef PLAYER_H
#define PLAYER_H
#include <stdbool.h>
#define GATO 1

typedef struct {
    Rectangle box;
    int linha;
#ifdef GATO
    Texture2D sprite;
#endif
    int **matrizAnim;
    int linhasAnim;
    int colunasAnim;
} Jogador;

Jogador *criargato(Vector2 startPos, float size);

void Player_Update(Jogador *p, float tile, int screenW, int screenH);
void personagem(const Jogador *p, Vector2 cameraOffset);
void salvar_recorde(int linha);
int carregar_recorde(void);

#endif