// src/player.c
#include "player.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

/* Caminho do PNG enviado — ajuste aqui se quiser usar outro arquivo */
#ifndef PLAYER_SPRITE_PATH
#define PLAYER_SPRITE_PATH "/mnt/data/7b1fc5ea-06a8-4c68-a78e-63a48550bc06.png"
#endif

void Player_Init(Jogador *p, Vector2 startPos, float size) {
    if (!p) return;

    p->box = (Rectangle){ startPos.x, startPos.y, size, size };
    p->linha = 0;
    p->maxlinha = 0;
    p->ponto = 0;
    p->moveCd = 0.0f;

#ifdef PLAYER_HAS_SPRITE
    p->sprite = (Texture2D){0};
    /* Primeiro tenta o resource padrão (se você tiver), depois o PNG enviado */
    p->sprite = LoadTexture("resources/cat.png");
    if (p->sprite.id == 0) {
        /* tenta o upload local */
        p->sprite = LoadTexture(PLAYER_SPRITE_PATH);
        if (p->sprite.id == 0) {
            fprintf(stderr, "Aviso: nao foi possivel carregar sprite do jogador em resources/cat.png ou %s\n", PLAYER_SPRITE_PATH);
            p->sprite = (Texture2D){0};
        }
    }
#endif
}

void Player_Update(Jogador *p, float dt, float tile, int screenW, int screenH) {
    if (!p) return;

    if (p->moveCd > 0.0f) p->moveCd -= dt;

    Vector2 delta = {0};
    bool movedRow = false;

    if (p->moveCd <= 0.0f) {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            delta.y -= tile;
            movedRow = true;
        } else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            if (p->linha > 0) {
                delta.y += tile;
                movedRow = true;
            }
        } else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            delta.x -= tile;
        } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            delta.x += tile;
        }

        if (delta.x != 0 || movedRow) {
            p->box.x += delta.x;

            if (movedRow) {
                p->box.y += delta.y;

                if (delta.y < 0) {
                    p->linha += 1;
                    if (p->linha > p->maxlinha) p->maxlinha = p->linha;
                    p->ponto = p->linha;
                } else if (delta.y > 0) {
                    p->linha -= 1;
                    if (p->linha < 0) p->linha = 0;
                }
            }
            p->moveCd = 0.12f;
        }
    }

    if (p->box.x < 0) p->box.x = 0;
    if (p->box.x + p->box.width > screenW) p->box.x = screenW - p->box.width;
    /* Não limite p->box.y para cima (o mundo pode ter Y negativos);
       limita apenas a queda abaixo da área visível para evitar sair da tela. */
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
        /* Mantém proporção e centraliza a imagem dentro do destino */
        float tw = (float)p->sprite.width;
        float th = (float)p->sprite.height;
        if (tw <= 0 || th <= 0) {
            /* fallback para o retângulo */
            DrawRectangleRec(destino, (Color){240, 180, 60, 255});
        } else {
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
            Rectangle src = { 0.0f, 0.0f, tw, th };
            DrawTexturePro(p->sprite, src, drawDest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            return;
        }
    }
#endif

    /* fallback: desenha seu personagem em forma geométrica */
    DrawRectangleRec(destino, (Color){240, 180, 60, 255});
    DrawTriangle(
        (Vector2){destino.x + destino.width*0.25f, destino.y + 8},
        (Vector2){destino.x + destino.width*0.40f, destino.y - destino.height*0.25f},
        (Vector2){destino.x + destino.width*0.55f, destino.y + 8},
        (Color){240, 180, 60, 255}
    );
    DrawTriangle(
        (Vector2){destino.x + destino.width*0.60f, destino.y + 8},
        (Vector2){destino.x + destino.width*0.75f, destino.y - destino.height*0.25f},
        (Vector2){destino.x + destino.width*0.90f, destino.y + 8},
        (Color){240, 180, 60, 255}
    );
    DrawCircle(destino.x + destino.width*0.35f, destino.y + destino.height*0.45f, 3, BLACK);
    DrawCircle(destino.x + destino.width*0.65f, destino.y + destino.height*0.45f, 3, BLACK);
}
