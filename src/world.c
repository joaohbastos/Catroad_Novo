#include "world.h"
#include "raylib.h"

static float randf(float a, float b) {
    return a + (float)GetRandomValue(0, 10000) / 10000.0f * (b - a);
}

void World_Init(World *w, int screenW, int screenH, float tile) {
    w->tile = tile;
    w->laneCount = MAX_LANES;

    for (int i = 0; i < w->laneCount; i++) {
        Lane *ln = &w->lanes[i];
        ln->y = screenH - (int)((i+1) * tile);
        ln->isRoad = (i % 2 == 1);
        ln->carCount = 0;

        if (ln->isRoad) {
            int cars = 2 + GetRandomValue(0, 2);
            if (cars > MAX_CARS_PER_LANE) cars = MAX_CARS_PER_LANE;
            ln->carCount = cars;

            int dir = (GetRandomValue(0, 1) == 0) ? -1 : +1;
            float speedBase = randf(90.0f, 160.0f);
            for (int c = 0; c < cars; c++) {
                float wcar = tile * randf(1.2f, 1.8f);
                float hcar = tile * 0.8f;
                float x = (float)GetRandomValue(-screenW, screenW);
                ln->cars[c].box = (Rectangle){ x, (float)ln->y + (tile - hcar)*0.5f, wcar, hcar };
                ln->cars[c].speed = speedBase * randf(0.9f, 1.2f);
                ln->cars[c].dir = dir;
                ln->cars[c].active = true;
            }
        }
    }
}

// 🎯 FUNÇÃO CORRIGIDA - MANTÉM O PADRÃO ORIGINAL
void World_AddLaneOnTop(World *w, int screenW, int screenH) {
    // Encontra o padrão da última lane (se é road ou grass)
    bool lastWasRoad = w->lanes[0].isRoad;
    
    // Move todas as lanes para baixo
    for (int i = w->laneCount - 1; i > 0; i--) {
        w->lanes[i] = w->lanes[i-1];
        w->lanes[i].y += (int)w->tile;
        
        // Atualiza a posição Y dos carros também
        for (int c = 0; c < w->lanes[i].carCount; c++) {
            w->lanes[i].cars[c].box.y += w->tile;
        }
    }
    
    // Cria nova lane no topo - ALTERNA entre road e grass
    Lane *newLane = &w->lanes[0];
    newLane->y = (int)(screenH - (w->laneCount * w->tile));
    newLane->isRoad = !lastWasRoad; // Alterna com a anterior
    newLane->carCount = 0;
    
    if (newLane->isRoad) {
        int cars = 2 + GetRandomValue(0, 2);
        if (cars > MAX_CARS_PER_LANE) cars = MAX_CARS_PER_LANE;
        newLane->carCount = cars;
        
        int dir = (GetRandomValue(0, 1) == 0) ? -1 : +1;
        float speedBase = randf(90.0f, 160.0f);
        
        for (int c = 0; c < cars; c++) {
            float wcar = w->tile * randf(1.2f, 1.8f);
            float hcar = w->tile * 0.8f;
            float x = (float)GetRandomValue(-screenW, screenW);
            newLane->cars[c].box = (Rectangle){ x, newLane->y + (w->tile - hcar)*0.5f, wcar, hcar };
            newLane->cars[c].speed = speedBase * randf(0.9f, 1.2f);
            newLane->cars[c].dir = dir;
            newLane->cars[c].active = true;
        }
    }
    
    printf("🛣️ Nova lane: %s (alternando padrão)\n", newLane->isRoad ? "RUA" : "GRAMA");
}
void World_Update(World *w, float dt, int screenW) {
    for (int i = 0; i < w->laneCount; i++) {
        Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;
        for (int c = 0; c < ln->carCount; c++) {
            Car *car = &ln->cars[c];
            if (!car->active) continue;
            car->box.x += car->dir * car->speed * dt;

            if (car->dir > 0 && car->box.x > screenW + 20) {
                car->box.x = -car->box.width - (float)GetRandomValue(0, (int)(w->tile*2));
            } else if (car->dir < 0 && car->box.x + car->box.width < -20) {
                car->box.x = screenW + (float)GetRandomValue(0, (int)(w->tile*2));
            }
        }
    }
}

void World_Draw(const World *w, Vector2 cameraOffset) {
    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        Color col = ln->isRoad ? (Color){ 60, 60, 60, 255 } : (Color){ 60, 120, 60, 255 };
        
        int drawY = ln->y - (int)cameraOffset.y;
        DrawRectangle(0, drawY, GetScreenWidth(), (int)w->tile, col);

        if (ln->isRoad) {
            DrawLine(0, drawY, GetScreenWidth(), drawY, (Color){ 230, 230, 230, 120 });
            DrawLine(0, drawY + (int)w->tile, GetScreenWidth(), drawY + (int)w->tile, (Color){ 230, 230, 230, 120 });
        }
    }

    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;
        for (int c = 0; c < ln->carCount; c++) {
            const Car *car = &ln->cars[c];
            if (!car->active) continue;
            
            Rectangle carRect = car->box;
            carRect.y -= cameraOffset.y;
            
            DrawRectangleRec(carRect, (Color){ 200, 30, 60, 255 });
            DrawCircle(carRect.x + (car->dir > 0 ? carRect.width - 6 : 6),
                       carRect.y + carRect.height*0.3f, 3, YELLOW);
            DrawCircle(carRect.x + (car->dir > 0 ? carRect.width - 6 : 6),
                       carRect.y + carRect.height*0.7f, 3, YELLOW);
        }
    }
}

bool World_CheckCollision(const World *w, Rectangle player) {
    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;
        for (int c = 0; c < ln->carCount; c++) {
            const Car *car = &ln->cars[c];
            if (!car->active) continue;
            if (CheckCollisionRecs(player, car->box)) return true;
        }
    }
    return false;
}