#ifndef TIMER_H
#define TIMER_H
#include <stdbool.h>
#include "raylib.h" 

typedef struct {
    float timeLeft;
    bool running;
} timer;

void Timer_Start(timer *t, float seconds);
void passartempo(timer *t, float dt);
void resettempo(timer *t, float seconds);
bool tempoesgotado(const timer *t);

#endif