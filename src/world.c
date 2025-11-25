#include "world.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

static void FreeCars(Carro *c) {
    while (c) {
        Carro *n = c->proximo;
        free(c);
        c = n;
    }
}

void liberarmundo(Mundo *m) {
    if (!m) return;
    if (m->faixas) {
        for (int i = 0; i < m->quantidadelinhas; i++)
            FreeCars(m->faixas[i].listaCarros);
        free(m->faixas);
        m->faixas = NULL;
    }
    if (m->mapaTerreno) {
        for (int i = 0; i < m->capacidade; i++)
            free(m->mapaTerreno[i]);
        free(m->mapaTerreno);
        m->mapaTerreno = NULL;
    }
    m->quantidadelinhas = m->capacidade = m->colunasMatriz = 0;
}

void criarmundo(Mundo *m, int largura, int altura, float tamanho) {
    if (!m) return;
    liberarmundo(m);
    m->tamanho = tamanho;
    m->dificuldadeatual = 1.0f;
    m->capacidade = MAXFAIXAS;
    m->colunasMatriz = 2;
    m->faixas = malloc(sizeof(Rua) * m->capacidade);
    m->mapaTerreno = malloc(sizeof(int *) * m->capacidade);
    if (!m->faixas || !m->mapaTerreno) return;

    for (int i = 0; i < m->capacidade; i++) {
        m->mapaTerreno[i] = malloc(sizeof(int) * m->colunasMatriz);

        Rua r = {0};
        r.rect = (Rectangle){0, (float)altura - (i + 1) * tamanho, (float)largura, tamanho};
        r.cor = (Color){130, 130, 130, 255};
        r.tipo = calcada;

        if (i % 2 == 0) {
            r.tipo = estrada;
            r.cor = (Color){60, 60, 60, 255};
            r.temcarro = GetRandomValue(0, 99) < 70;
            if (r.temcarro) {
                if (GetRandomValue(0, 1) == 0) {
                    r.posicaocarroX = -tamanho * (float)GetRandomValue(2, 4);
                    r.velocidade = (float)GetRandomValue(120, 220);
                } else {
                    r.posicaocarroX = (float)largura + tamanho * (float)GetRandomValue(2, 4);
                    r.velocidade = -(float)GetRandomValue(120, 220);
                }
                r.contagemcarros = GetRandomValue(1, 3);
                r.espacoentrecarros = (float)GetRandomValue(200, 400);
                if (r.espacoentrecarros < m->tamanho * 1.5f)
                    r.espacoentrecarros = m->tamanho * 1.5f;

                for (int c = r.contagemcarros - 1; c >= 0; c--) {
                    Carro *nc = malloc(sizeof(Carro));
                    if (!nc) continue;
                    nc->indice = c;
                    nc->proximo = r.listaCarros;
                    r.listaCarros = nc;
                }
            }
        }

        m->faixas[i] = r;
        m->mapaTerreno[i][0] = (r.tipo == estrada);
        m->mapaTerreno[i][1] = (r.temcarro ? 1 : 0);
        m->quantidadelinhas++;
    }

    printf("Mundo criado com %d linhas\n", m->quantidadelinhas);
}

static float Espacamento(const Mundo *m, const Rua *r) {
    float e = r->espacoentrecarros;
    if (e <= 0) e = m->tamanho * 1.2f;
    if (m->dificuldadeatual > 1.0f) {
        e /= m->dificuldadeatual;
        if (e < m->tamanho * 1.2f) e = m->tamanho * 1.2f;
    }
    return e;
}

void atualizar_mundo(Mundo *m, float dt, int largura, float dificuldade) {
    if (!m || !m->faixas) return;
    if (dt > 0.05f) dt = 0.05f;
    m->dificuldadeatual = dificuldade;

    for (int i = 0; i < m->quantidadelinhas; i++) {
        Rua *r = &m->faixas[i];
        if (r->tipo != estrada || !r->temcarro || r->contagemcarros <= 0) continue;

        float esp = Espacamento(m, r);
        r->posicaocarroX += r->velocidade * dificuldade * dt;
        float comboio = (r->contagemcarros - 1) * esp + m->tamanho * 2.0f;

        if (r->velocidade > 0 && r->posicaocarroX > (float)largura + comboio)
            r->posicaocarroX = -comboio;
        else if (r->velocidade < 0 && r->posicaocarroX < -comboio)
            r->posicaocarroX = (float)largura + comboio;
    }
}

void planodefundo(const Mundo *m, Vector2 cam) {
    if (!m || !m->faixas) return;
    const int W = 1200, H = 720;

    for (int i = 0; i < m->quantidadelinhas; i++) {
        const Rua *r = &m->faixas[i];
        Rectangle dr = r->rect;
        dr.y -= cam.y;
        if (dr.y + dr.height < 0 || dr.y > H) continue;

        DrawRectangleRec(dr, r->cor);

        if (m->mapaTerreno[i][0]) {
            for (int x = 40; x < W; x += 80)
                DrawRectangle(x, dr.y + dr.height / 2 - 1, 30, 2, YELLOW);

            if (m->mapaTerreno[i][1] && r->contagemcarros > 0) {
                float esp = Espacamento(m, r);
                for (Carro *c = r->listaCarros; c; c = c->proximo) {
                    float x = r->posicaocarroX + c->indice * esp;
                    if (x < -m->tamanho * 3 || x > W + m->tamanho * 3) continue;

                    Rectangle car = { x, dr.y + 2, m->tamanho * 1.8f, m->tamanho - 4 };
                    Color cores[] = { RED, BLUE, GREEN, PURPLE, ORANGE, DARKGRAY };
                    DrawRectangleRec(car, cores[i % 6]);
                    DrawRectangle(car.x + 5, car.y + 5, car.width - 10, 8,
                                  (Color){200, 200, 200, 255});
                }
            }
        } else {
            for (int x = 30; x < W; x += 90)
                if (((i * 37 + x) % 100) > 60)
                    DrawRectangle(x, dr.y + dr.height - 6, 2, 6,
                                  (Color){40, 120, 40, 255});
        }
    }
}

bool checarcolisao(const Mundo *m, Rectangle player) {
    if (!m || !m->faixas) return false;

    for (int i = 0; i < m->quantidadelinhas; i++) {
        const Rua *r = &m->faixas[i];
        if (!CheckCollisionRecs(player, r->rect)) continue;
        if (!m->mapaTerreno[i][0] || !m->mapaTerreno[i][1] || r->contagemcarros <= 0) continue;

        float esp = Espacamento(m, r);
        for (Carro *c = r->listaCarros; c; c = c->proximo) {
            float x = r->posicaocarroX + c->indice * esp;
            Rectangle car = { x, r->rect.y + 2, m->tamanho * 1.8f, m->tamanho - 4 };
            if (CheckCollisionRecs(player, car)) return true;
        }
    }
    return false;
}
