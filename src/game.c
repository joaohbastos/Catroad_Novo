#include "raylib.h"
#include "player.h"
#include "world.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define LARGURA_TELA 1200
#define ALTURA_TELA 720
#define TILE_SIZE 50.0f
#define TEMPO_JOGO 35.0f

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
    criarmundo(&mundo, LARGURA_TELA, ALTURA_TELA, TILE_SIZE);
    player = criargato(
        (Vector2){ LARGURA_TELA * 0.5f - TILE_SIZE * 0.5f, ALTURA_TELA - TILE_SIZE * 4.0f },
        TILE_SIZE
    );
    resettempo(&cronometro, TEMPO_JOGO);
    deslocamentocamera = (Vector2){0, 0};
    dificuldade = 1.0f;
    estado = jogando;
}

void iniciarjogo(void) {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "CatRoad - Radar Edition");
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)GetTime());
    recorde = carregar_recorde();
    mundo.faixas = NULL;
    mundo.mapa= NULL;
    mundo.quantidadelinhas = mundo.capacidade = mundo.colunasmatriz = 0;
    player = NULL;
    estado = menu;
}

void atualizarjogo(void) {
    static bool iniciou = false;
    float frametime = GetFrameTime();  
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
        passartempo(&cronometro, frametime);
        dificuldade = 1.0f + player->linha * 0.07f;
        if (dificuldade > 3.0f) dificuldade = 3.0f;
        
        atualizar_mundo(&mundo, frametime, LARGURA_TELA, dificuldade);
        movimentacao(player, TILE_SIZE, LARGURA_TELA, ALTURA_TELA);
        atualizar_radar(player, &mundo, TILE_SIZE); 

        float CAM_TARGET_Y = (float)ALTURA_TELA * 0.5f - TILE_SIZE * 0.5f;
        float targetOffsetY = player->box.y - CAM_TARGET_Y;
        float worldHeight = (float)mundo.quantidadelinhas * mundo.tamanho;
        if (worldHeight <= (float)ALTURA_TELA) {
            deslocamentocamera.y = 0.0f;
        } else {
            float maxNegativeScroll = - (worldHeight - (float)ALTURA_TELA);
            if (targetOffsetY > 0.0f) targetOffsetY = 0.0f;
            if (targetOffsetY < maxNegativeScroll) targetOffsetY = maxNegativeScroll;
            float suavizacao = 0.20f;
            deslocamentocamera.y += (targetOffsetY - deslocamentocamera.y) * suavizacao;
        }
        
        if (worldHeight > player->box.altura) {
            if (player->box.y > worldHeight - player->box.altura)
                player->box.y = worldHeight - player->box.altura;
        } else {
            if (player->box.y > (float)ALTURA_TELA - player->box.altura)
                player->box.y = (float)ALTURA_TELA - player->box.altura;
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

        DrawText(titulo, LARGURA_TELA/2 - tw/2, ALTURA_TELA/2 - 120, 60, RAYWHITE);
        DrawText(sub, LARGURA_TELA/2 - sw/2, ALTURA_TELA/2 - 40, 24, YELLOW);
        DrawText(info, LARGURA_TELA/2 - iw/2, ALTURA_TELA/2 + 10, 20, LIGHTGRAY);

        char rec[50];
        snprintf(rec, sizeof(rec), "Recorde atual: %d linhas", recorde);
        int rw = MeasureText(rec, 20);
        DrawText(rec, LARGURA_TELA/2 - rw/2, ALTURA_TELA/2 + 50, 20, GREEN);
        EndDrawing();
        return;
    }

    planodefundo(&mundo, deslocamentocamera);
    personagem(player, deslocamentocamera);
    DrawRectangle(0, 0, LARGURA_TELA, 40, (Color){0, 0, 0, 140});
    
    char hud[100];
    snprintf(hud, sizeof(hud), "Tempo: %02d | Distancia: %d | Recorde: %d",
             (int)cronometro.timeLeft, player->linha, recorde);
    DrawText(hud, 16, 10, 20, RAYWHITE);

    char dif[50];
    snprintf(dif, sizeof(dif), "Dificuldade: %.2f", dificuldade);
    DrawText(dif, LARGURA_TELA - 230, 10, 20, YELLOW);

    if (estado == fimdejogo) {
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){0, 0, 0, 180});

        char *msg = "Game Over!";
        int mw = MeasureText(msg, 40);
        DrawText(msg, LARGURA_TELA/2 - mw/2, ALTURA_TELA/2 - 60, 40, RED);

        char pont[100];
        snprintf(pont, sizeof(pont), "Distancia: %d linhas", player->linha);
        int pw = MeasureText(pont, 24);
        DrawText(pont, LARGURA_TELA/2 - pw/2, ALTURA_TELA/2 - 16, 24, RAYWHITE);

        char *instr = "[ENTER] ou [R] para reiniciar";
        int iw = MeasureText(instr, 20);
        DrawText(instr, LARGURA_TELA/2 - iw/2, ALTURA_TELA/2 + 20, 20, GRAY);
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