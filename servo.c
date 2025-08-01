#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "servo.h"

uint16_t inclinacao(float angulo) {
    // Converte o ângulo em duração de pulso (em milissegundos)
    float pulso = 0.5f + (angulo / 180.0f) * 2.0f; 

    // Converte duração de pulso em valor proporcional ao ciclo de 20ms (50Hz)
    return (uint16_t)((pulso / 20.0f) * 20000.0f); 
}


void inicializar_pwm_servo() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 20000);
    pwm_init(slice_num, &config, true);
}

float mover_servo_por_lum(uint16_t lum) {
    // Mapeia valores de luminosidade para ângulos do servo
    float angulo;
    if (lum < 1000)
        angulo = 0;
    else if (lum < 3000)
        angulo = 45;
    else if (lum < 5000)
        angulo = 90;
    else if (lum < 7000)
        angulo = 135;
    else
        angulo = 180;
    // Aplica sinal PWM correspondente ao ângulo calculado
    pwm_set_gpio_level(SERVO_PIN, inclinacao(angulo));
    // Exibe no terminal o ângulo aplicado
    printf("Servo ajustado para %.0f° com base em %d lum\n", angulo, lum);
    return angulo;
}
