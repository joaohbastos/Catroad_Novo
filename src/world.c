#include "world.h"
#include "raylib.h"
#include <stdio.h>

void World_Init(World *world, int screenWidth, int screenHeight, float tileSize) {
    world->tileSize = tileSize;
    world->laneCount = 0;
    
    printf("🌍 Criando 300 lanes...\n");
    
    // 🔥 CRIAR 300 LANES
    for (int i = 0; i < 300; i++) {
        if (world->laneCount >= MAX_LANES) break;
        
        Lane lane;
        lane.rect = (Rectangle){0, screenHeight - (i + 1) * tileSize, screenWidth, tileSize};
        
        // Padrão: Grama, Estrada, Grama, Estrada, etc.
        if (i % 2 == 0) {
            // LANE DE ESTRADA
            lane.type = LANE_ROAD;
            lane.color = (Color){60, 60, 60, 255};  // Cinza escuro
            lane.hasCar = true;
            
            // Carros alternando direções
            if (i % 4 == 0) {
                lane.carX = -tileSize * 2;
                lane.carSpeed = 180.0f;
            } else {
                lane.carX = screenWidth + tileSize;
                lane.carSpeed = -160.0f;
            }
        } else {
            // LANE DE GRAMA
            lane.type = LANE_GRASS;
            lane.color = (Color){50, 168, 82, 255};  // Verde grama
            lane.hasCar = false;
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
            
            // 🔥 CORRIGIDO: usar world->tileSize em vez de tileSize
            if (lane->carSpeed > 0 && lane->carX > screenWidth + world->tileSize * 2) {
                lane->carX = -world->tileSize * 3;
            } else if (lane->carSpeed < 0 && lane->carX < -world->tileSize * 3) {
                lane->carX = screenWidth + world->tileSize * 2;
            }
        }
    }
}

void World_Draw(const World *world, Vector2 cameraOffset) {
    // 🔥 CORRIGIDO: usar SCREEN_W ou passar como parâmetro
    // Vamos usar 800 como padrão (igual ao game.c)
    int screenWidth = 800;
    
    for (int i = 0; i < world->laneCount; i++) {
        const Lane *lane = &world->lanes[i];
        
        // Aplicar offset da câmera
        Rectangle drawRect = lane->rect;
        drawRect.y -= cameraOffset.y;
        
        // Só desenhar se estiver visível na tela
        if (drawRect.y + drawRect.height < 0) continue;  // Muito acima
        if (drawRect.y > 600) continue;  // Muito abaixo
        
        // Desenhar a lane base
        DrawRectangleRec(drawRect, lane->color);
        
        if (lane->type == LANE_ROAD) {
            // Linhas divisórias da estrada
            for (int x = 40; x < screenWidth; x += 80) {
                DrawRectangle(x, drawRect.y + drawRect.height/2 - 1, 30, 2, YELLOW);
            }
            
            // Desenhar carro
            if (lane->hasCar) {
                Rectangle carRect = {
                    lane->carX,
                    drawRect.y + 2,
                    world->tileSize * 1.8f,
                    world->tileSize - 4
                };
                DrawRectangleRec(carRect, RED);
                // Detalhes do carro
                DrawRectangle(carRect.x + 5, carRect.y + 5, carRect.width - 10, 8, (Color){200, 200, 200, 255});
            }
        } else {
            // Detalhes da grama (flores/graminha)
            for (int x = 20; x < screenWidth; x += 60) {
                if (GetRandomValue(0, 100) > 70) {
                    DrawCircle(x, drawRect.y + drawRect.height/2, 2, (Color){255, 255, 100, 255});
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
            if (lane->type == LANE_ROAD && lane->hasCar) {
                Rectangle carRect = {
                    lane->carX,
                    lane->rect.y + 2,
                    world->tileSize * 1.8f,
                    world->tileSize - 4
                };
                
                if (CheckCollisionRecs(playerRect, carRect)) {
                    printf("💥 Colisão na lane %d!\n", i);
                    return true;
                }
            }
        }
    }
    return false;
}