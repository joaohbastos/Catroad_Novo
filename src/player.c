#include "raylib.h"
#include "player.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>

char *SPRITE = "resources/cat.png";
char *UPLOADED_SPRITE = "player.png"; 
char *SAVE_FILE = "highscore.txt";

void alocarmatriz(Jogador *p) {
    p->linhas = 3;
    p->colunas = 3;
    p->matriz = (int **)malloc(sizeof(int *) * p->linhas);
    if (p->matriz == NULL) return;
    for (int i = 0; i < p->linhas; i++) {
        p->matriz[i] = (int *)malloc(sizeof(int) * p->colunas);
        for (int j = 0; j < p->colunas; j++) {
            p->matriz[i][j] = 0;
        }
    }
}

void liberarMatriz(Jogador *p) {
    if (p->matriz != NULL) {
        for (int i = 0; i < p->linhas; i++) {
            if (p->matriz[i] != NULL) free(p->matriz[i]);
        }
        free(p->matriz);
        p->matriz = NULL;
    }
}

void atualizar_radar(Jogador *p, Mundo *m, float tile_size) {
    p->matriz[1][1] = 2; 
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == 1 && j == 1) continue;

            float offY = (i - 1) * tile_size;
            float offX = (j - 1) * tile_size;

            Rectangle sensor = {
                p->box.x + offX,
                p->box.y + offY,
                p->box.largura * 0.8f,
                p->box.altura * 0.8f
            };

            if (checarcolisao(m, sensor)) {
                p->matriz[i][j] = 1;
            } else {
                p->matriz[i][j] = 0;
            }
        }
    }
}

Jogador *criargato(Vector2 startPos, float size) {
    Jogador *p = (Jogador *)malloc(sizeof(Jogador));
    p->box = (Rectangle){ startPos.x, startPos.y, size, size };
    p->linha = 0;

    p->sprite = LoadTexture(SPRITE);
    if (p->sprite.id == 0) {
        p->sprite = LoadTexture(UPLOADED_SPRITE);
    }

    alocarmatriz(p);
    return p;
}

void movimentacao(Jogador *p, float tile, int larguraTela, int alturaTela) {
    Vector2 delta = {0};
    bool movedRow = false;
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        delta.y -= tile;
        movedRow = true;
    } 
    else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        if (p->linha > 0) {
            delta.y += tile;
            movedRow = true;
        }
    } 
    else if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
        delta.x -= tile;
    } 
    else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
        delta.x += tile;
    }

    if (delta.x != 0 || movedRow) {
        p->box.x += delta.x;

        if (movedRow) {
            p->box.y += delta.y;
            if (delta.y < 0) {
                p->linha += 1;
            } 
            else if (delta.y > 0) { 
                p->linha -= 1;
                if (p->linha < 0) p->linha = 0;
            }
        }
    }

    if (p->box.x < 0) p->box.x = 0;
    if (p->box.x + p->box.largura > larguraTela) p->box.x = larguraTela - p->box.largura;
    if (p->box.y + p->box.altura > alturaTela) p->box.y = alturaTela - p->box.altura;
}

void personagem(const Jogador *p, Vector2 cameraOffset) {
    Rectangle destino = {
        p->box.x,
        p->box.y - cameraOffset.y,
        p->box.largura,
        p->box.altura
    };

    if (p->matriz != NULL) {
        int radarX = 10;
        int radarY = 60; 
        int tamCélula = 10;
        DrawText("RADAR:", radarX, radarY - 15, 10, WHITE);
        for (int i = 0; i < p->linhas; i++) {
            for (int j = 0; j < p->colunas; j++) {
                Color cor = GRAY;
                if (p->matriz[i][j] == 1) cor = RED;
                if (p->matriz[i][j] == 2) cor = GREEN;
                DrawRectangle(radarX + j * (tamCélula + 2), 
                              radarY + i * (tamCélula + 2), 
                              tamCélula, tamCélula, cor);
            }
        }
    }

    if (p->sprite.id > 0) {
        float tw = (float)p->sprite.largura;
        float th = (float)p->sprite.altura;
        if (tw > 0 && th > 0) {
            float sw = destino.largura / tw;
            float sh = destino.altura / th;
            float s = (sw < sh) ? sw : sh;
            if (s <= 0.0f) s = 1.0f;
            float drawW = tw * s;
            float drawH = th * s;
            Rectangle drawDest ={
                destino.x + (destino.largura - drawW) * 0.5f,
                destino.y + (destino.altura - drawH) * 0.5f,
                drawW,
                drawH
            };
            Rectangle src = { 0, 0, tw, th };
            DrawTexturePro(p->sprite, src, drawDest, (Vector2){0,0}, 0.0f, WHITE);
        }
    } else {
        DrawRectangleRec(destino, RED);
    }
}

void salvar_recorde(int linha) {
    FILE *f = fopen(SAVE_FILE, "w");
    if (f != NULL) {
        fprintf(f, "%d\n", linha);
        fclose(f);
    }
}

int carregar_recorde(void) {
    FILE *f = fopen(SAVE_FILE, "r");
    int recorde = 0;
    if (fscanf(f, "%d", &recorde) != 1) recorde = 0;
    fclose(f);
    return recorde;
}