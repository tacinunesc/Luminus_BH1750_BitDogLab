#include <stdio.h>                    // Biblioteca padrão de entrada/saída
#include "pico/stdlib.h"             // Biblioteca padrão para Raspberry Pi Pico
#include "hardware/i2c.h"            // Biblioteca para comunicação I2C
#include "hardware/pwm.h"            // Biblioteca para controle PWM
#include "bh1750.h"                  // Biblioteca do sensor de luminosidade BH1750
#include "inc/ssd1306.h"             // Biblioteca do display OLED SSD1306
#include "inc/ssd1306_fonts.h"       // Biblioteca de fontes para o display OLED

// --- Definições de pinos e portas ---
#define I2C_PORT i2c0                 // Define uso do I2C0
#define I2C_SDA 0                     // Pino GPIO 0 como SDA
#define I2C_SCL 1                     // Pino GPIO 1 como SCL

#define LED_AZUL 12                   // LED azul no GPIO 12
#define LED_VERDE 11                 // LED verde no GPIO 11
#define LED_VERMELHO 13              // LED vermelho no GPIO 13

#define SERVO_PIN 2                  // Servo motor conectado ao GPIO 2

// --- Inicializa os LEDs como saída ---
void configurar_leds() {
    gpio_init(LED_AZUL); gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_VERDE); gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO); gpio_set_dir(LED_VERMELHO, GPIO_OUT);
}

// --- Desliga todos os LEDs ---
void desligar_leds() {
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_VERMELHO, 0);
}

// --- Ativa LED com base na luminosidade ---
void controlar_leds(uint16_t lum) {
    desligar_leds(); // Garante que apenas um LED esteja ligado por vez
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

// --- Calcula pulso PWM conforme ângulo desejado ---
uint16_t inclinacao(float angulo) {
    float pulso = 0.5f + (angulo / 180.0f) * 2.0f;     // Converte ângulo para duração de pulso
    return (uint16_t)((pulso / 20.0f) * 20000.0f);     // Escala para ciclo de 20ms
}

// --- Inicializa PWM para controle do servo ---
void inicializar_pwm_servo() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);       // Define função PWM no pino
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN); // Obtém número do "slice" PWM
    pwm_config config = pwm_get_default_config();      // Carrega configuração padrão
    pwm_config_set_clkdiv(&config, 125.0f);            // Define 1 us por tick
    pwm_config_set_wrap(&config, 20000);               // Define ciclo de 20ms (50Hz)
    pwm_init(slice_num, &config, true);                // Inicializa PWM com configuração
}

// --- Define ângulo do servo conforme valor de luminosidade ---
float mover_servo_por_lum(uint16_t lum) {
    float angulo;

    // Mapeia valor de luminosidade para ângulo
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

    // Aplica sinal PWM com ângulo correspondente
    pwm_set_gpio_level(SERVO_PIN, inclinacao(angulo));
    printf("Servo ajustado para %.0f° com base em %d lum\n", angulo, lum);
    return angulo;
}

// --- Função principal do programa ---
int main() {
    stdio_init_all();                               // Inicializa entrada/saída padrão
    while (!stdio_usb_connected()) sleep_ms(100);   // Aguarda conexão USB
    printf("Sistema iniciado.\n");

    // --- Inicialização do sensor BH1750 ---
    i2c_init(I2C_PORT, 100 * 1000);                 // Inicializa I2C a 100 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);      // Define função I2C nos pinos
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);                          // Ativa resistores de pull-up
    gpio_pull_up(I2C_SCL);
    bh1750_init(I2C_PORT);                          // Inicializa sensor BH1750

    // --- Inicialização do Display OLED ---
    ssd1306_Init();                                 // Inicializa display
    ssd1306_Fill(Black);                            // Limpa tela
    ssd1306_UpdateScreen();                         // Atualiza tela

    // --- Configura LEDs e Servo ---
    configurar_leds();
    desligar_leds();
    inicializar_pwm_servo();

    while (1) {
        float lum_f = bh1750_read_lux(I2C_PORT);    // Lê valor de luminosidade
        float angulo = 0;
        char buffer[32];

        // --- Verifica leitura válida ---
        if (lum_f >= 0) {
            uint16_t lum = (uint16_t)lum_f;         // Converte para inteiro
            printf("Luminosidade: %u lum\n", lum);
            controlar_leds(lum);                    // Ativa LED correspondente
            angulo = mover_servo_por_lum(lum);      // Ajusta servo
        } else {
            printf("Erro ao ler o sensor de luminosidade.\n");
            desligar_leds();                        // Desliga LEDs se leitura falhar
        }

        // --- Atualiza display OLED ---
        ssd1306_Fill(Black);                        // Limpa tela
        ssd1306_SetCursor(0, 0);                    // Posição inicial
        ssd1306_WriteString("MONITOR L-S", Font_7x10, White);  // Título

        snprintf(buffer, sizeof(buffer), "Lum: %.1f lum", lum_f); // Exibe luminosidade
        ssd1306_SetCursor(0, 12);
        ssd1306_WriteString(buffer, Font_7x10, White);

        snprintf(buffer, sizeof(buffer), "Servo: %.0f graus", angulo); // Exibe ângulo do servo
        ssd1306_SetCursor(0, 24);
        ssd1306_WriteString(buffer, Font_7x10, White);

        ssd1306_UpdateScreen();                     // Atualiza tela

        sleep_ms(1000);                             // Aguarda 1 segundo
    }

    return 0;                                       // Fim do programa
}
