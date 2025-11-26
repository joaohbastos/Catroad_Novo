#include "raylib.h"
#include "player.h"
#include "world.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define largura 1200
#define altura 720
#define TILE_SIZE 50.0f
#define tempo 35.0f

typedef enum { menu, jogando, fimdejogo } estados;

static Jogador *player = NULL;
static Mundo mundo;
static timer cronometro;
static estados estado;
static Vector2 deslocamentocamera = {0, 0};
static float dificuldade = 1.0f;
static int recorde = 0;

void NovoJogo(void) {
    if (player) {
        player = NULL;
    }
    criarmundo(&mundo, largura, altura, TILE_SIZE);
    player = criargato(
        (Vector2){ largura * 0.5f - TILE_SIZE * 0.5f, altura - TILE_SIZE * 4.0f },
        TILE_SIZE
    );
    resettempo(&cronometro, tempo);
    deslocamentocamera = (Vector2){0, 0};
    dificuldade = 1.0f;
    estado = jogando;
}

void iniciarjogo(void) {
    InitWindow(largura, altura, "CatRoad - raylib");
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)GetTime());
    recorde = carregar_recorde();
    mundo.faixas = NULL;
    mundo.mapa= NULL;
    mundo.quantidadelinhas = mundo.capacidade = mundo.colunasMatriz = 0;
    player = NULL;
    estado = menu;
}

void atualizarjogo(void) {
    static bool iniciou = false;
    float dt = GetFrameTime();  
    if (dt > 0.05f) dt = 0.05f;
    if (estado == menu) {
        if (IsKeyPressed(KEY_ENTER)) {
            NovoJogo();
            iniciou = false;
        }
        return;
    }
    if (!iniciou) {
        iniciou = true;
        return;
    }
    if (estado == jogando) {
        passartempo(&cronometro, dt);
        dificuldade = 1.0f + player->linha * 0.07f;
        if (dificuldade > 3.0f) dificuldade = 3.0f;
        atualizar_mundo(&mundo, dt, largura, dificuldade);
        Player_Update(player, TILE_SIZE, largura, altura);

        float CAM_TARGET_Y = (float)altura * 0.5f - TILE_SIZE * 0.5f;
        float targetOffsetY = player->box.y - CAM_TARGET_Y;
        float worldHeight = (float)mundo.quantidadelinhas * mundo.tamanho;

        if (worldHeight <= (float)altura) {
            deslocamentocamera.y = 0.0f;
        } else {
            float maxNegativeScroll = - (worldHeight - (float)altura);
            if (targetOffsetY > 0.0f) targetOffsetY = 0.0f;
            if (targetOffsetY < maxNegativeScroll) targetOffsetY = maxNegativeScroll;
            float suavizacao = 0.20f;
            deslocamentocamera.y += (targetOffsetY - deslocamentocamera.y) * suavizacao;
        }
        if (worldHeight > player->box.height) {
            if (player->box.y > worldHeight - player->box.height)
                player->box.y = worldHeight - player->box.height;
        } else {
            if (player->box.y > (float)altura - player->box.height)
                player->box.y = (float)altura - player->box.height;
        }
        if (checarcolisao(&mundo, player->box) || tempoesgotado(&cronometro)) {
            estado = fimdejogo;
            if (player->linha > recorde) {
                recorde = player->linha;
                salvar_recorde(recorde);
            }
        }
    } else if (estado == fimdejogo) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_R)) {
            NovoJogo();
            iniciou = false;
        }
    }
}

void desenharcenario(void) {
    BeginDrawing();
    ClearBackground((Color){30, 30, 40, 255});
    if (estado == menu) {
        char *titulo = "CatRoad";
        char *sub = "Pressione [ENTER] para comecar";
        char *info = "Use W A S D ou setas para se mover";
        int tw = MeasureText(titulo, 60);
        int sw = MeasureText(sub, 24);
        int iw = MeasureText(info, 20);

        DrawText(titulo, largura/2 - tw/2, altura/2 - 120, 60, RAYWHITE);
        DrawText(sub, largura/2 - sw/2, altura/2 - 40, 24, YELLOW);
        DrawText(info, largura/2 - iw/2, altura/2 + 10, 20, LIGHTGRAY);

        char rec[50];
        snprintf(rec, sizeof(rec), "Recorde atual: %d linhas", recorde);
        int rw = MeasureText(rec, 20);
        DrawText(rec, largura/2 - rw/2, altura/2 + 50, 20, GREEN);
        EndDrawing();
        return;
    }

    planodefundo(&mundo, deslocamentocamera);
    personagem(player, deslocamentocamera);
    DrawRectangle(0, 0, largura, 40, (Color){0, 0, 0, 140});
    char hud[100];
    snprintf(hud, sizeof(hud), "Tempo: %02d | Distancia: %d | Recorde: %d",
             (int)cronometro.timeLeft, player->linha, recorde);
    DrawText(hud, 16, 10, 20, RAYWHITE);

    char dif[50];
    snprintf(dif, sizeof(dif), "Dificuldade: %.2f", dificuldade);
    DrawText(dif, largura - 230, 10, 20, YELLOW);

    if (estado == fimdejogo) {
        DrawRectangle(0, 0, largura, altura, (Color){0, 0, 0, 180});

        char *msg = "Game Over!";
        int mw = MeasureText(msg, 40);
        DrawText(msg, largura/2 - mw/2, altura/2 - 60, 40, RED);

        char pont[100];
        snprintf(pont, sizeof(pont), "Distancia: %d linhas", player->linha);
        int pw = MeasureText(pont, 24);
        DrawText(pont, largura/2 - pw/2, altura/2 - 16, 24, RAYWHITE);

        char *instr = "[ENTER] ou [R] para reiniciar";
        int iw = MeasureText(instr, 20);
        DrawText(instr, largura/2 - iw/2, altura/2 + 20, 20, GRAY);
    }
    EndDrawing();
}

void parar_de_rodar(void) {
    if (player) {
        player = NULL;
    }
    liberarmundo(&mundo);
}

bool fechar_jogo(void) {
    return WindowShouldClose();
}