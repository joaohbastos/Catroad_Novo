#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"

#define MAX_LANES 350

typedef enum {
    pavimento,
    estrada,
    rio
} tipo;

typedef struct {
    tipo type;
    Rectangle rect;
    bool hasCar;
    int carCount;
    float carX;
    float velocidade;
    float carSpacing;
    Color cor;
} Lane;

typedef struct {
    Lane ruas[MAX_LANES];
    int quantidade_linha;
    float tileSize;
} World;

void criarmundo(World *mundo, int screenWidth, int screenHeight, float tileSize);
void World_Update(World *mundo, float dt, int screenWidth);
void World_Draw(const World *mundo, Vector2 cameraOffset);
bool World_CheckCollision(const World *mundo, Rectangle playerRect);

#endif