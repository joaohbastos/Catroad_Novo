#include "world.h"
#include "raylib.h"
#include <stdio.h>

void World_Init(World *world, int screenWidth, int screenHeight, float tileSize) {
    world->tileSize = tileSize;
    world->laneCount = 0;
    
    // 🔥 Criar MUITAS mais lanes (exemplo: 200)
    for (int i = 0; i < 200; i++) {
        Lane lane;
        lane.rect = (Rectangle){0, screenHeight - (i + 1) * tileSize, screenWidth, tileSize};
        
        // Alternar entre grama e estrada
        if (i % 3 == 0) {
            lane.type = LANE_ROAD;
            lane.color = (Color){80, 80, 80, 255};  // Cinza escuro (estrada)
            lane.hasCar = true;
            lane.carX = (i % 2 == 0) ? -tileSize : screenWidth;  // Carros vindo de lados diferentes
            lane.carSpeed = (i % 2 == 0) ? 150.0f : -120.0f;
        } else {
            lane.type = LANE_GRASS;
            lane.color = (Color){34, 139, 34, 255};  // Verde (grama)
            lane.hasCar = false;
        }
        
        world->lanes[world->laneCount] = lane;
        world->laneCount++;
        
        if (world->laneCount >= MAX_LANES) break;
    }
    
    printf("🌍 Mundo inicializado com %d lanes\n", world->laneCount);
}

void World_Update(World *world, float dt, int screenWidth) {
    for (int i = 0; i < world->laneCount; i++) {
        Lane *lane = &world->lanes[i];
        
        if (lane->type == LANE_ROAD && lane->hasCar) {
            // Atualizar posição do carro
            lane->carX += lane->carSpeed * dt;
            
            // Fazer o carro reaparecer do outro lado
            if (lane->carSpeed > 0 && lane->carX > screenWidth) {
                lane->carX = -world->tileSize * 2;
            } else if (lane->carSpeed < 0 && lane->carX < -world->tileSize * 2) {
                lane->carX = screenWidth;
            }
        }
    }
}

void World_Draw(const World *world, Vector2 cameraOffset) {
    for (int i = 0; i < world->laneCount; i++) {
        const Lane *lane = &world->lanes[i];
        
        // Aplicar offset da câmera
        Rectangle drawRect = lane->rect;
        drawRect.y -= cameraOffset.y;
        
        // Desenhar a lane
        DrawRectangleRec(drawRect, lane->color);
        
        // Desenhar marcações da estrada
        if (lane->type == LANE_ROAD) {
            for (int x = 20; x < drawRect.width; x += 80) {
                DrawRectangle(x, drawRect.y + drawRect.height/2 - 2, 40, 4, YELLOW);
            }
        }
        
        // Desenhar carro se existir
        if (lane->type == LANE_ROAD && lane->hasCar) {
            Rectangle carRect = {
                lane->carX,
                drawRect.y,
                world->tileSize * 2,
                world->tileSize
            };
            DrawRectangleRec(carRect, RED);
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
                    lane->rect.y,
                    world->tileSize * 2,
                    world->tileSize
                };
                
                if (CheckCollisionRecs(playerRect, carRect)) {
                    return true;  // Colisão com carro!
                }
            }
        }
    }
    return false;
}

void World_AddLaneOnTop(World *world, int screenWidth, int screenHeight) {
    if (world->laneCount >= MAX_LANES) return;
    
    // Encontrar a Y mais alta (menor valor)
    float highestY = world->lanes[0].rect.y;
    for (int i = 1; i < world->laneCount; i++) {
        if (world->lanes[i].rect.y < highestY) {
            highestY = world->lanes[i].rect.y;
        }
    }
    
    // Criar nova lane acima da mais alta
    Lane newLane;
    newLane.rect = (Rectangle){0, highestY - world->tileSize, screenWidth, world->tileSize};
    
    // Alternar tipos
    int laneType = world->laneCount % 3;
    if (laneType == 0) {
        newLane.type = LANE_ROAD;
        newLane.color = (Color){80, 80, 80, 255};
        newLane.hasCar = true;
        newLane.carX = (world->laneCount % 2 == 0) ? -world->tileSize : screenWidth;
        newLane.carSpeed = (world->laneCount % 2 == 0) ? 150.0f : -120.0f;
    } else {
        newLane.type = LANE_GRASS;
        newLane.color = (Color){34, 139, 34, 255};
        newLane.hasCar = false;
    }
    
    world->lanes[world->laneCount] = newLane;
    world->laneCount++;
}