#include "player.h"
#include "raylib.h"

static const char *DEFAULT_SPRITE_PATH = "resources/cat.png";
static const char *UPLOADED_SPRITE_PATH = "/mnt/data/94c4c162-4883-46ef-9714-199eec5663b4.png";

void Player_Init(Jogador *p, Vector2 startPos, float size) {
    if (!p) return;

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