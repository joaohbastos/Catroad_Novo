// src/world.c
#include "world.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

/* Observação:
   Este arquivo usa os valores de tela 800x600 para desenhar e checagens rápidas.
   Se você mudar largura/altura em game.c, considere passar esses valores como parâmetros. */

void criarmundo(Mundo *mundo, int largura, int altura, float tamanho) {
    if (!mundo) return;

    mundo->tamanho = tamanho;
    mundo->quantidadelinhas = 0;
    mundo->dificuldadeatual = 1.0f;

    /* Gerar até MAXFAIXAS linhas (ou até preencher) */
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
            /* estrada */
            rua.tipo = estrada;
            rua.cor = (Color){ 60, 60, 60, 255 };

            /* 70% de chance de ter carros nesta faixa */
            rua.temcarro = (GetRandomValue(0, 99) < 70);

            if (rua.temcarro) {
                /* direção aleatória e posição inicial fora da tela */
                if (GetRandomValue(0, 1) == 0) {
                    rua.posicaocarroX = -tamanho * (float)GetRandomValue(2, 4);
                    rua.velocidade = (float)GetRandomValue(120, 220);
                } else {
                    rua.posicaocarroX = (float)largura + tamanho * (float)GetRandomValue(2, 4);
                    rua.velocidade = - (float)GetRandomValue(120, 220);
                }

                rua.contagemcarros = GetRandomValue(1, 3);

                /* espaco entre carros como float */
                rua.espacoentrecarros = (float)GetRandomValue(200, 400);
                /* garante espaçamento mínimo razoável */
                if (rua.espacoentrecarros < mundo->tamanho * 1.2f) {
                    rua.espacoentrecarros = mundo->tamanho * 1.2f;
                }
            } else {
                /* sem carros */
                rua.contagemcarros = 0;
                rua.velocidade = 0.0f;
                rua.posicaocarroX = 0.0f;
                rua.espacoentrecarros = 0.0f;
            }
        } else {
            /* calçada */
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

        /* Proteção: se a altura total gerada ficar absurda, sai do loop */
        if ((i + 1) * tamanho > altura * 1000) break;
    }

    printf("Mundo criado com %d linhas (tile %.1f)\n", mundo->quantidadelinhas, mundo->tamanho);
}

void World_Update(Mundo *mundo, float dt, int largura, float dificuldade) {
    if (!mundo) return;

    mundo->dificuldadeatual = dificuldade;

    for (int i = 0; i < mundo->quantidadelinhas; i++) {
        Rua *rua = &mundo->faixas[i];

        if (rua->tipo == estrada && rua->temcarro && rua->contagemcarros > 0) {
            /* velocidade escala com dificuldade */
            float velocidadeAtual = rua->velocidade * dificuldade;

            /* espaçamento efetivo com proteções */
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
    if (!mundo) return;

    const int screenWidth = 800;
    const int screenHeight = 600;

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
            /* grama / calçada — decoração determinística sem RNG por frame */
            for (int x = 30; x < screenWidth; x += 90) {
                int seed = (i * 37 + x) % 100;
                if (seed > 60) {
                    DrawRectangle(x, drawRect.y + drawRect.height - 6, 2, 6, (Color){ 40, 120, 40, 255 });
                }
            }
        } else if (rua->tipo == rio) {
            /* exemplo simples para 'rio' — desenha azul e pequenas ondas */
            for (int x = 0; x < screenWidth; x += 20) {
                DrawRectangleRec((Rectangle){ (float)x, drawRect.y, 18, drawRect.height }, (Color){ 30, 144, 255, 255 });
            }
            /* ondas simples (determinísticas) */
            for (int x = 0; x < screenWidth; x += 40) {
                DrawCircle((float)x + 10.0f, drawRect.y + drawRect.height / 2.0f, 3.0f, (Color){ 180, 220, 255, 200 });
            }
        }
    }
}

bool checarcolisao(const Mundo *mundo, Rectangle retanguloJogador) {
    if (!mundo) return false;

    for (int i = 0; i < mundo->quantidadelinhas; i++) {
        const Rua *rua = &mundo->faixas[i];

        /* primeiro filtro: overlap rápido entre player e a faixa */
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
        } else if (rua->tipo == rio) {
            /* exemplo: colisão com rio sempre considera perda */
            /* se o jogador está sobre o rio, retornar true (pode adaptar) */
            if (CheckCollisionRecs(retanguloJogador, rua->rect)) {
                /* dependendo da mecânica, você pode querer tratar rio diferente */
                return true;
            }
        }
    }

    return false;
}
