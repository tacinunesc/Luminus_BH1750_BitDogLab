#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bh1750.h"
#include "inc/ssd1306.h"
#include "inc/ssd1306_fonts.h"
#include "servo.h"

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

#define LED_AZUL 12
#define LED_VERDE 11
#define LED_VERMELHO 13

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
    if (lum < 2500)
        gpio_put(LED_AZUL, 1);
    else if (lum < 4500)
        gpio_put(LED_VERDE, 1);
    else
        gpio_put(LED_VERMELHO, 1);
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) sleep_ms(100);
    printf("Sistema iniciado.\n");

    i2c_init(I2C_PORT, 100000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    bh1750_init(I2C_PORT);


    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    configurar_leds();
    desligar_leds();
    inicializar_pwm_servo();

    while (1) {
        float lum_f = bh1750_read_lux(I2C_PORT);
        float angulo = -1;
        char linha1[32], linha2[32], linha3[32];
printf("Display: Servo: %.0f graus\n", angulo);

        if (lum_f >= 0) {
            uint16_t lum = (uint16_t)lum_f;
            printf("Luminosidade: %u lum\n", lum);
            controlar_leds(lum);
            angulo = mover_servo_por_lum(lum);
            printf("Ângulo calculado: %.0f graus\n", angulo);
        } else {
            printf("Erro ao ler luminosidade\n");
            desligar_leds();
        }

        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 0);
        snprintf(linha1, sizeof(linha1), "MONITOR L-S");
        ssd1306_WriteString(linha1, Font_7x10, White);

        ssd1306_SetCursor(0, 12);
        snprintf(linha2, sizeof(linha2), "Lum: %.1f lum", lum_f);
        ssd1306_WriteString(linha2, Font_7x10, White);

        ssd1306_SetCursor(0, 24);
        snprintf(linha3, sizeof(linha3), "Servo: %.0f graus", angulo);
        ssd1306_WriteString(linha3, Font_7x10, White);
        ssd1306_UpdateScreen();

        sleep_ms(1000);
    }

    return 0;
}
