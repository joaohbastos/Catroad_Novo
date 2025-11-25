#include "raylib.h"
#include "player.h"
#include "world.h"
#include "timer.h"
#include <stdio.h>

#define largura 1200
#define altura 720
#define TILE_SIZE 48.0f
#define tempo 35.0f

typedef enum {jogando, fimdejogo} estados;

static Jogador player;
static Mundo mundo;
static timer cronometro;
static estados estado;
static Vector2 deslocamentocamera = {0, 0};

static float dificuldade = 1.0f;

static void reiniciar(void) {
    criarmundo(&mundo, largura, altura, TILE_SIZE);
    Player_Init(&player, (Vector2){ largura*0.5f - TILE_SIZE*0.5f, altura - TILE_SIZE * 4.0f }, TILE_SIZE);
    resettempo(&cronometro, tempo);
    estado = jogando;
    deslocamentocamera = (Vector2){0, 0};
    dificuldade = 1.0f;
}

void iniciarjogo(void) {
    InitWindow(largura, altura, "CatRoad - raylib");
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)GetTime());
    reiniciar();
}

void atualizarjogo(void) {
    float dt = GetFrameTime();
    if (dt > 0.05f) dt = 0.05f;

    if (estado == jogando) {
        passartempo(&cronometro, dt);
        dificuldade = 1.0f + player.linha * 0.08f;
        if (dificuldade > 3.0f) dificuldade = 3.0f;

        atualizar_mundo(&mundo, dt, largura, dificuldade);
        Player_Update(&player, TILE_SIZE, largura, altura);

        const float CAM_TARGET_Y = (float)altura * 0.5f - TILE_SIZE * 0.5f;
        float targetOffsetY = player.box.y - CAM_TARGET_Y;

        float worldHeight = (float)mundo.quantidadelinhas * mundo.tamanho;

        if (worldHeight <= (float)altura) {
            deslocamentocamera.y = 0.0f;
        } else {
            float maxNegativeScroll = - (worldHeight - (float)altura);
            if (targetOffsetY > 0.0f) targetOffsetY = 0.0f;
            if (targetOffsetY < maxNegativeScroll) targetOffsetY = maxNegativeScroll;
            float suavizacao = 0.20;
            deslocamentocamera.y += (targetOffsetY - deslocamentocamera.y) * suavizacao;
        }

        if (worldHeight > player.box.height) {
            if (player.box.y > worldHeight - player.box.height) player.box.y = worldHeight - player.box.height;
        } else {
            if (player.box.y > (float)altura - player.box.height) player.box.y = (float)altura - player.box.height;
        }

        if (checarcolisao(&mundo, player.box) || tempoesgotado(&cronometro)) {
            estado = fimdejogo;
        }
        if (IsKeyPressed(KEY_R)) reiniciar();

    } else if (estado == fimdejogo) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_R)) reiniciar();
    }
}

void desenharcenario(void) {
    BeginDrawing();
    ClearBackground((Color){ 30, 30, 40, 255 });
    planodefundo(&mundo, deslocamentocamera);
    personagem(&player, deslocamentocamera);

    DrawRectangle(0, 0, largura, 40, (Color){0, 0, 0, 140});
    char hud[128];
    snprintf(hud, sizeof(hud), "Tempo: %02d   |   Distancia: %d  ", (int)cronometro.timeLeft, player.linha);
    DrawText(hud, 16, 10, 20, RAYWHITE);

    char textodificuldade[64];
    snprintf(textodificuldade, sizeof(textodificuldade), "Dificuldade: %.2f", dificuldade);
    DrawText(textodificuldade, largura - 230, 10, 20, YELLOW);

    if (estado == fimdejogo) {
        DrawRectangle(0, 0, largura, altura, (Color){0, 0, 0, 180});
        const char *mensagem = "Game Over!";
        int textomensagem = MeasureText(mensagem, 40);
        DrawText(mensagem, largura/2 - textomensagem/2, altura/2 - 60, 40, RED);

        char pontuacao[128];
        snprintf(pontuacao, sizeof(pontuacao), "Distancia: %d linhas", player.linha);
        int textopontuacao = MeasureText(pontuacao, 24);
        DrawText(pontuacao, largura/2 - textopontuacao/2, altura/2 - 16, 24, RAYWHITE);

        const char *instrucao = "[ENTER] ou [R] para reiniciar";
        int textoinstrucao = MeasureText(instrucao, 20);
        DrawText(instrucao, largura/2 - textoinstrucao/2, altura/2 + 20, 20, GRAY);
    }

    EndDrawing();
}

void parar_de_rodar(void) {
#ifdef PLAYER_HAS_SPRITE
    if (player.sprite.id != 0) {
        UnloadTexture(player.sprite);
        player.sprite = (Texture2D){0};
    }
#endif
}

bool fechar_jogo(void) {
    return WindowShouldClose();
}