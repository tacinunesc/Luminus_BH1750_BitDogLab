#ifndef SERVO_H
#define SERVO_H

#include "pico/stdlib.h"

#define SERVO_PIN 2  // Altere conforme o pino físico conectado ao servo

void inicializar_pwm_servo(void);
float mover_servo_por_lum(uint16_t lum);
uint16_t inclinacao(float angulo);
void mover_servo(float angulo);
#endif

