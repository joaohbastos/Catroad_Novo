#include "world.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

void criarmundo(World *mundo, int screenWidth, int screenHeight, float tileSize) {
    mundo->tileSize = tileSize;
    mundo->quantidade_linha = 0;
    
    // Criar ruas e grama alternadas
    for (int i = 0; i < 300; i++) {
        if (mundo->quantidade_linha >= MAX_LANES) break;
        
        Lane rua;
        rua.rect = (Rectangle){0, screenHeight - (i + 1) * tileSize, screenWidth, tileSize};
        
        // Padrão: Grama, Estrada, Grama, Estrada, etc.
        if (i % 2 == 0) {
            // LANE DE ESTRADA
            rua.type = estrada; // CORRIGIDO: Era LANE_ROAD
            rua.cor = (Color){60, 60, 60, 255};  // Cinza escuro
            
            // 🔥 ALEATORIEDADE: 70% de chance de ter carro
            rua.hasCar = (GetRandomValue(0, 100) < 70);

            if (rua.hasCar) {
                // 🔥 ALEATORIEDADE: Direção aleatória
                if (GetRandomValue(0, 1) == 0) {
                    rua.carX = -tileSize * GetRandomValue(2, 4);
                    rua.velocidade = GetRandomValue(120, 220);
                } else {
                    rua.carX = screenWidth + tileSize * GetRandomValue(2, 4);
                    rua.velocidade = -GetRandomValue(120, 220);
                }
                
                // 🔥 ALEATORIEDADE: Quantidade de carros (1-3 carros por lane)
                rua.carCount = GetRandomValue(1, 3);
                rua.carSpacing = GetRandomValue(200, 400);
            } else {
                rua.carCount = 0;
                rua.velocidade = 0;
                rua.carX = 0;
                rua.carSpacing = 0;
            }
        } else {
            rua.type = pavimento; // CORRIGIDO: Era rua.tipo
            rua.cor = (Color){130, 130, 130, 255};
            rua.hasCar = false;
            rua.carCount = 0;
            rua.velocidade = 0;
            rua.carX = 0;
            rua.carSpacing = 0;
        }

        mundo->ruas[mundo->quantidade_linha] = rua; // CORRIGIDO: Era world->linhas
        mundo->quantidade_linha++;
    }

    printf("Mundo criado com %d ruas (de 0 a %d)\n", mundo->quantidade_linha, mundo->quantidade_linha - 1);
}

void World_Update(World *mundo, float dt, int screenWidth) {
    for (int i = 0; i < mundo->quantidade_linha; i++) {
        Lane *rua = &mundo->ruas[i];
        
        if (rua->type == estrada && rua->hasCar) { // CORRIGIDO: Era LANE_ROAD
            // Atualizar posição base do comboio de carros
            rua->carX += rua->velocidade * dt;

            // Tamanho aproximado do comboio (espaçamento entre carros + largura do primeiro)
            float comboioSize = 0.0f;
            if (rua->carCount > 0) {
                comboioSize = (rua->carCount - 1) * rua->carSpacing + mundo->tileSize * 2.0f;
            }

            // Fazer o comboio reaparecer do outro lado da tela
            if (rua->velocidade > 0 && rua->carX > screenWidth + comboioSize) {
                rua->carX = -comboioSize;
            } else if (rua->velocidade < 0 && rua->carX < -comboioSize) {
                rua->carX = screenWidth + comboioSize;
            }
        }
    }
}

void World_Draw(const World *mundo, Vector2 cameraOffset) {
    int screenWidth = 800; // Você pode querer passar isso como parâmetro

    for (int i = 0; i < mundo->quantidade_linha; i++) {
        const Lane *rua = &mundo->ruas[i];

        // Aplicar offset da câmera
        Rectangle drawRect = rua->rect;
        drawRect.y -= cameraOffset.y;
        
        // Só desenhar se estiver visível na tela
        if (drawRect.y + drawRect.height < 0) continue;
        if (drawRect.y > 600) continue; // Assumindo altura de 600
        
        // Desenhar a lane base
        DrawRectangleRec(drawRect, rua->cor); // CORRIGIDO: Era rua->color
        
        if (rua->type == estrada) { // CORRIGIDO: Era LANE_ROAD
            // Linhas divisórias da estrada
            for (int x = 40; x < screenWidth; x += 80) {
                DrawRectangle(x, drawRect.y + drawRect.height/2 - 1, 30, 2, YELLOW);
            }
            
            // 🔥 DESENHAR MÚLTIPLOS CARROS
            if (rua->hasCar && rua->carCount > 0) {
                for (int carIndex = 0; carIndex < rua->carCount; carIndex++) {
                    float carOffset = carIndex * rua->carSpacing;
                    float currentCarX = rua->carX + carOffset;

                    // Só desenhar se estiver visível na tela
                    if (currentCarX > -mundo->tileSize * 3 && currentCarX < screenWidth + mundo->tileSize * 3){
                        Rectangle carRect = {
                            currentCarX,
                            drawRect.y + 2,
                            mundo->tileSize * 1.8f,
                            mundo->tileSize - 4
                        };
                        
                        // 🔥 CORES ALEATÓRIAS PARA CARROS
                        Color carColors[] = {RED, BLUE, GREEN, PURPLE, ORANGE, DARKGRAY};
                        Color carColor = carColors[i % 6];
                        
                        DrawRectangleRec(carRect, carColor);
                        // Detalhes do carro
                        DrawRectangle(carRect.x + 5, carRect.y + 5, carRect.width - 10, 8, (Color){200, 200, 200, 255});
                    }
                }
            }
        } else {
            // 🔥 APENAS graminha simples (opcional)
            for (int x = 30; x < screenWidth; x += 90) {
                if (GetRandomValue(0, 100) > 60) {
                    DrawRectangle(x, drawRect.y + drawRect.height - 6, 2, 6, (Color){40, 120, 40, 255});
                }
            }
        }
    }
}

// CORRIGIDO: Nome da função e lógica de colisão
bool World_CheckCollision(const World *mundo, Rectangle playerRect) {
    for (int i = 0; i < mundo->quantidade_linha; i++) {
        const Lane *rua = &mundo->ruas[i];

        // Verificar se o player está nesta lane
        if (CheckCollisionRecs(playerRect, rua->rect)) {
            if (rua->type == estrada && rua->hasCar && rua->carCount > 0) { // CORRIGIDO: Era LANE_ROAD
                // 🔥 VERIFICAR COLISÃO COM TODOS OS CARROS
                for (int carIndex = 0; carIndex < rua->carCount; carIndex++) {
                    float carOffset = carIndex * rua->carSpacing;
                    float currentCarX = rua->carX + carOffset;

                    Rectangle carRect = {
                        currentCarX,
                        rua->rect.y + 2,
                        mundo->tileSize * 1.8f,
                        mundo->tileSize - 4
                    };

                    // CORRIGIDO: Checar colisão entre player e o carro
                    if (CheckCollisionRecs(playerRect, carRect)) {
                        printf("💥 Colisão na lane %d\n", i);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
