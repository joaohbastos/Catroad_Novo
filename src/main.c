#include "game.h"

int main(void) {
    iniciarjogo();
    while (!fechar_jogo()) { 
        atualizarjogo();
        desenharcenario();
    }
    parar_de_rodar(); 
    return 0;
}