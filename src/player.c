#include "player.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

static const char *DEFAULT_SPRITE_PATH = "resources/cat.png";
static const char *UPLOADED_SPRITE_PATH = "/mnt/data/94c4c162-4883-46ef-9714-199eec5663b4.png";
static const char *SAVE_FILE = "highscore.txt";

/* ------------------------ FUNÇÕES INTERNAS ------------------------ */

static void AlocarMatrizAnim(Jogador *p) {
    p->linhasAnim = 2;
    p->colunasAnim = 2;
    p->matrizAnim = (int **)malloc(sizeof(int *) * p->linhasAnim);
    for (int i = 0; i < p->linhasAnim; i++) {
        p->matrizAnim[i] = (int *)malloc(sizeof(int) * p->colunasAnim);
        for (int j = 0; j < p->colunasAnim; j++) {
            p->matrizAnim[i][j] = (i == j) ? 1 : 0; // apenas simbólico
        }
    }
}

static void LiberarMatrizAnim(Jogador *p) {
    if (!p || !p->matrizAnim) return;
    for (int i = 0; i < p->linhasAnim; i++) {
        free(p->matrizAnim[i]);
    }
    free(p->matrizAnim);
    p->matrizAnim = NULL;
}

/* ------------------------ FUNÇÕES PÚBLICAS ------------------------ */

Jogador *Player_Create(Vector2 startPos, float size) {
    Jogador *p = (Jogador *)malloc(sizeof(Jogador));
    if (!p) return NULL;

    p->box = (Rectangle){ startPos.x, startPos.y, size, size };
    p->linha = 0;

#ifdef PLAYER_HAS_SPRITE
    p->sprite = LoadTexture(DEFAULT_SPRITE_PATH);
    if (p->sprite.id == 0) {
        p->sprite = LoadTexture(UPLOADED_SPRITE_PATH);
        if (p->sprite.id == 0) {
            p->sprite = (Texture2D){0};
        }
    }
#endif

    AlocarMatrizAnim(p);
    return p;
}

void Player_Destroy(Jogador *p) {
    if (!p) return;

#ifdef PLAYER_HAS_SPRITE
    if (p->sprite.id != 0) {
        UnloadTexture(p->sprite);
    }
#endif

    LiberarMatrizAnim(p);
    free(p);
}

void Player_Update(Jogador *p, float tile, int screenW, int screenH) {
    if (!p) return;

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
    if (p->box.x + p->box.width > screenW) p->box.x = screenW - p->box.width;
    if (p->box.y + p->box.height > screenH) p->box.y = screenH - p->box.height;
}

void personagem(const Jogador *p, Vector2 cameraOffset) {
    if (!p) return;

    Rectangle destino = {
        p->box.x,
        p->box.y - cameraOffset.y,
        p->box.width,
        p->box.height
    };

#ifdef PLAYER_HAS_SPRITE
    if (p->sprite.id != 0) {
        float tw = (float)p->sprite.width;
        float th = (float)p->sprite.height;

        if (tw > 0 && th > 0) {
            float sw = destino.width / tw;
            float sh = destino.height / th;
            float s = (sw < sh) ? sw : sh;
            if (s <= 0.0f) s = 1.0f;

            float drawW = tw * s;
            float drawH = th * s;

            Rectangle drawDest = {
                destino.x + (destino.width - drawW) * 0.5f,
                destino.y + (destino.height - drawH) * 0.5f,
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

/* ------------------------ ARQUIVO: SALVAR/LEITURA ------------------------ */

void Player_SaveHighscore(int linha) {
    FILE *f = fopen(SAVE_FILE, "w");
    if (!f) return;
    fprintf(f, "%d\n", linha);
    fclose(f);
}

int Player_LoadHighscore(void) {
    FILE *f = fopen(SAVE_FILE, "r");
    if (!f) return 0;
    int record = 0;
    fscanf(f, "%d", &record);
    fclose(f);
    return record;
}
