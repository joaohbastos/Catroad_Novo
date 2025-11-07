void World_AddLaneOnTop(World *w, int screenW, int screenH) {
    // 1. Encontra a lane MAIS ALTA (menor Y) e MAIS BAIXA (maior Y)
    int highestY = w->lanes[0].y;
    int highestIndex = 0;
    int lowestY = w->lanes[0].y;
    int lowestIndex = 0;
    
    for (int i = 1; i < w->laneCount; i++) {
        if (w->lanes[i].y < highestY) {
            highestY = w->lanes[i].y;
            highestIndex = i;
        }
        if (w->lanes[i].y > lowestY) {
            lowestY = w->lanes[i].y;
            lowestIndex = i;
        }
    }
    
    // 2. REUTILIZA a lane mais baixa (que está fora da tela devido à câmera)
    Lane *newLane = &w->lanes[lowestIndex];
    newLane->y = highestY - (int)w->tile; // Coloca ACIMA da lane mais alta
    
    newLane->isRoad = (GetRandomValue(0, 1) == 1);
    
    if (newLane->isRoad) {
        int cars = 2 + GetRandomValue(0, 2);
        if (cars > MAX_CARS_PER_LANE) cars = MAX_CARS_PER_LANE;
        newLane->carCount = cars;
        
        int dir = (GetRandomValue(0, 1) == 0) ? -1 : +1;
        float speedBase = randf(90.0f, 160.0f);
        
        for (int c = 0; c < cars; c++) {
            float wcar = w->tile * randf(1.2f, 1.8f);
            float hcar = w->tile * 0.8f;
            float x = (dir > 0) ? 
                -wcar - GetRandomValue(0, 100) :
                screenW + GetRandomValue(0, 100);
            
            newLane->cars[c].box = (Rectangle){ x, (float)newLane->y + (w->tile - hcar)*0.5f, wcar, hcar };
            newLane->cars[c].speed = speedBase * randf(0.9f, 1.2f);
            newLane->cars[c].dir = dir;
            newLane->cars[c].active = true;
        }
    } else {
        newLane->carCount = 0;
    }
    
    printf("✅ Nova lane criada em Y=%d (acima de Y=%d)\n", newLane->y, highestY);
}