# Sensor de Luminosidade BH1750 + Servo Motor atuando com Pico W (BitDogLab)
![Linguagem](https://img.shields.io/badge/Linguagem-C-blue.svg)
![Plataforma](https://img.shields.io/badge/Plataforma-Raspberry%20Pi%20Pico-purple.svg)
![Sensor](https://img.shields.io/badge/Sensor-BH1750-yellow.svg)
![Sensor](https://img.shields.io/badge/Servo-Motor-green.svg)


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

3- Testar a aplicação
