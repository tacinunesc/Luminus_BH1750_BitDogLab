// Bibliotecas padrão e específicas do Raspberry Pi Pico
#include <stdio.h>                  // Entrada e saída padrão
#include "pico/stdlib.h"           // Funções básicas do Pico (GPIO, tempo, etc.)
#include "hardware/i2c.h"          // Comunicação I2C
#include "bh1750.h"                // Driver do sensor de luminosidade BH1750
#include "inc/ssd1306.h"           // Driver do display OLED SSD1306
#include "inc/ssd1306_fonts.h"     // Fontes para o display OLED
#include "servo.h"                 // Controle do servo motor

// Definições dos pinos I2C
#define I2C_PORT i2c0              // Porta I2C utilizada
#define I2C_SDA 0                  // Pino SDA (dados)
#define I2C_SCL 1                  // Pino SCL (clock)

// Definições dos pinos dos LEDs
#define LED_AZUL 12
#define LED_VERDE 11
#define LED_VERMELHO 13

// Inicializa os pinos dos LEDs como saída
void configurar_leds() {
    gpio_init(LED_AZUL); gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_VERDE); gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO); gpio_set_dir(LED_VERMELHO, GPIO_OUT);
}

// Desliga todos os LEDs
void desligar_leds() {
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_VERMELHO, 0);
}

// Liga o LED correspondente à faixa de luminosidade
void controlar_leds(uint16_t lum) {
    desligar_leds();  // Garante que apenas um LED fique aceso
    if (lum < 2500)
        gpio_put(LED_AZUL, 1);     // Baixa luminosidade
    else if (lum < 4500)
        gpio_put(LED_VERDE, 1);    // Média luminosidade
    else
        gpio_put(LED_VERMELHO, 1); // Alta luminosidade
}

int main() {
    // Inicializa comunicação USB para debug
    stdio_init_all();
    while (!stdio_usb_connected()) sleep_ms(100); // Aguarda conexão USB
    printf("Sistema iniciado.\n");

    // Inicializa comunicação I2C
    i2c_init(I2C_PORT, 100000); // Velocidade de 100kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA); 
    gpio_pull_up(I2C_SCL);
    bh1750_init(I2C_PORT); // Inicializa sensor BH1750

    // Inicializa display OLED
    ssd1306_Init();
    ssd1306_Fill(Black);         // Limpa a tela
    ssd1306_UpdateScreen();      // Atualiza o display

    // Inicializa LEDs e servo motor
    configurar_leds();
    desligar_leds();
    inicializar_pwm_servo(); //Inicializa o servo motor

    // Loop principal
    while (1) {
        float lum_f = bh1750_read_lux(I2C_PORT); // Lê luminosidade em lux
        float angulo = -1;                       // Valor inicial do ângulo
        char linha1[32], linha2[32], linha3[32]; // Buffers para texto do display

        printf("Display: Servo: %.0f graus\n", angulo);

        if (lum_f >= 0) {
            uint16_t lum = (uint16_t)lum_f;      // Converte para inteiro
            printf("Luminosidade: %u lum\n", lum);
            controlar_leds(lum);                 // Atualiza LEDs
            angulo = mover_servo_por_lum(lum);   // Move servo conforme luminosidade
            printf("Ângulo calculado: %.0f graus\n", angulo);
        } else {
            printf("Erro ao ler luminosidade\n");
            desligar_leds(); // desliga LEDs em caso de erro
        }

        // Atualiza display OLED com informações
        ssd1306_Fill(Black); // Limpa tela
        ssd1306_SetCursor(0, 0);
        snprintf(linha1, sizeof(linha1), "MONITOR L-S");
        ssd1306_WriteString(linha1, Font_7x10, White);

        ssd1306_SetCursor(0, 12);
        snprintf(linha2, sizeof(linha2), "Lum: %.1f lum", lum_f);
        ssd1306_WriteString(linha2, Font_7x10, White);

        ssd1306_SetCursor(0, 24);
        snprintf(linha3, sizeof(linha3), "Servo: %.0f graus", angulo);
        ssd1306_WriteString(linha3, Font_7x10, White);
        ssd1306_UpdateScreen(); // Aplica as mudanças na tela

        sleep_ms(1000); // Aguarda 1 segundo antes da próxima leitura
    }

    return 0;
}
