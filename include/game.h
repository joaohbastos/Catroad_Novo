#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "raylib.h" 

void iniciarjogo(void);
void criarmundo(void);
void atualizarjogo(void);
void desenharcenario(void);
void parar_de_rodar(void);
bool fechar_jogo(void);

#endif