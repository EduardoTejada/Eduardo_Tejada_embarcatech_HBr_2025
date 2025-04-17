#include "pico/stdlib.h"    // Biblioteca padrão para controle de GPIO, temporização e comunicação serial.
#include "hardware/timer.h" // Biblioteca para gerenciamento de temporizadores de hardware.
#include "hardware/irq.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "inc/Display_Oled.h"
#include <stdio.h>          // Biblioteca padrão de entrada e saída para uso de funções como printf.

// Definição dos pinos para o LED e o botão
#define BUTTON_PIN_A 5 // Define o pino GPIO 5 para ler o estado do botão.
#define BUTTON_PIN_B 6 // Define o pino GPIO 6 para ler o estado do botão.

#define DEBOUNCE_TIME_US 50000  // Tempo de debounce de 50 ms para os botões

// Variáveis globais voláteis para sincronização com interrupções
volatile int button_press_count_a = 0; // Contador de pressões do botão.
volatile int button_press_count_b = 0; // Contador de pressões do botão.

volatile int contagem = 9;
volatile int contagem_botao_b = 0;
volatile bool contagem_congelada = false;
volatile bool mudou = false;

absolute_time_t last_press_time_a;
absolute_time_t last_press_time_b;

/* Cabeçalho das funções utilizadas */
static void gpio_irq_handler(uint gpio, uint32_t events);
int64_t countdown_callback(alarm_id_t id, void *user_data);

alarm_id_t alarm_id;

int main() {
    stdio_init_all(); // Inicializa a comunicação serial para permitir o uso de printf para depuração.

    oled_init();
    oled_clear();

    // Configura o pino do botão A como entrada com resistor de pull-up interno.
    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A); // Habilita o resistor pull-up interno para evitar leituras incorretas.

    // Configura o pino do botão B como entrada com resistor de pull-up interno.
    gpio_init(BUTTON_PIN_B);
    gpio_set_dir(BUTTON_PIN_B, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_B); // Habilita o resistor pull-up interno para evitar leituras incorretas.

    /* Configura interrupções para ambas bordas nos dois botões */
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    alarm_id = add_alarm_in_ms(1000, countdown_callback, NULL, false);
    
    last_press_time_a = get_absolute_time();
    last_press_time_b = get_absolute_time();

    char cont_str[3], cont_botao_str[3];
    // O loop principal permanece vazio, pois o controle do LED e do botão é gerenciado pelo temporizador.
    while (true) {
        printf("%d %d\n", contagem, contagem_botao_b);
        sprintf(cont_str, "%2d", contagem);
        sprintf(cont_botao_str, "%2d", contagem_botao_b);
        
        if(mudou){
          oled_clear();
          mudou = false;
        }
        oled_text(0, 0, cont_str);
        oled_text(0, 20, cont_botao_str);
        oled_show();

        sleep_ms(100);
        tight_loop_contents(); // Função que otimiza o loop vazio para evitar consumo excessivo de CPU.
    }
}

// Essa função é chamada pelo temporizador quando o alarme expira.
int64_t countdown_callback(alarm_id_t id, void *user_data) {
    if(contagem > 1){
      contagem -= 1;
      mudou = true;
    } else if(contagem == 1){
      contagem -= 1;
      mudou = true;
      contagem_congelada = true;
      return 0;
    }
    return 1000000; // Retorna 0 para não repetir o alarme.
}


/* Função de tratamento de interrupção dos botões */
void gpio_irq_handler(uint gpio, uint32_t events){
    absolute_time_t now = get_absolute_time();

    // Verifica se o botão A foi pressionado
    if (gpio == BUTTON_PIN_A) {
        if (absolute_time_diff_us(last_press_time_a, now) > DEBOUNCE_TIME_US) {
            last_press_time_a = now;  // Atualiza o tempo da última ativação

            if(button_press_count_a == 0){
              mudou = true;
              contagem = 9;
              contagem_botao_b = 0;
              contagem_congelada = false;
              button_press_count_a++;   // Incrementa o contador de pressões
              cancel_alarm(alarm_id);
              alarm_id = add_alarm_in_ms(1000, countdown_callback, NULL, false);
            }
            else{
              button_press_count_a = 0;
            }
        }
    }

    // Verifica se o botão B foi pressionado
    if (gpio == BUTTON_PIN_B) {
        if (absolute_time_diff_us(last_press_time_b, now) > DEBOUNCE_TIME_US) {
            last_press_time_b = now;  // Atualiza o tempo da última ativação
            
            if(button_press_count_b == 0 && !contagem_congelada){
              mudou = true;
              contagem_botao_b++;
              button_press_count_b++;   // Incrementa o contador de pressões
            }
            else button_press_count_b = 0;
        }
    }
}
