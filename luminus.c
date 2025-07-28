#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "bh1750.h"
#include "inc/ssd1306.h"
#include "inc/ssd1306_fonts.h"

// --- Configurações ---
#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

#define LED_AZUL 12
#define LED_VERDE 11
#define LED_VERMELHO 13

#define SERVO_PIN 2   // servo no GPIO 2

// --- Funções auxiliares ---

void configurar_leds() {
    gpio_init(LED_AZUL); gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_VERDE); gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO); gpio_set_dir(LED_VERMELHO, GPIO_OUT);
}

void desligar_leds() {
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_VERMELHO, 0);
}

void controlar_leds(uint16_t lum) {
    desligar_leds();
    if (lum < 2500) {
        gpio_put(LED_AZUL, 1);
        printf("LED Azul (Lum < 2500)\n");
    } else if (lum < 4500) {
        gpio_put(LED_VERDE, 1);
        printf("LED Verde (Lum < 4500)\n");
    } else if (lum >= 5000) {
        gpio_put(LED_VERMELHO, 1);
        printf("LED Vermelho (Lum >= 5000)\n");
    }
}

// --- PWM Servo ---

uint16_t inclinacao(float angulo) {
    float pulso = 0.5f + (angulo / 180.0f) * 2.0f;
    return (uint16_t)((pulso / 20.0f) * 20000.0f);
}

void inicializar_pwm_servo() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f); // 1 us por tick
    pwm_config_set_wrap(&config, 20000);    // 20ms (50Hz)
    pwm_init(slice_num, &config, true);
}

float mover_servo_por_lum(uint16_t lum) {
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

    pwm_set_gpio_level(SERVO_PIN, inclinacao(angulo));
    printf("Servo ajustado para %.0f° com base em %d lum\n", angulo, lum);
    return angulo;
}

// --- Programa principal ---

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) sleep_ms(100);
    printf("Sistema iniciado.\n");

    // BH1750 (I2C0)
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    bh1750_init(I2C_PORT);

    // OLED
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    // LEDs e Servo
    configurar_leds();
    desligar_leds();
    inicializar_pwm_servo();

    while (1) {
        float lum_f = bh1750_read_lux(I2C_PORT);
        float angulo = 0;
        char buffer[32];

        if (lum_f >= 0) {
            uint16_t lum = (uint16_t)lum_f;
            printf("Luminosidade: %u lum\n", lum);
            controlar_leds(lum);
            angulo = mover_servo_por_lum(lum);
        } else {
            printf("Erro ao ler o sensor de luminosidade.\n");
            desligar_leds();
        }

        // Atualiza o display OLED
        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("MONITOR L-S", Font_7x10, White);

        snprintf(buffer, sizeof(buffer), "Lum: %.1f lum", lum_f);
        ssd1306_SetCursor(0, 12);
        ssd1306_WriteString(buffer, Font_7x10, White);

        snprintf(buffer, sizeof(buffer), "Servo: %.0f graus", angulo);
        ssd1306_SetCursor(0, 24);
        ssd1306_WriteString(buffer, Font_7x10, White);

        ssd1306_UpdateScreen();

        sleep_ms(1000);
    }

    return 0;
}