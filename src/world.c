#include "world.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

void criarmundo(Mundo *mundo, int largura, int altura, float tamanho) {

    mundo->tamanho = tamanho;
    mundo->quantidadelinhas = 0;
    mundo->dificuldadeatual = 1.0f;

    for (int i = 0; i < MAXFAIXAS; i++) {
        Rua rua;
        rua.rect = (Rectangle){ 0.0f, (float)altura - (i + 1) * tamanho, (float)largura, tamanho };
        rua.cor = (Color){ 0, 0, 0, 255 };
        rua.temcarro = false;
        rua.contagemcarros = 0;
        rua.posicaocarroX = 0.0f;
        rua.velocidade = 0.0f;
        rua.espacoentrecarros = 0.0f;

        if (i % 2 == 0) {
            rua.tipo = estrada;
            rua.cor = (Color){ 60, 60, 60, 255 };

            /* 70% de chance de ter carros nesta faixa */
            rua.temcarro = (GetRandomValue(0, 99) < 70);

            if (rua.temcarro) {
                if (GetRandomValue(0, 1) == 0) {
                    rua.posicaocarroX = -tamanho * (float)GetRandomValue(2, 4);
                    rua.velocidade = (float)GetRandomValue(120, 220);
                } else {
                    rua.posicaocarroX = (float)largura + tamanho * (float)GetRandomValue(2, 4);
                    rua.velocidade = - (float)GetRandomValue(120, 220);
                }

                rua.contagemcarros = GetRandomValue(1, 3);

                rua.espacoentrecarros = (float)GetRandomValue(200, 400);
                if (rua.espacoentrecarros < mundo->tamanho * 1.5f) {
                    rua.espacoentrecarros = mundo->tamanho * 1.5f;
                }
            } else {
                rua.contagemcarros = 0;
                rua.velocidade = 0.0f;
                rua.posicaocarroX = 0.0f;
                rua.espacoentrecarros = 0.0f;
            }
        } else {
            rua.tipo = calcada;
            rua.cor = (Color){ 130, 130, 130, 255 };
            rua.temcarro = false;
            rua.contagemcarros = 0;
            rua.velocidade = 0.0f;
            rua.posicaocarroX = 0.0f;
            rua.espacoentrecarros = 0.0f;
        }

        mundo->faixas[mundo->quantidadelinhas] = rua;
        mundo->quantidadelinhas++;
    }

    printf("Mundo criado com %d linhas (tile %.1f)\n", mundo->quantidadelinhas, mundo->tamanho);
}

void atualizar_mundo(Mundo *mundo, float dt, int largura, float dificuldade) {
    mundo->dificuldadeatual = dificuldade;

    for (int i = 0; i < mundo->quantidadelinhas; i++) {
        Rua *rua = &mundo->faixas[i];

        if (rua->tipo == estrada && rua->temcarro && rua->contagemcarros > 0) {
            float velocidadeAtual = rua->velocidade * dificuldade;

            float espacamentoEfetivo = rua->espacoentrecarros;
            if (espacamentoEfetivo <= 0.0f) espacamentoEfetivo = mundo->tamanho * 1.2f;

            if (dificuldade > 1.0f) {
                espacamentoEfetivo = espacamentoEfetivo / dificuldade;
                if (espacamentoEfetivo < mundo->tamanho * 1.2f) {
                    espacamentoEfetivo = mundo->tamanho * 1.2f;
                }
            }

            /* atualiza posição base do comboio */
            rua->posicaocarroX += velocidadeAtual * dt;

            /* calcula tamanho do comboio para wrap-around */
            float comboioSize = 0.0f;
            if (rua->contagemcarros > 0) {
                comboioSize = (rua->contagemcarros - 1) * espacamentoEfetivo + mundo->tamanho * 2.0f;
            }

            /* wrap-around: reaparecer do outro lado */
            if (rua->velocidade > 0.0f && rua->posicaocarroX > (float)largura + comboioSize) {
                rua->posicaocarroX = -comboioSize;
            } else if (rua->velocidade < 0.0f && rua->posicaocarroX < -comboioSize) {
                rua->posicaocarroX = (float)largura + comboioSize;
            }
        }
    }
}

void planodefundo(const Mundo *mundo, Vector2 deslocamentocamera) {

    const int screenWidth = 1200;
    const int screenHeight = 720;

    for (int i = 0; i < mundo->quantidadelinhas; i++) {
        const Rua *rua = &mundo->faixas[i];

        Rectangle drawRect = rua->rect;
        drawRect.y -= deslocamentocamera.y;

        /* desenha apenas se estiver dentro da área visível */
        if (drawRect.y + drawRect.height < 0) continue;
        if (drawRect.y > screenHeight) continue;

        /* base da faixa */
        DrawRectangleRec(drawRect, rua->cor);

        if (rua->tipo == estrada) {
            /* linhas divisórias */
            for (int x = 40; x < screenWidth; x += 80) {
                DrawRectangle(x, drawRect.y + drawRect.height / 2 - 1, 30, 2, YELLOW);
            }

            /* desenhar carros (retângulos) */
            if (rua->temcarro && rua->contagemcarros > 0) {
                float espacamentoEfetivo = rua->espacoentrecarros;
                if (espacamentoEfetivo <= 0.0f) espacamentoEfetivo = mundo->tamanho * 1.2f;
                if (mundo->dificuldadeatual > 1.0f) {
                    espacamentoEfetivo = espacamentoEfetivo / mundo->dificuldadeatual;
                    if (espacamentoEfetivo < mundo->tamanho * 1.2f) espacamentoEfetivo = mundo->tamanho * 1.2f;
                }

                for (int carIndex = 0; carIndex < rua->contagemcarros; carIndex++) {
                    float carOffset = carIndex * espacamentoEfetivo;
                    float currentCarX = rua->posicaocarroX + carOffset;

                    if (currentCarX > -mundo->tamanho * 3 && currentCarX < screenWidth + mundo->tamanho * 3) {
                        Rectangle carRect = {
                            currentCarX,
                            drawRect.y + 2,
                            mundo->tamanho * 1.8f,
                            mundo->tamanho - 4
                        };

                        Color carColors[] = { RED, BLUE, GREEN, PURPLE, ORANGE, DARKGRAY };
                        Color carColor = carColors[i % (sizeof(carColors) / sizeof(carColors[0]))];

                        DrawRectangleRec(carRect, carColor);
                        DrawRectangle(carRect.x + 5, carRect.y + 5, carRect.width - 10, 8,
                                      (Color){ 200, 200, 200, 255 });
                    }
                }
            }
        } else if (rua->tipo == calcada) {
            for (int x = 30; x < screenWidth; x += 90) {
                int seed = (i * 37 + x) % 100;
                if (seed > 60) {
                    DrawRectangle(x, drawRect.y + drawRect.height - 6, 2, 6, (Color){ 40, 120, 40, 255 });
                }
            }
        }
    }
}

bool checarcolisao(const Mundo *mundo, Rectangle retanguloJogador) {

    for (int i = 0; i < mundo->quantidadelinhas; i++) {
        const Rua *rua = &mundo->faixas[i];
        if (!CheckCollisionRecs(retanguloJogador, rua->rect)) continue;

        if (rua->tipo == estrada && rua->temcarro && rua->contagemcarros > 0) {
            float espacamentoEfetivo = rua->espacoentrecarros;
            if (espacamentoEfetivo <= 0.0f) espacamentoEfetivo = mundo->tamanho * 1.2f;
            if (mundo->dificuldadeatual > 1.0f) {
                espacamentoEfetivo = espacamentoEfetivo / mundo->dificuldadeatual;
                if (espacamentoEfetivo < mundo->tamanho * 1.2f) espacamentoEfetivo = mundo->tamanho * 1.2f;
            }

            for (int carIndex = 0; carIndex < rua->contagemcarros; carIndex++) {
                float carOffset = carIndex * espacamentoEfetivo;
                float currentCarX = rua->posicaocarroX + carOffset;

                Rectangle carRect = {
                    currentCarX,
                    rua->rect.y + 2,
                    mundo->tamanho * 1.8f,
                    mundo->tamanho - 4
                };

                if (CheckCollisionRecs(retanguloJogador, carRect)) {
                    return true;
                }
            }
        } 
    }
    return false;
}