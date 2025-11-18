#include "raylib.h"
#include "player.h"
#include "world.h"
#include "timer.h"
#include <stdio.h>

#define largura 800
#define altura 600
#define tamanho 48.0f
#define tempo 35.0f

typedef enum {jogando, fimdejogo} estados;

static Jogador player;
static Mundo mundo;
static timer cronometro;
static estados estado;
static Vector2 deslocamentocamera = {0, 0};

static float dificuldade = 1.0f;

//inicializa o mundo novamente, voltando do estado inicial
static void reiniciar(void) {
    criarmundo(&mundo, largura, altura, tamanho); 
    Player_Init(&player, (Vector2){ largura*0.5f - tamanho*0.5f, altura - tamanho }, tamanho);
    resettempo(&cronometro, tempo);
    estado = jogando;
    deslocamentocamera = (Vector2){0, 0};
    dificuldade = 1.0f;
    printf("🔄 Jogo reiniciado\n");
}

void iniciarjogo(void) {
    InitWindow(largura, altura, "CatRoad - raylib");
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)GetTime());
    reiniciar();
}

void atualizarjogo(void) {
    float dt = GetFrameTime();
    if (estado == jogando) {
        passartempo(&cronometro, dt);
        dificuldade = 1.0f + player.linha * 0.08f;
        if (dificuldade > 3.0f) {
            dificuldade = 3.0f;
        }

        World_Update(&mundo, dt, largura, dificuldade);
        Player_Update(&player, dt, tamanho, largura, altura);

        //camera
        if (player.box.y < 300.0f){
            deslocamentocamera.y = player.box.y - 300.0f;
        }

        if (deslocamentocamera.y > 0){
            deslocamentocamera.y = 0;
        }

        if (checarcolisao(&mundo, player.box) || tempoesgotado(&cronometro)) {
            estado = fimdejogo;
        }
        if (IsKeyPressed(KEY_R)) {
            reiniciar();
        }

    }else if(estado == fimdejogo){
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_R)) {
            reiniciar();
        }
    }
}

void desenharcenario(void) {
    BeginDrawing();
    ClearBackground((Color){ 30, 30, 40, 255 });
    planodefundo(&mundo, deslocamentocamera);
    personagem(&player, deslocamentocamera);

    // UI
    DrawRectangle(0, 0, largura, 40, (Color){0, 0, 0, 140});
    char hud[128];
    snprintf(hud, sizeof(hud), "Tempo: %02d   |   Pontos: %d | Linha: %d", (int)cronometro.timeLeft, player.ponto, player.linha);   
    DrawText(hud, 16, 10, 20, RAYWHITE);

    //printar a dificuldade
    char textodificuldade[64];
    snprintf(textodificuldade, sizeof(textodificuldade), "Dificuldade: %.2f", dificuldade);
    DrawText(textodificuldade, largura - 230, 10, 20, YELLOW);

    //instruções para quando o jogo acaba
    if (estado == fimdejogo) {
        DrawRectangle(0, 0, largura, altura, (Color){0, 0, 0, 180});
        
        const char *mensagem = "Game Over!";
        int textomensagem = MeasureText(mensagem, 40);
        DrawText(mensagem, largura/2 - textomensagem/2, altura/2 - 60, 40, RED);

        char pontuacao[128];
        snprintf(pontuacao, sizeof(pontuacao), "Distancia: %d linhas", player.ponto);
        int textopontuacao = MeasureText(pontuacao, 24);
        DrawText(pontuacao, largura/2 - textopontuacao/2, altura/2 - 16, 24, RAYWHITE);

        const char *instrucao = "[ENTER] ou [R] para reiniciar";
        int textoinstrucao = MeasureText(instrucao, 20);
        DrawText(instrucao, largura/2 - textoinstrucao/2, altura/2 + 20, 20, GRAY);
    }
    EndDrawing();
}

void parar_de_rodar(void){
    Player_Unload(&player);  // 🐱 ADICIONE ESTA LINHA - Libera textura do gato
    if (IsAudioDeviceReady()) {
        CloseAudioDevice();
    }
    CloseWindow();
}

bool fechar_jogo(void){
    return WindowShouldClose();
}