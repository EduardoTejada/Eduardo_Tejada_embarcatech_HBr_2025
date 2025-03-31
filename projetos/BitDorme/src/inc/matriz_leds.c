#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "matriz_leds.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define MATRIX_PIN 7

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;


// Constantes usadas no código
const npLED_t ALL_RED[5][5] = {
  {RED, RED, RED, RED, RED},
  {RED, RED, RED, RED, RED},
  {RED, RED, RED, RED, RED},
  {RED, RED, RED, RED, RED},
  {RED, RED, RED, RED, RED}
};

const npLED_t CORRECT[5][5] = {
  {BLACK, BLACK, BLACK, BLACK, GREEN},
  {BLACK, BLACK, BLACK, GREEN, BLACK},
  {GREEN, BLACK, GREEN, BLACK, BLACK},
  {BLACK, GREEN, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK}
};

const npLED_t NOTHING[5][5] = {
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK}
};

const npLED_t UP_ARROW[5][5] = {
  {BLACK, BLACK, GREEN, BLACK, BLACK},
  {BLACK, GREEN, BLACK, GREEN, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK}
};

const npLED_t UP_INDICATOR[5][5] = {
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, GREEN, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK}
};

const npLED_t DOWN_ARROW[5][5] = {
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, YELLOW, BLACK, YELLOW, BLACK},
  {BLACK, BLACK, YELLOW, BLACK, BLACK}
};

const npLED_t DOWN_INDICATOR[5][5] = {
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, YELLOW, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK}
};

const npLED_t RIGHT_ARROW[5][5] = {
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLUE, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLUE},
  {BLACK, BLACK, BLACK, BLUE, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK}
};

const npLED_t RIGHT_INDICATOR[5][5] = {
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLUE, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK}
};

const npLED_t LEFT_ARROW[5][5] = {
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, MAGENTA, BLACK, BLACK, BLACK},
  {MAGENTA, BLACK, BLACK, BLACK, BLACK},
  {BLACK, MAGENTA, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK}
};

const npLED_t LEFT_INDICATOR[5][5] = {
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, MAGENTA, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK}
};

const npLED_t JOYSTICK_ARROW[5][5] = {
  {BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLUE, BLACK},
  {BLUE, BLACK, BLUE, BLACK, BLACK},
  {BLUE, BLUE, BLACK, BLACK, BLACK},
  {BLUE, BLUE, BLUE, BLACK, BLACK}
};

const npLED_t (*lookup_table_arrow[4])[5] = {
  UP_ARROW, DOWN_ARROW, RIGHT_ARROW, LEFT_ARROW
};

const npLED_t (*lookup_table_indicator[4])[5] = {
  UP_INDICATOR, DOWN_INDICATOR, RIGHT_INDICATOR, LEFT_INDICATOR
};

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit() {

  // Cria programa PIO.
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;

  // Toma posse de uma máquina PIO.
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
  }

  // Inicia programa na máquina PIO obtida.
  ws2818b_program_init(np_pio, sm, offset, MATRIX_PIN, 800000.f);

  // Limpa buffer de pixels.
  for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

/**
 * Limpa o buffer de pixels.
 */
void npClear() {
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite() {
  // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
  for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

/**
 * Recebe uma matriz 5x5 e mostra ela na ordem correta na matriz de LEDs.
 */
void npMatrix(const npLED_t matrix[5][5]) {
  for(int i = 0; i < 5; i++)
    leds[i] = matrix[4][4-i];
  
  for(int i = 0; i < 5; i++)
    leds[i+5] = matrix[3][i];

  for(int i = 0; i < 5; i++)
    leds[i+10] = matrix[2][4-i];

  for(int i = 0; i < 5; i++)
    leds[i+15] = matrix[1][i];

  for(int i = 0; i < 5; i++)
    leds[i+20] = matrix[0][4-i];

  npWrite(); // Atualiza os LEDs
}

/**
 * Mostra a flecha no jogo da memória e faz os delays
 */
void show_arrow(int which_arrow){
  npMatrix(lookup_table_arrow[which_arrow]);
  sleep_ms(500);
  npMatrix(NOTHING);
  sleep_ms(200);
}