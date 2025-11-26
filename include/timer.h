#include <stdbool.h>
#ifndef TIMER_H
#define TIMER_H

typedef struct {
    float timeLeft;
    bool running;
} timer;

void passartempo(timer *t, float frametime);
void resettempo(timer *t, float seconds);
bool tempoesgotado(const timer *t);
#endif