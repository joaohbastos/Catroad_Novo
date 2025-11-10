#include "raylib.h"
#include "player.h"
#include "world.h"
#include "timer.h"
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define TILE 48.0f
#define TOTAL_TIME 35.0f

typedef enum { STATE_PLAYING, STATE_GAMEOVER } GameState;

static Jogador player;
static World world;
static GameTimer timer35;
static GameState state;
static Vector2 cameraOffset = {0, 0};

static void ResetGame(void) {
    criarmundo(&world, SCREEN_W, SCREEN_H, TILE); 
    
    Player_Init(&player, (Vector2){ SCREEN_W*0.5f - TILE*0.5f, SCREEN_H - TILE }, TILE);
    Timer_Reset(&timer35, TOTAL_TIME);
    state = STATE_PLAYING;
    cameraOffset = (Vector2){0, 0};
    printf("🔄 Jogo reiniciado\n");
}

void Game_Init(void) {
    InitWindow(SCREEN_W, SCREEN_H, "CatRoad - raylib");
    SetTargetFPS(60);
    
    // Tenta inicializar áudio, mas não quebra se falhar
    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }
    
    SetRandomSeed((unsigned int)GetTime());
    ResetGame();
}

void Game_Update(void) {
    float dt = GetFrameTime();

    if (state == STATE_PLAYING) {
        Timer_Update(&timer35, dt);
        World_Update(&world, dt, SCREEN_W);
        Player_Update(&player, dt, TILE, SCREEN_W, SCREEN_H);

        // Câmera
        if (player.box.y < 300.0f) {
            cameraOffset.y = player.box.y - 300.0f;
        }
        if (cameraOffset.y > 0) cameraOffset.y = 0;

        // Colisão ou tempo
        if (World_CheckCollision(&world, player.box) || Timer_IsOver(&timer35)) {
            state = STATE_GAMEOVER;
        }

        // Restart
        if (IsKeyPressed(KEY_R)) {
            ResetGame();
        }
    } else if (state == STATE_GAMEOVER) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_R)) {
            ResetGame();
        }
    }
}

void Game_Draw(void) {
    BeginDrawing();
    ClearBackground((Color){ 30, 30, 40, 255 });

    World_Draw(&world, cameraOffset);
    Player_Draw(&player, cameraOffset);

    // UI
    DrawRectangle(0, 0, SCREEN_W, 40, (Color){0, 0, 0, 140});
    char hud[128];
    
    snprintf(hud, sizeof(hud), "Tempo: %02d   |   Pontos: %d | Linha: %d",
             (int)timer35.timeLeft, player.ponto, player.linha);
             
    DrawText(hud, 16, 10, 20, RAYWHITE);

    if (state == STATE_GAMEOVER) {
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 180});
        
        const char *msg = "Game Over!";
        int fw = MeasureText(msg, 40);
        DrawText(msg, SCREEN_W/2 - fw/2, SCREEN_H/2 - 60, 40, RED);

        char sc[128];
        
        snprintf(sc, sizeof(sc), "Distancia: %d linhas", player.ponto);
        
        int sw = MeasureText(sc, 24);
        DrawText(sc, SCREEN_W/2 - sw/2, SCREEN_H/2 - 16, 24, RAYWHITE);

        const char *hint = "[ENTER] ou [R] para reiniciar";
        int hw = MeasureText(hint, 20);
        DrawText(hint, SCREEN_W/2 - hw/2, SCREEN_H/2 + 20, 20, GRAY);
    }

    EndDrawing();
}

void Game_Unload(void) {
    if (IsAudioDeviceReady()) {
        CloseAudioDevice();
    }
    CloseWindow();
}

bool Game_ShouldClose(void) {
    return WindowShouldClose();
}