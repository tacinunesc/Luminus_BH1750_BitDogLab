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
   float pulso_ms;
   float angulo;

   if (lum < 1000)
        angulo = 0;
        pulso_ms = 1.5f;
    if (lum < 3000)
        angulo = 45;
        pulso_ms =  1.5f;
    if (lum < 5000)
        angulo = 90;
        pulso_ms = 1.5f;
    if (lum < 7000)
        angulo = 135;
        pulso_ms = 1.5f;
    if (lum >7000)
         angulo = 180;
         pulso_ms = 1.5f;

    if (angulo != angulo_anterior) {
        float passo = (angulo > angulo_anterior) ? 1 : -1;
        for (float a = angulo_anterior + passo; a != angulo + passo; a += passo) {
            pwm_set_gpio_level(SERVO_PIN, inclinacao(a));
            sleep_ms(20);  // tempo entre passos
        }
        angulo_anterior = angulo;
    
        uint16_t nivel_pwm = (uint16_t)((pulso_ms / 20.0f) * 20000.0f);
    pwm_set_gpio_level(SERVO_PIN, nivel_pwm);

        printf("Servo suavemente ajustado para %.0f° com base em %d lum\n", angulo, lum);
    } else {
        printf("Luminosidade estável (%d lum), servo mantido em %.0f°\n", lum, angulo);
    }

    return angulo;
    
}