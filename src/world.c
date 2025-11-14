#include "world.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

void criarmundo(Mundo *mundo, int largura, int altura, float tamanho) {
    mundo->tamanho = tamanho;
    mundo->quantidadelinhas = 0;
    mundo->dificuldadeatual = 1.0f; // inicial
    
    // Criar ruas e grama alternadas
    for (int i = 0; i < 300; i++) {
        if (mundo->quantidadelinhas >= MAXFAIXAS) break;
        
        Rua rua;
        rua.rect = (Rectangle){0, altura - (i + 1) * tamanho, largura, tamanho};
        if (i % 2 == 0) {
            rua.tipo = estrada;
            rua.cor = (Color){60, 60, 60, 255};  // Cinza escuro
            
            // 70% de chance de ter carro
            rua.temcarro = (GetRandomValue(0, 100) < 70);

            if (rua.temcarro) {
                // Direção aleatória
                if (GetRandomValue(0, 1) == 0) {
                    rua.posicaocarroX = -tamanho * GetRandomValue(2, 4);
                    rua.velocidade = GetRandomValue(120, 220);
                } else {
                    rua.posicaocarroX = largura + tamanho * GetRandomValue(2, 4);
                    rua.velocidade = -GetRandomValue(120, 220);
                }
                
                // Quantidade de carros (1-3 carros por lane)
                rua.contagemcarros  = GetRandomValue(1, 3);
                rua.espacoentrecarros = GetRandomValue(200, 400);
            } else {
                rua.contagemcarros  = 0;
                rua.velocidade = 0;
                rua.posicaocarroX       = 0;
                rua.espacoentrecarros = 0;
            }
        } else {
            rua.tipo = calcada;
            rua.cor = (Color){130, 130, 130, 255};
            rua.temcarro    = false;
            rua.contagemcarros  = 0;
            rua.velocidade = 0;
            rua.posicaocarroX       = 0;
            rua.espacoentrecarros = 0;
        }

        mundo->faixas[mundo->quantidadelinhas] = rua;
        mundo->quantidadelinhas++;
    }

    printf("Mundo criado com %d ruas (de 0 a %d)\n", mundo->quantidadelinhas, mundo->quantidadelinhas - 1);
}

// agora recebe dificuldade
void World_Update(Mundo *mundo, float dt, int largura, float dificuldade) {
    // guardar dificuldade dentro do mundo para o Draw e a colisão usarem
    mundo->dificuldadeatual = dificuldade;

    for (int i = 0; i < mundo->quantidadelinhas; i++) {
        Rua *rua = &mundo->faixas[i];
        
        if (rua->tipo == estrada && rua->temcarro) {
            // velocidade escalar com a dificuldade
            float velocidadeAtual = rua->velocidade * dificuldade;

            // espaçamento efetivo (maior dificuldade => carros mais próximos)
            float espacamentoEfetivo = rua->espacoentrecarros;
            if (dificuldade > 1.0f) {
                espacamentoEfetivo = rua->espacoentrecarros / dificuldade;
                if (espacamentoEfetivo < mundo->tamanho * 1.2f) {
                    // não deixar ficar colado demais
                    espacamentoEfetivo = mundo->tamanho * 1.2f;
                }
            }

            // Atualizar posição base do comboio de carros
            rua->posicaocarroX += velocidadeAtual * dt;

            // Tamanho aproximado do comboio
            float comboioSize = 0.0f;
            if (rua->contagemcarros > 0) {
                comboioSize = (rua->contagemcarros - 1) * espacamentoEfetivo + mundo->tamanho * 2.0f;
            }

            // Fazer o comboio reaparecer do outro lado da tela
            if (rua->velocidade > 0 && rua->posicaocarroX > largura + comboioSize) {
                rua->posicaocarroX = -comboioSize;
            } else if (rua->velocidade < 0 && rua->posicaocarroX < -comboioSize) {
                rua->posicaocarroX = largura + comboioSize;
            }
        }
    }
}

void planodefundo(const Mundo *mundo, Vector2 deslocamentocamera) {
    int screenWidth = 800; // Você pode querer passar isso como parâmetro

    for (int i = 0; i < mundo->quantidadelinhas; i++) {
        const Rua *rua = &mundo->faixas[i];

        // Aplicar offset da câmera
        Rectangle drawRect = rua->rect;
        drawRect.y -= deslocamentocamera.y;
        
        // Só desenhar se estiver visível na tela
        if (drawRect.y + drawRect.height < 0) continue;
        if (drawRect.y > 600) continue; // Assumindo altura de 600
        
        // Desenhar a lane base
        DrawRectangleRec(drawRect, rua->cor);
        
        if (rua->tipo == estrada) {
            // Linhas divisórias da estrada
            for (int x = 40; x < screenWidth; x += 80) {
                DrawRectangle(x, drawRect.y + drawRect.height/2 - 1, 30, 2, YELLOW);
            }
            
            // DESENHAR MÚLTIPLOS CARROS
            if (rua->temcarro && rua->contagemcarros > 0) {

                // usar o mesmo conceito de "espacamentoEfetivo" aqui
                float espacamentoEfetivo = rua->espacoentrecarros;
                if (mundo->dificuldadeatual > 1.0f) {
                    espacamentoEfetivo = rua->espacoentrecarros / mundo->dificuldadeatual;
                    if (espacamentoEfetivo < mundo->tamanho * 1.2f) {
                        espacamentoEfetivo = mundo->tamanho * 1.2f;
                    }
                }

                for (int carIndex = 0; carIndex < rua->contagemcarros; carIndex++) {
                    float carOffset = carIndex * espacamentoEfetivo;
                    float currentCarX = rua->posicaocarroX + carOffset;

                    // Só desenhar se estiver visível na tela
                    if (currentCarX > -mundo->tamanho * 3 && currentCarX < screenWidth + mundo->tamanho * 3){
                        Rectangle carRect = {
                            currentCarX,
                            drawRect.y + 2,
                            mundo->tamanho * 1.8f,
                            mundo->tamanho - 4
                        };
                        
                        Color carColors[] = {RED, BLUE, GREEN, PURPLE, ORANGE, DARKGRAY};
                        Color carColor = carColors[i % 6];
                        
                        DrawRectangleRec(carRect, carColor);
                        DrawRectangle(carRect.x + 5, carRect.y + 5, carRect.width - 10, 8,
                                      (Color){200, 200, 200, 255});
                    }
                }
            }
        } else {
            for (int x = 30; x < screenWidth; x += 90) {
                if (GetRandomValue(0, 100) > 60) {
                    DrawRectangle(x, drawRect.y + drawRect.height - 6, 2, 6, (Color){40, 120, 40, 255});
                }
            }
        }
    }
}

bool checarcolisao(const Mundo *mundo, Rectangle playerRect) {
    for (int i = 0; i < mundo->quantidadelinhas; i++) {
        const Rua *rua = &mundo->faixas[i];

        //comentar algo
        if (CheckCollisionRecs(playerRect, rua->rect)) {
            if (rua->tipo == estrada && rua->temcarro && rua->contagemcarros > 0) {
                float espacamentoEfetivo = rua->espacoentrecarros;
                if (mundo->dificuldadeatual > 1.0f) {
                    espacamentoEfetivo = rua->espacoentrecarros / mundo->dificuldadeatual;
                    if (espacamentoEfetivo < mundo->tamanho * 1.2f) {
                        espacamentoEfetivo = mundo->tamanho * 1.2f;
                    }
                }

                //verificar colisao com todos os carros
                for (int carIndex = 0; carIndex < rua->contagemcarros; carIndex++) {
                    float carOffset = carIndex * espacamentoEfetivo;
                    float currentCarX = rua->posicaocarroX + carOffset;

                    Rectangle carRect = {
                        currentCarX,
                        rua->rect.y + 2,
                        mundo->tamanho * 1.8f,
                        mundo->tamanho - 4
                    };

                    if (CheckCollisionRecs(playerRect, carRect)) {
                        printf("💥 Colisão na rua %d\n", i);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}