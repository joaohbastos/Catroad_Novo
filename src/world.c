#include "world.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

void liberarcarro(Carro *c) {
    while (c) {
        Carro *n = c->proximo;
        free(c);
        c = n;
    }
}

void liberarmundo(Mundo *m) {
    if (m->faixas) {
        for (int i = 0; i < m->quantidadelinhas; i++)
            liberarcarro(m->faixas[i].listaCarros);
        free(m->faixas);
        m->faixas = NULL;
    }
    if (m->mapa) {
        for (int i = 0; i < m->capacidade; i++)
            free(m->mapa[i]);
        free(m->mapa);
        m->mapa = NULL;
    }
    m->quantidadelinhas = m->capacidade = m->colunasmatriz = 0;
}

void criarmundo(Mundo *m, int largura, int altura, float tamanho) {
    liberarmundo(m);
    m->tamanho = tamanho;
    m->dificuldadeatual = 1.0f;
    m->capacidade = MAXFAIXAS;
    m->colunasmatriz = 2;
    m->faixas = malloc(sizeof(Rua) * m->capacidade);
    m->mapa = malloc(sizeof(int *) * m->capacidade);
    for (int i = 0; i < m->capacidade; i++) {
        m->mapa[i] = malloc(sizeof(int) * m->colunasmatriz);
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
                    r.velocidade = (float)GetRandomValue(200, 250);
                } else {
                    r.posicaocarroX = (float)largura + tamanho * (float)GetRandomValue(2, 4);
                    r.velocidade = -(float)GetRandomValue(200, 250);
                }
                r.contagemcarros = GetRandomValue(1, 3);
                r.espacoentrecarros = (float)GetRandomValue(300, 500);
                
                if (r.espacoentrecarros < m->tamanho * 2.5f) {
                    r.espacoentrecarros = m->tamanho * 2.5f;
                }
                
                for (int c = r.contagemcarros - 1; c >= 0; c--) {
                    Carro *nc = malloc(sizeof(Carro));
                    nc->indice = c;
                    nc->proximo = r.listaCarros;
                    r.listaCarros = nc;
                }
            }
        }
        m->faixas[i] = r;
        if (r.tipo == estrada) {
            m->mapa[i][0] = 1;
        } else {
            m->mapa[i][0] = 0;
        }
        if (r.temcarro) {
            m->mapa[i][1] = 1;
        } else {
            m->mapa[i][1] = 0;
        }
        m->quantidadelinhas++;
    }
}

float Espacamento(const Mundo *m, const Rua *r) {
    float e = r->espacoentrecarros;
    if (e <= 0) e = m->tamanho * 1.2f;
    if (m->dificuldadeatual > 1.0f) {
        e /= m->dificuldadeatual;
        if (e < m->tamanho * 1.2f) e = m->tamanho * 1.2f;
    }
    return e;
}

void atualizar_mundo(Mundo *m, float frametime, int largura, float dificuldade) {
    m->dificuldadeatual = dificuldade;
    for (int i = 0; i < m->quantidadelinhas; i++) {
        Rua *r = &m->faixas[i];
        float esp = Espacamento(m, r);
        r->posicaocarroX += r->velocidade * dificuldade * frametime;
        float comboio = (r->contagemcarros - 1) * esp + m->tamanho * 2.0f;
        if (r->velocidade > 0 && r->posicaocarroX > (float)largura + comboio)
            r->posicaocarroX = -comboio;
        else if (r->velocidade < 0 && r->posicaocarroX < -comboio)
            r->posicaocarroX = (float)largura + comboio;
    }
}

void planodefundo(const Mundo *m, Vector2 cam) {
    const int W = 1200;
    const int H = 720;
    for (int i = 0; i < m->quantidadelinhas; i++) {
        const Rua *r = &m->faixas[i];
        Rectangle dr = r->rect;
        dr.y -= cam.y;
        if (dr.y + dr.altura < 0 || dr.y > H) continue;

        DrawRectangleRec(dr, r->cor);

        if (m->mapa[i][0]) {
            for (int x = 40; x < W; x += 80)
                DrawRectangle(x, dr.y + dr.altura / 2 - 1, 30, 2, YELLOW);

            if (m->mapa[i][1] && r->contagemcarros > 0) {
                float esp = Espacamento(m, r);
                for (Carro *c = r->listaCarros; c; c = c->proximo) {
                    float x = r->posicaocarroX + c->indice * esp;
                    if (x < -m->tamanho * 3 || x > W + m->tamanho * 3) continue;

                    Rectangle car = { x, dr.y + 2, m->tamanho * 1.8f, m->tamanho - 4 };
                    Color cores[] = { RED, BLUE, GREEN, PURPLE, ORANGE, DARKGRAY };
                    DrawRectangleRec(car, cores[i % 6]);
                    DrawRectangle(car.x + 5, car.y + 5, car.largura - 10, 8,
                                  (Color){200, 200, 200, 255});
                }
            }
        } else {
            for (int x = 30; x < W; x += 90)
                if (((i * 37 + x) % 100) > 60)
                    DrawRectangle(x, dr.y + dr.altura - 6, 2, 6,
                                  (Color){40, 120, 40, 255});
        }
    }
}

bool checarcolisao(const Mundo *m, Rectangle player) {
    for (int i = 0; i < m->quantidadelinhas; i++) {
        const Rua *r = &m->faixas[i];
        
        if (!CheckCollisionRecs(player, r->rect)) continue;
        if (!m->mapa[i][0] || !m->mapa[i][1] || r->contagemcarros <= 0) continue;

        float esp = Espacamento(m, r);
        for (Carro *c = r->listaCarros; c; c = c->proximo) {
            float x = r->posicaocarroX + c->indice * esp;
            Rectangle car = { x, r->rect.y + 2, m->tamanho * 1.8f, m->tamanho - 4 };
            if (CheckCollisionRecs(player, car)) return true;
        }
    }
    return false;
}