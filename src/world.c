#include "world.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

void World_Init(World *world, int screenWidth, int screenHeight, float tileSize) {
    world->tileSize = tileSize;
    world->laneCount = 0;
    
    printf("🌍 Criando 300 lanes...\n");
    
    // 🔥 CRIAR 300 LANES COM ALEATORIEDADE
    for (int i = 0; i < 300; i++) {
        if (world->laneCount >= MAX_LANES) break;
        
        Lane lane;
        lane.rect = (Rectangle){0, screenHeight - (i + 1) * tileSize, screenWidth, tileSize};
        
        // Padrão: Grama, Estrada, Grama, Estrada, etc.
        if (i % 2 == 0) {
            // LANE DE ESTRADA
            lane.type = LANE_ROAD;
            lane.color = (Color){60, 60, 60, 255};  // Cinza escuro
            
            // 🔥 ALEATORIEDADE: 70% de chance de ter carro
            lane.hasCar = (GetRandomValue(0, 100) < 70);
            
            if (lane.hasCar) {
                // 🔥 ALEATORIEDADE: Direção aleatória
                if (GetRandomValue(0, 1) == 0) {
                    lane.carX = -tileSize * GetRandomValue(2, 4);
                    lane.carSpeed = GetRandomValue(120, 220);
                } else {
                    lane.carX = screenWidth + tileSize * GetRandomValue(2, 4);
                    lane.carSpeed = -GetRandomValue(120, 220);
                }
                
                // 🔥 ALEATORIEDADE: Quantidade de carros (1-3 carros por lane)
                lane.carCount = GetRandomValue(1, 3);
                lane.carSpacing = GetRandomValue(200, 400);
            } else {
                lane.carCount = 0;
                lane.carSpeed = 0;
                lane.carX = 0;
            }
        } else {
            // LANE DE GRAMA
            lane.type = LANE_GRASS;
            lane.color = (Color){50, 168, 82, 255};  // Verde grama
            lane.hasCar = false;
            lane.carCount = 0;
            lane.carSpeed = 0;
            lane.carX = 0;
        }
        
        world->lanes[world->laneCount] = lane;
        world->laneCount++;
    }
    
    printf("✅ Mundo criado com %d lanes (de 0 a %d)\n", world->laneCount, world->laneCount - 1);
}

void World_Update(World *world, float dt, int screenWidth) {
    for (int i = 0; i < world->laneCount; i++) {
        Lane *lane = &world->lanes[i];
        
        if (lane->type == LANE_ROAD && lane->hasCar) {
            // Atualizar posição do carro
            lane->carX += lane->carSpeed * dt;
            
            // Fazer o carro reaparecer do outro lado
            if (lane->carSpeed > 0 && lane->carX > screenWidth + world->tileSize * 4) {
                lane->carX = -world->tileSize * 4;
            } else if (lane->carSpeed < 0 && lane->carX < -world->tileSize * 4) {
                lane->carX = screenWidth + world->tileSize * 4;
            }
        }
    }
}

void World_Draw(const World *world, Vector2 cameraOffset) {
    int screenWidth = 800;
    
    for (int i = 0; i < world->laneCount; i++) {
        const Lane *lane = &world->lanes[i];
        
        // Aplicar offset da câmera
        Rectangle drawRect = lane->rect;
        drawRect.y -= cameraOffset.y;
        
        // Só desenhar se estiver visível na tela
        if (drawRect.y + drawRect.height < 0) continue;
        if (drawRect.y > 600) continue;
        
        // Desenhar a lane base
        DrawRectangleRec(drawRect, lane->color);
        
        if (lane->type == LANE_ROAD) {
            // Linhas divisórias da estrada
            for (int x = 40; x < screenWidth; x += 80) {
                DrawRectangle(x, drawRect.y + drawRect.height/2 - 1, 30, 2, YELLOW);
            }
            
            // 🔥 DESENHAR MÚLTIPLOS CARROS
            if (lane->hasCar && lane->carCount > 0) {
                for (int carIndex = 0; carIndex < lane->carCount; carIndex++) {
                    float carOffset = carIndex * lane->carSpacing;
                    float currentCarX = lane->carX + carOffset;
                    
                    // Ajustar para aparecer do outro lado se necessário
                    if (lane->carSpeed > 0 && currentCarX > screenWidth + world->tileSize * 4) {
                        currentCarX = lane->carX + carOffset - (screenWidth + world->tileSize * 8);
                    } else if (lane->carSpeed < 0 && currentCarX < -world->tileSize * 4) {
                        currentCarX = lane->carX + carOffset + (screenWidth + world->tileSize * 8);
                    }
                    
                    // Só desenhar se estiver visível na tela
                    if (currentCarX > -world->tileSize * 3 && currentCarX < screenWidth + world->tileSize * 3) {
                        Rectangle carRect = {
                            currentCarX,
                            drawRect.y + 2,
                            world->tileSize * 1.8f,
                            world->tileSize - 4
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
            // 🔥 REMOVIDO: pontos amarelos piscando
            // 🔥 APENAS graminha simples (opcional)
            for (int x = 30; x < screenWidth; x += 90) {
                if (GetRandomValue(0, 100) > 60) {
                    DrawRectangle(x, drawRect.y + drawRect.height - 6, 2, 6, (Color){40, 120, 40, 255});
                }
            }
        }
    }
}

bool World_CheckCollision(const World *world, Rectangle playerRect) {
    for (int i = 0; i < world->laneCount; i++) {
        const Lane *lane = &world->lanes[i];
        
        // Verificar se o player está nesta lane
        if (CheckCollisionRecs(playerRect, lane->rect)) {
            if (lane->type == LANE_ROAD && lane->hasCar && lane->carCount > 0) {
                // 🔥 VERIFICAR COLISÃO COM TODOS OS CARROS
                for (int carIndex = 0; carIndex < lane->carCount; carIndex++) {
                    float carOffset = carIndex * lane->carSpacing;
                    float currentCarX = lane->carX + carOffset;
                    
                    Rectangle carRect = {
                        currentCarX,
                        lane->rect.y + 2,
                        world->tileSize * 1.8f,
                        world->tileSize - 4
                    };
                    
                    if (CheckCollisionRecs(playerRect, carRect)) {
                        printf("💥 Colisão na lane %d com carro %d!\n", i, carIndex);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}