#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "servo.h"

static float angulo_anterior = -1;

uint16_t inclinacao(float angulo) {
    float pulso = 0.5f + (angulo / 180.0f) * 2.0f;
    return (uint16_t)((pulso / 20.0f) * 20000.0f);
}

void inicializar_pwm_servo() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 20000);
    pwm_init(slice_num, &config, true);
    pwm_set_enabled(slice_num, true);
}

float mover_servo_por_lum(uint16_t lum) {
    float pulso_ms = 1.5f;
    float angulo = 0.0f;

    // Categoriza a faixa de luminosidade
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

    // Determina o ângulo com switch
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

    if (angulo != angulo_anterior) {
        float passo = (angulo > angulo_anterior) ? 1 : -1;
        for (float a = angulo_anterior + passo; a != angulo + passo; a += passo) {
            pwm_set_gpio_level(SERVO_PIN, inclinacao(a));
            sleep_ms(10);
        }
        angulo_anterior = angulo;
    }

    uint16_t nivel_pwm = (uint16_t)((pulso_ms / 20.0f) * 20000.0f);
    pwm_set_gpio_level(SERVO_PIN, nivel_pwm);

    return pulso_ms;
}
