
#ifndef SERVO_H
#define SERVO_H

#include "pico/stdlib.h"

// Define o pino GPIO conectado ao servo motor
//Altere este valor conforme o pino físico utilizado no seu projeto
#define SERVO_PIN 2

// Inicializa o PWM no pino do servo 
void inicializar_pwm_servo(void);

// 🌞 Move o servo com base em um valor de luminosidade 
// Retorna o ângulo correspondente ao valor de entrada
float mover_servo_por_lum(uint16_t lum);

//  Converte um ângulo (0° a 180°) 
uint16_t inclinacao(float angulo);

// Move o servo diretamente para um ângulo específico
void mover_servo(float angulo);

#endif

