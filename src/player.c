void Player_Update(Player *p, float dt, float tile, int screenW, int screenH) {
    if (p->moveCd > 0.0f) p->moveCd -= dt;

    Vector2 delta = {0};
    bool movedRow = false;

    if (p->moveCd <= 0.0f) {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            delta.y -= tile;
            movedRow = true;
        } else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            if (p->row > 0) {
                delta.y += tile;
                p->row -= 1;
                p->moveCd = 0.12f;
                p->box.y += delta.y;
            }
            return;
        } else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            delta.x -= tile;
        } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            delta.x += tile;
        }

        if (delta.x != 0 || movedRow) {
            p->box.x += delta.x;
            if (movedRow) {
                p->box.y += delta.y;
                if (delta.y < 0) { // subiu uma linha
                    p->row += 1;
                    
                    // 🎯 ATUALIZAR SCORE
                    if (p->row > p->maxRow) {
                        p->maxRow = p->row;
                    }
                    p->score = p->row;
                    
                    // Debug a cada 10 linhas
                    if (p->row % 10 == 0) {
                        printf("🎯 Linha %d | Score: %d\n", p->row, p->score);
                    }
                }
            }
            p->moveCd = 0.12f;
        }
    }

    // Limites
    if (p->box.x < 0) p->box.x = 0;
    if (p->box.x + p->box.width > screenW) p->box.x = screenW - p->box.width;
    if (p->box.y + p->box.height > screenH) p->box.y = screenH - p->box.height;
}