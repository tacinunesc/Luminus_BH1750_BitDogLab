#include <stdio.h>                      // Entrada/saída
#include "pico/stdlib.h"               // Funções básicas da Pico
#include "hardware/i2c.h"              // Comunicação I2C
#include "bh1750.h"                    // Definições da leitura do sensor de luminosidade BH1750
#include "inc/ssd1306.h"               // Driver para display OLED SSD1306
#include "inc/ssd1306_fonts.h"         // Fontes para o display OLED
#include "servo.h"                     // Módulo com funções para controle do servo


// --- Definições das portas utilizadas pelo BH1750 no i2c0---
#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

//---Definições da portas GPIOs utilizadas pelos LEDs---
#define LED_AZUL 12
#define LED_VERDE 11
#define LED_VERMELHO 13

void configurar_leds() {
    // Inicializa os pinos dos LEDs como saída
    gpio_init(LED_AZUL); gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_VERDE); gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO); gpio_set_dir(LED_VERMELHO, GPIO_OUT);
}

void desligar_leds() {
    // Apaga todos os LEDs
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_VERMELHO, 0);
}

void controlar_leds(uint16_t lum) {
    // Acende um LED com base no nível de luminosidade
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

    // Inicialização do I2C e sensor BH1750
    i2c_init(I2C_PORT, 100000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    bh1750_init(I2C_PORT);

    // Inicialização do Display
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    configurar_leds();
    desligar_leds();
    inicializar_pwm_servo();  // <- vindo do módulo servo

        while (1) {
        float lum_f = bh1750_read_lux(I2C_PORT);    // Lê luminosidade 
        float angulo = 0;
        char buffer[32];

        if (lum_f >= 0) {
            uint16_t lum = (uint16_t)lum_f;
            printf("Luminosidade: %u lum\n", lum);
            controlar_leds(lum);                    // Ajusta LEDs
            angulo = mover_servo_por_lum(lum);      // Ajusta servo
        } else {
            printf("Erro ao ler luminosidade\n");
            desligar_leds();                        // Se houver erro apaga os LEDs
        }


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
