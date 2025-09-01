// Bibliotecas padrão e específicas do Raspberry Pi Pico
#include <stdio.h>                  // Entrada e saída padrão
#include "pico/stdlib.h"           // Funções básicas do Pico 
#include "hardware/pwm.h"          // Controle de PWM 
#include "servo.h"                 // Relacionadas ao servo
#include <math.h>                  // Biblioteca matemática 

// Função para mover o servo para um ângulo específico
void mover_servo(float angulo) {
    // Converte o ângulo (0° a 180°) para um pulso PWM entre 0.5ms e 2.5ms
    float pulso = 0.5f + (angulo / 180.0f) * 2.0f;

    // Converte o pulso para nível PWM (baseado em período de 20ms e resolução de 20000)
    uint16_t nivel_pwm = (uint16_t)((pulso / 20.0f) * 20000.0f);

    // Define o nível PWM no pino do servo
    pwm_set_gpio_level(SERVO_PIN, nivel_pwm);
} 

// Armazena o último ângulo usado para evitar movimentos repetidos
static float angulo_anterior = -1;

// Converte um ângulo para o valor PWM correspondente
uint16_t inclinacao(float angulo) {
    float pulso = 0.5f + (angulo / 180.0f) * 2.0f;
    return (uint16_t)((pulso / 20.0f) * 20000.0f);
}

// Inicializa o PWM no pino do servo
void inicializar_pwm_servo() {
    // Define a função do pino como PWM
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);

   
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);

    // Configurações padrão de PWM
    pwm_config config = pwm_get_default_config();

    // Define o divisor de clock para ajustar a frequência
    pwm_config_set_clkdiv(&config, 125.0f);

    // Define o valor de wrap (resolução do ciclo PWM)
    pwm_config_set_wrap(&config, 20000);

    // Inicializa o PWM com a configuração e ativa o slice
    pwm_init(slice_num, &config, true);
    pwm_set_enabled(slice_num, true);
}

// Move o servo com base na luminosidade medida
float mover_servo_por_lum(uint16_t lum) {
    float pulso_ms = 1.5f;  // Pulso padrão de 1.5ms (posição central)
    float angulo;

    // Determina a faixa de luminosidade
    int faixa;
    if (lum < 100)
        faixa = 0;
    else if (lum < 1000)
        faixa = 1;
    else if (lum < 2000)
        faixa = 2;
    else if (lum < 3000)
        faixa = 3;
    else if(lum < 4000)
        faixa = 4;
    else if(lum < 5000)
        faixa = 5;
    else if(lum < 6000)
        faixa = 6;
    else if(lum < 7000)
        faixa = 7;
    else if(lum < 8000)
        faixa = 8;
    else if(lum < 9000)
        faixa = 9;
    else if(lum < 10000)
        faixa = 10;

    // Associa cada faixa a um ângulo específico
    switch (faixa) {
        case 0: angulo = 0; break;
        case 1: angulo = 15; break;
        case 2: angulo = 30; break;
        case 3: angulo = 45; break;
        case 4: angulo = 55; break;
        case 5: angulo = 75; break;
        case 6: angulo = 90; break;
        case 7: angulo = 110; break;
        case 8: angulo = 135; break;
        case 9: angulo = 160; break;
        case 10: angulo = 180; break;
    }

    // Se o ângulo mudou, realiza movimento suave até o novo valor
    if (angulo != angulo_anterior) {
        float passo = (angulo > angulo_anterior) ? 1 : -1;
        for (float a = angulo_anterior + passo; a != angulo + passo; a += passo) {
            pwm_set_gpio_level(SERVO_PIN, inclinacao(a));
            sleep_ms(10);  
        }
        angulo_anterior = angulo;
    }

    // Define o nível PWM final com base no pulso padrão
    uint16_t nivel_pwm = (uint16_t)((pulso_ms / 20.0f) * 20000.0f);
    pwm_set_gpio_level(SERVO_PIN, nivel_pwm);

    return angulo;  
}
