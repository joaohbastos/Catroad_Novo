#include "timer.h"

void Timer_Start(timer *t, float segundos) {
    t->timeLeft = segundos;
    t->running = true;
}

void passartempo(timer *t, float frametime) {
    if (!t->running) return;
    t->timeLeft -= frametime;
    if (t->timeLeft < 0.0f) {
        t->timeLeft = 0.0f;
        t->running = false;
    }
}

void resettempo(timer *t, float segundos) {
    t->timeLeft = segundos;
    t->running = true;
}

bool tempoesgotado(const timer *t) {
    return !t->running || t->timeLeft <= 0.0f;
}