#include "raylib.h"
#include "player.h"
#include <stdio.h>
#include <stdlib.h>

char *SPRITE = "resources/cat.png";
char *UPLOADED_SPRITE = "/mnt/data/94c4c162-4883-46ef-9714-199eec5663b4.png";
char *SAVE_FILE = "highscore.txt";

void alocarmatriz(Jogador *p) {
    p->linhas = 2;
    p->colunas = 2;
    p->matriz = (int **)malloc(sizeof(int *) * p->linhas);
    if (p->matriz == NULL) return;
    for (int i = 0; i < p->linhas; i++) {
        p->matriz[i] = (int *)malloc(sizeof(int) * p->colunas);
        for (int j = 0; j < p->colunas; j++) {
            p->matriz[i][j] = (i == j) ? 1 : 0;
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

Jogador *criargato(Vector2 startPos, float size) {
    Jogador *p = (Jogador *)malloc(sizeof(Jogador));
    if (p == NULL) return NULL;
    p->box = (Rectangle){ startPos.x, startPos.y, size, size };
    p->linha = 0;

#ifdef GATO
    p->sprite = LoadTexture(SPRITE);
    if (p->sprite.id == 0) {
        p->sprite = LoadTexture(UPLOADED_SPRITE);
        if (p->sprite.id == 0) {
            p->sprite = (Texture2D){0};
        }
    }
#endif

    alocarmatriz(p);
    return p;
}

void movimentacao(Jogador *p, float tile, int largura, int altura) {
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
    if (p->box.x + p->box.largura > largura) p->box.x = largura - p->box.largura;
    if (p->box.y + p->box.altura > altura) p->box.y = altura - p->box.altura;
}

void personagem(const Jogador *p, Vector2 cameraOffset) {
    Rectangle destino = {
        p->box.x,
        p->box.y - cameraOffset.y,
        p->box.largura,
        p->box.altura
    };

#ifdef GATO
    if (p->sprite.id != 0) {
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
            return;
        }
    }
#endif
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
    if (f == NULL) {
        return 0;
    }
    int recorde = 0;
    if (fscanf(f, "%d", &recorde) != 1) {
        recorde = 0;
    }
    fclose(f);
    return recorde;
}