#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

// Pino do servo motor no gpio 2 - conectado no i2c1
#define SERVO_PIN 2

// Inicializa o PWM no pino do servo
void inicializar_pwm_servo(void);

// Converte ângulo para valor de PWM
uint16_t inclinacao(float angulo);

// Ajusta ângulo do servo com base na luminosidade
float mover_servo_por_lum(uint16_t lum);

#endif // SERVO_H
