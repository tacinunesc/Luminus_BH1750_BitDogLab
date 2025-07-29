# Sensor de Luminosidade BH1750 + Servo Motor atuando com Pico W (BitDogLab)
![Linguagem](https://img.shields.io/badge/Linguagem-C-blue.svg)
![Plataforma](https://img.shields.io/badge/Plataforma-Raspberry%20Pi%20Pico-purple.svg)
![Sensor](https://img.shields.io/badge/Sensor-BH1750-yellow.svg)
![Sensor](https://img.shields.io/badge/Servo-Motor-green.svg)

## 🧠 Sobre o Projeto
Este código foi desenvolvido para um sistema de monitoramento de luminosidade com o Raspberry Pi Pico, utilizando os seguintes componentes:

Sensor BH1750 para medir luminosidade via I2C

Display OLED SSD1306 para exibir informações

LEDs RGB para indicar níveis de luz

Servo motor para atuar conforme a intensidade luminosa

## 🛠️ Estrutura do projeto
- luminus.c – Programa principal em C que faz leitura luminosidade do local, com base nesta informação utiliza o servo motor girar no especificado no código e essa informação é exibida no porta serial e no visor oled da BitDogLab
- bh1750.c - Onde fica as definições do sensor de luminosidade
- Pasta inc - Onde esta localizada as informações da oled
- CMakeLists.txt – Configuração do build usando o Pico SDK

- ## 🔌 Requisitos
Hardware:

- Raspberry Pi Pico W
- Sensor de Luminosidade (BH1750)
- Servo Motor 9g
- Adaptador do Servo Motor

## ⚙️ Como usar
1- Clone o repositorio

2- Deploy no Pico W
 - Segure o botão BOOTSEL do Pico W e conecte-o ao PC
 - Clique no botão run no Vscode ou arraste o arquivo .u2 para dentro do disco removível que aparecer
 - O Pico irá reiniciar executando o firmware

## 🔧 Funcionamento do Código
O programa realiza as seguintes ações:

1. Inicialização dos periféricos
- Configura o barramento I2C0 (pinos GPIO 0 e 1)
- Inicializa o sensor BH1750 e o display OLED SSD1306
- Configura os pinos dos LEDs RGB
- Inicializa o sinal PWM para controle do servo motor
  
2. Leitura da luminosidade
A cada segundo, o sistema:

- Lê a intensidade de luz (em lux) com o sensor BH1750
- Exibe o valor no display OLED
- Acende um LED correspondente ao nível de luminosidade:

   * 🔵 Azul: baixa (<2500 lux)

   * 🟢 Verde: média (<4500 lux)

   * 🔴 Vermelho: alta (>5000 lux)

3. Controle do servo motor
- O ângulo do servo é ajustado de acordo com a luminosidade
- Com isso, enquando o sensor de luminosidade detecatar lux de 0 a 2500 o led azul será ativado e o servo motor irá girar, entre 3000 e 4500  o led verde será ativado e o servo irá parar de rodar, acima ou igual a 5000 lux o servo rodará em sentido contrário.

## 📦 Dependências

bh1750.h para o sensor de luz

ssd1306.h e ssd1306_fonts.h para o display OLED
