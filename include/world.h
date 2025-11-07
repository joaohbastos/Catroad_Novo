#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"

#define MAX_LANES 350  // 🔥 Aumentamos para 350 (mais que 300)

typedef enum {
    LANE_GRASS,
    LANE_ROAD,
    LANE_RIVER
} LaneType;

typedef struct {
    LaneType type;
    Rectangle rect;
    bool hasCar;
    float carX;
    float carSpeed;
    Color color;
} Lane;

typedef struct {
    Lane lanes[MAX_LANES];
    int laneCount;
    float tileSize;
} World;

void World_Init(World *world, int screenWidth, int screenHeight, float tileSize);
void World_Update(World *world, float dt, int screenWidth);
void World_Draw(const World *world, Vector2 cameraOffset);
bool World_CheckCollision(const World *world, Rectangle playerRect);

#endif