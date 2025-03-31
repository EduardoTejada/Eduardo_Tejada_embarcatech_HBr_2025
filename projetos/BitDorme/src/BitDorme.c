#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "inc/joystick.h"
#include "inc/Display_Oled.h"
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "inc/Buzzer.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "inc/matriz_leds.h"
#include "inc/microphone_dma.h"
#include "hardware/flash.h"
#include <time.h>

/* Definições de constantes */
#define FLASH_TARGET_OFFSET (256 * 1024)  // Offset de 256 KB na memória flash
#define DEBOUNCE_TIME_US 50000  // Tempo de debounce de 50 ms para os botões
#define MICROPHONE_SAMPLING_TIME 1000 // Tempo de amostragem do microfone: 1 segundo
#define LEVELS_MEMORY_GAME 6 // Número de níveis no jogo da memória

/* Definições dos pinos GPIO */
#define BUTTON_PIN_A 5 // Pino GPIO 5 para o botão A
#define BUTTON_PIN_B 6 // Pino GPIO 6 para o botão B
#define LED_PIN 11     // Pino GPIO 11 para o LED

/* Variáveis globais */
int state = 0; // Estado atual do sistema
volatile int button_press_count_a = 0; // Contador de pressões do botão A
volatile int button_press_count_b = 0; // Contador de pressões do botão B

/* Enumeração para as direções no jogo da memória */
enum Directions {
    UP = 0,
    DOWN,
    RIGHT,
    LEFT
};

/* Estrutura para o jogo da memória */
typedef struct {
    int substate; // Subestado do jogo
    int level;    // Nível atual do jogo
    int flecha;   // Posição atual na sequência
    enum Directions sequencia[LEVELS_MEMORY_GAME]; // Sequência de direções
} memoryGame;

/* Variáveis para temporização e alarmes */
struct repeating_timer timer;
alarm_id_t alarm_id;

/* Variáveis para debounce dos botões */
absolute_time_t last_press_time_a;
absolute_time_t last_press_time_b;

/* Contador para o monitoramento do sono */
volatile int counter = 0;

/* Cabeçalho das funções utilizadas */
void gpio_irq_handler(uint gpio, uint32_t events); // Função de tratamento de interrupção dos botões
bool repeating_timer_callback(struct repeating_timer *t); // Função de callback para o temporizador repetitivo a cada 1 segundo
void callback_despertador(); // Função de callback para o RTC (despertador)
int64_t alarm_game_callback(alarm_id_t id, void *user_data); // Função de callback para o alarme do jogo da memória
void save_data(uint8_t* data_sono); // Função para salvar dados na memória flash
void load_data(uint8_t* data_sono); // Função para carregar dados da memória flash
int selecaoHorario(int* horario, char* mensagem, bool voltar); // Função para selecionar horário
void telaInicial(); // Função para exibir a tela inicial
void selecionarHorarioAtual(int *horario); // Função para selecionar o horário atual
void selecionarHorarioParaTocar(int *horario); // Função para selecionar o horário do alarme
bool calculoTempo(int* horario_atual, int* horario_alarme, uint8_t* data_sono); // Função para calcular o tempo restante até o alarme
void confirmacaoAlarme(int* horario_atual, int* horario_alarme, bool mesmo_dia, uint8_t* data_sono); // Função para confirmar o alarme
void monitorandoSono(); // Função para monitorar o sono
void tocarAlarme(uint8_t* data_sono); // Função para tocar o alarme
int randint(int n); // Função para gerar números aleatórios
memoryGame* gameInit(); // Função para inicializar o jogo da memória
void jogoDaMemoria(memoryGame* game); // Função principal do jogo da memória
int resultadosMonitoramento(int pos_display, uint8_t* data_sono); // Função para exibir os resultados do monitoramento
void button_init(); // Função para inicializar os botões


/* Função principal */
int main() {
    stdio_init_all(); // Inicializa a comunicação serial

    // Inicializa os periféricos
    oled_init();
    oled_clear();
    joystick_init();
    button_init();
    buzzer_init();
    rtc_init();
    npInit();
    microphone_init();

    // Variaveis para o RTC
    char datetime_buf[256];
    char *datetime_str = &datetime_buf[0];

    // Variaveis para armazenar os horarios
    int horario_atual[2] = {0, 0};
    int horario_alarme[2] = {0, 0};

    uint8_t* data_sono = NULL; // Ponteiro para salvar o endereço do array dos dados
    
    srand(time(NULL)); // Inicializa o gerador de números aleatórios

    int display_pos = 0; // Pagina atual do grafico (cada pagina contem 127 dados/pixels)

    // Loop principal que roda a máquina de estados
    while (true) {
        switch (state) {
            case 0: // Tela inicial
                telaInicial();
                break;
            case 1: // Selecionar horário atual
                selecionarHorarioAtual(horario_atual);
                break;
            case 2: // Selecionar horário do alarme
                selecionarHorarioParaTocar(horario_alarme);
                break;
            case 3: // Calcular tempo até o alarme
                bool mesmo_dia = calculoTempo(horario_atual, horario_alarme, data_sono);
                break;
            case 4: // Confirmar alarme
                confirmacaoAlarme(horario_atual, horario_alarme, mesmo_dia, data_sono);
                break;
            case 5: // Monitorar sono
                monitorandoSono();
                break;
            case 6: // Tocar alarme
                tocarAlarme(data_sono);
                break;
            case 7: // Iniciar jogo da memória
                memoryGame* game = gameInit();
                break;
            case 8: // Jogar jogo da memória
                jogoDaMemoria(game);
                break;
            case 9: // Exibir resultados do monitoramento
                display_pos = resultadosMonitoramento(display_pos, data_sono);
                break;
        }
    }
    return 0;
}

/* Função de tratamento de interrupção dos botões */
void gpio_irq_handler(uint gpio, uint32_t events){
    absolute_time_t now = get_absolute_time();

    // Verifica se o botão A foi pressionado
    if (gpio == BUTTON_PIN_A) {
        if (absolute_time_diff_us(last_press_time_a, now) > DEBOUNCE_TIME_US) {
            last_press_time_a = now;  // Atualiza o tempo da última ativação
            button_press_count_a++;   // Incrementa o contador de pressões
        }
    }

    // Verifica se o botão B foi pressionado
    if (gpio == BUTTON_PIN_B) {
        if (absolute_time_diff_us(last_press_time_b, now) > DEBOUNCE_TIME_US) {
            last_press_time_b = now;  // Atualiza o tempo da última ativação
            button_press_count_b++;   // Incrementa o contador de pressões
        }
    }
}

/* Função de callback para o temporizador repetitivo a cada 1 segundo */
bool repeating_timer_callback(struct repeating_timer *t) {
    datetime_t rtc_info;
    rtc_get_datetime(&rtc_info); // Obtém o horário atual do RTC

    int micro_value = microphone_read(); // Lê o valor do microfone

    // Exibe o horário e o valor do microfone no console
    printf("%2d:%2d:%2d - %3d\n", rtc_info.hour, rtc_info.min, rtc_info.sec, micro_value);

    // Converte os valores para strings
    char hora_str[3];
    char minuto_str[3];
    char segundos_str[3];
    char microphone_str[3];

    uint8_t *data_sono = (uint8_t *)t->user_data;
    data_sono[counter] = micro_value; // Armazena o valor do microfone no array

    counter++; // Incrementa o contador de amostras

    // Formata as strings para exibição no display
    sprintf(hora_str, "%2d", rtc_info.hour);
    sprintf(minuto_str, "%2d", rtc_info.min);
    sprintf(segundos_str, "%2d", rtc_info.sec);
    sprintf(microphone_str, "%3d", micro_value);

    // Exibe as informações no display OLED
    oled_clear();
    oled_text(0, 0, "Monitoramento");
    oled_text(0, 15, hora_str);
    oled_text(25, 15, minuto_str);
    oled_text(50, 15, segundos_str);
    oled_text(0, 30, "Nivel");
    oled_text(40, 30, microphone_str);
    oled_show();

    return true; // Continua o temporizador
}


/* Função de callback para o RTC (despertador) */
void callback_despertador(){
    cancel_repeating_timer(&timer); // Cancela a coleta de dados
    state++; // Passa ao próximo estado
}

/* Função de callback para o alarme do jogo da memória */
int64_t alarm_game_callback(alarm_id_t id, void *user_data) {
    memoryGame* game = (memoryGame*)user_data;
    free(game); // Libera a memória do jogo
    state -= 2; // Volta para o estado anterior
}

/* Função para salvar dados na memória flash */
void save_data(uint8_t* data_sono) {
    // Apaga a região da flash
    flash_range_erase(FLASH_TARGET_OFFSET, sizeof(data_sono));

    // Escreve os dados na flash
    flash_range_program(FLASH_TARGET_OFFSET, (const uint8_t*)data_sono, sizeof(data_sono));
}

/* Função para carregar dados da memória flash */
void load_data(uint8_t* data_sono) {
    // Lê os dados da flash
    memcpy(data_sono, (void*)(XIP_BASE + FLASH_TARGET_OFFSET), sizeof(data_sono));
}

/* Função para selecionar horário */
int selecaoHorario(int* horario, char* mensagem, bool voltar) {
    char hora_str[3];
    char minuto_str[3];

    int posicao_joystick = 0; // Define se a hora ou o minuto seram alterados
    int posicao_joystick_display = 0; // Mostra o "cursor" no display

    int variacao = 0; // Salva a diferença na posição do joystick para somar no horário
    bool changed = true; // Indica se houve mudança no joystick
    
    button_press_count_a = 0;
    button_press_count_b = 0;

    // Converte os valores do horário para strings
    sprintf(hora_str, "%d", horario[0]);
    sprintf(minuto_str, "%d", horario[1]);

    // Loop para seleção do horário
    while (button_press_count_b == 0) {
        if (changed) { // Evita que o display fique piscando
            oled_clear();
            changed = false;
        }
        /*
        USE O JOYSTICK
        vv
        00:00
        B PARA CONFIRMAR
        A PARA VOLTAR
        */
        oled_text(0, 0, "Use o Joystick");
        oled_text(0, 23, mensagem);
        oled_text(posicao_joystick_display, 30, "vv");
        if (horario[0] < 10) { // Garante que não fique aparecendo somente um digito
            oled_text(0, 35, "0");
            oled_text(8, 35, hora_str);
        } else {
            oled_text(0, 35, hora_str);
        }
        if (horario[1] < 10) { // Garante que não fique aparecendo somente um digito
            oled_text(25, 35, "0");
            oled_text(33, 35, minuto_str);
        } else {
            oled_text(25, 35, minuto_str);
        }

        oled_text(0, 48, "B para confirmar");
        if (voltar) oled_text(0, 56, "A para voltar"); // Não é mostrado no estado de seleção do horário ATUAL pois é o "primeiro estado" de configuração
        oled_show();

        // Verifica a posição do joystick e muda entre 0 e 1 (hora e minuto) e ativa a flag "changed"
        if (get_joystick_horizontal_pos() == -2 && posicao_joystick != 0){
            posicao_joystick = 0;
            changed = true;
        }
        else if (get_joystick_horizontal_pos() == 2 && posicao_joystick != 1){
            posicao_joystick = 1;
            changed = true;
        }
        
        if (posicao_joystick != 0) posicao_joystick_display = 25; // Ajusta a posição do cursor "vv"
        else posicao_joystick_display = 0;

        // Ajusta o horário com base no joystick
        variacao = get_joystick_vertical_pos();
        horario[posicao_joystick] += variacao;

        // Limita os valores do horário
        if (horario[posicao_joystick] < 0) horario[posicao_joystick] = 0;
        if (horario[0] > 23) horario[0] = 23;
        if (horario[1] > 59) horario[1] = 59;

        // Atualiza as strings do horário
        sprintf(hora_str, "%d", horario[0]);
        sprintf(minuto_str, "%d", horario[1]);

        // Verifica se o botão A foi pressionado para voltar
        if (button_press_count_a > 0 && voltar) {
            sleep_ms(200);
            oled_clear();
            button_press_count_a = 0;
            button_press_count_b = 0;
            return 0; // Retorna 0 para voltar para o estado anterior em outra função
        }
        sleep_ms(170);
    }
    sleep_ms(200);
    oled_clear();
    button_press_count_a = 0;
    button_press_count_b = 0;
    return 1; // Retorna 1 para avançar para o próximo estado
}

/* Função para exibir a tela inicial */
void telaInicial() {
    oled_clear(); // Limpa o display
    button_press_count_a = 0;
    oled_draw(); // Escreve o desenho no display

    // Aguarda o botão A ser pressionado
    while (button_press_count_a == 0) {
        sleep_ms(1);
    }

    button_press_count_a = 0;
    state++; // Avança para o próximo estado
    oled_clear();
    oled_init(); // Necessário por conta da função de desenho
}

/* Função para selecionar o horário atual */
void selecionarHorarioAtual(int *horario) {
    selecaoHorario(horario, "Horario Atual", false);
    state++; // Avança o estado
}

/* Função para selecionar o horário do alarme */
void selecionarHorarioParaTocar(int *horario) {
    if (selecaoHorario(horario, "Horario Alarme", true))
        state++; // Avança o estado
    else
        state--; // Regride um estado
}

/* Função para calcular o tempo restante até o alarme */
bool calculoTempo(int* horario_atual, int* horario_alarme, uint8_t* data_sono) {
    int horario_faltando[2] = {0, 0}; // Diferença de horários
    char hora_faltando_str[3];
    char minuto_faltando_str[3];
    bool mesmo_dia = false; // Indica se o alarme irá tocar no mesmo dia ou no próximo dia

    // Calcula a diferença entre os horários
    if (horario_atual[0] > horario_alarme[0] || (horario_atual[0] == horario_alarme[0] && horario_atual[1] > horario_alarme[1])) {
        horario_faltando[1] = (23 - horario_atual[0]) * 60;
        horario_faltando[1] += 60 - horario_atual[1];
        horario_faltando[1] += horario_alarme[0] * 60 + horario_alarme[1];
    } else {
        horario_faltando[1] = (60 * horario_alarme[0] + horario_alarme[1]) - 60 * horario_atual[0] - horario_atual[1];
        mesmo_dia = true;
    }

    // Aloca memória para os dados do sono (baseado na quantidade de segundos que o usuário irá dormir)
    data_sono = (uint8_t*)calloc(60 * horario_alarme[1], sizeof(uint8_t));

    // Converte o tempo restante para horas e minutos
    horario_faltando[0] = horario_faltando[1] / 60;
    horario_faltando[1] -= horario_faltando[0] * 60;

    // Formata as strings para exibição
    sprintf(hora_faltando_str, "%d", horario_faltando[0]);
    sprintf(minuto_faltando_str, "%d", horario_faltando[1]);

    // Exibe o tempo restante no display
    oled_text(0, 0, "Tocara em");
    oled_text(0, 20, hora_faltando_str);
    oled_text(25, 20, "horas e");
    oled_text(0, 30, minuto_faltando_str);
    oled_text(25, 30, "minutos");
    oled_text(0, 48, "B para confirmar");
    oled_text(0, 56, "A para voltar");
    oled_show();

    state++; // Avança para o próximo estado
    return mesmo_dia; // Retorna para a função principal (evita o uso de variáveis globais)
}

/* Função para confirmar o alarme */
void confirmacaoAlarme(int* horario_atual, int* horario_alarme, bool mesmo_dia, uint8_t* data_sono) {
    if (button_press_count_a > 1) {
        state -= 2; // Volta para o estado de seleção da hora
        button_press_count_a = 0;
        button_press_count_b = 0;
        free(data_sono);
    }
    else if (button_press_count_b > 0) {
        button_press_count_b = 0;

        // Configura o horário atual no RTC (os únicos parâmetros que importam são horas, minutos e segundos)
        datetime_t t1 = {
            .year = 2025,
            .month = 01,
            .day = 01,
            .dotw = 0,
            .hour = horario_atual[0],
            .min = horario_atual[1],
            .sec = 00
        };

        rtc_set_datetime(&t1);
        sleep_us(64);

        // Configura o horário do alarme no RTC
        datetime_t t_alarme = {
            .year = 2025,
            .month = 01,
            .day = mesmo_dia ? 01 : 02,
            .dotw = 0,
            .hour = horario_alarme[0],
            .min = horario_alarme[1],
            .sec = 00
        };

        // Configura para despertar (gerar uma interrupção quando chegar na hora)
        rtc_set_alarm(&t_alarme, callback_despertador);

        // Inicia o temporizador para o monitoramento do sono
        add_repeating_timer_ms(MICROPHONE_SAMPLING_TIME, repeating_timer_callback, data_sono, &timer);

        state++; // Avança o estado
    }
}

/* Função para monitorar o sono */
void monitorandoSono() {
    tight_loop_contents(); // Loop vazio para manter o sistema ativo
}

/* Função para tocar o alarme */
void tocarAlarme(uint8_t* data_sono) {
    bool led_active = false;

    // Salva os dados do sono na flash e libera a memória
    save_data(data_sono);
    free(data_sono);

    button_press_count_a = 0;
    button_press_count_b = 0;

    // Loop para tocar o alarme até que um botão seja pressionado
    while (button_press_count_a == 0 && button_press_count_b == 0) {
        if (!led_active) {
            npMatrix(ALL_RED); // Acende a matriz de LEDs
            led_active = true;
        } else {
            npMatrix(NOTHING); // Apaga a matriz de LEDs
            led_active = false;
        }
        play_alarm(); // Toca o alarme
    }

    button_press_count_a = 0;
    button_press_count_b = 0;
    npMatrix(NOTHING); // Apaga a matriz de LEDs
    state++; // Avança para o próximo estado
}

/* Função para gerar números aleatórios */
int randint(int n) {
    if ((n - 1) == RAND_MAX) {
        return rand();
    } else {
        // Evita viés na geração de números aleatórios
        assert(n <= RAND_MAX);
        int end = RAND_MAX / n; // Trunca o viés
        assert(end > 0);
        end *= n;

        int r;
        while ((r = rand()) >= end); // Gera números até que um válido seja encontrado

        return r % n;
    }
}

/* Função para inicializar o jogo da memória */
memoryGame* gameInit() {
    memoryGame* game = (memoryGame*)malloc(sizeof(memoryGame)); // Aloca um espaço na memória para a estrutura
    game->flecha = 0;
    game->level = 0;
    game->substate = 0;

    // Gera uma sequência aleatória de direções (0, 1, 2, 3 são direções)
    for (int i = 0; i < LEVELS_MEMORY_GAME; i++) {
        game->sequencia[i] = randint(4);
    }

    oled_clear();
    state++; // Avança para o próximo estado
    return game;
}

/* Função principal do jogo da memória */
void jogoDaMemoria(memoryGame* game) {
    int joy_z = 0; // Posição do joystick (-1 [centro], 0 [cima], 1 [baixo], 2 [direita], 3 [esquerda])

    oled_text(0, 0, "JOGO DA MEMORIA");
    oled_show();

    // Submáquina de estados para o jogo
    switch (game->substate) {
        case 0:
            // Exibe a sequência de direções com base nas macros definidas em matriz_leds.c
            for (int i = 0; i <= game->level; i++) {
                show_arrow(game->sequencia[i]);
            }
            alarm_id = add_alarm_in_ms(10000, alarm_game_callback, (void *)game, false); // Aciona um alarme para daqui 10 segundos
            game->substate = 1; // Vai para o próximo subestado
        case 1:
            // Aguarda a entrada do usuário
            joy_z = get_joystick_position();
            if (joy_z != -1) {
                npMatrix(lookup_table_indicator[joy_z]); // Caso houve movimento, mostra um feedback sobre a direção selecionada
                cancel_alarm(alarm_id); // Cancela o alarme de 10 segundos
                sleep_ms(500);
                game->substate = 2; // Passa para o próximo subestado (sem rodar o break para preservar o valor de joy_z)
            } else {
                break; // Caso não tiver movimento no joystick não roda o próximo subestado e o alarme ativa, fazendo com que o despertador toque de novo
            }
        case 2:
            npMatrix(NOTHING);
            // Verifica se a entrada do usuário está correta
            if (game->sequencia[game->flecha] == joy_z) {
                if (game->flecha >= game->level) { // Verifica se é a ultima entrada para esse nível
                    game->level++; // Se for, passa para o próximo níve
                    if (game->level >= LEVELS_MEMORY_GAME) { // Se chegar ao fim dos níveis
                        oled_clear();
                        free(game);
                        state++; // Avança para o próximo estado
                    } else {
                        game->flecha = 0; // Como não acabou volta para a primeira flecha
                        game->substate = 0; // e para o subestado 0
                    }
                } else {
                    game->substate = 1; // Como não é a última flecha, volta ao subestado 1 para receber uma nova entrada
                    game->flecha++;
                    alarm_id = add_alarm_in_ms(10000, alarm_game_callback, (void *)game, false); // Aciona o alarme novamente
                }
            } else {
                // Exibe mensagem de derrota
                oled_clear();
                oled_text(0, 0, "JOGO DA MEMORIA");
                oled_text(0, 30, "Voce perdeu");
                oled_show();

                free(game);
                state--; // Volta para o estado anterior

                sleep_ms(2000);
                oled_clear();
            }
    }
}

/* Função para exibir os resultados do monitoramento */
int resultadosMonitoramento(int pos_display, uint8_t* data_sono) {
    /*  Como o display oled só tem 127 pixels, caso o alarme rode por mais tempo que 127 segundos não é 
        possível gerar todo o gráfico de uma vez só no display, por conta disso a variável pos_display
        indica qual "parte" do gráfico está sendo mostrada (0, 1, ...) e é modificada pelo joystick
    */
    int joy = get_joystick_horizontal_pos(); // Lê o joystick
    if (joy > 0) {
        oled_clear();
        pos_display = (pos_display >= counter / 127) ? counter / 127 : pos_display + 1; // Garante que não passe do número máximo de telas que devem ser mostradas
        oled_graph(data_sono, counter > 127 ? 127 : counter, pos_display * 127); // Caso o alarme tenha ficado menos de 127 segundos rodando não tera 127 pontos para exibir, então exibe COUNTER pontos
        oled_show();
        sleep_ms(500);
    } else if (joy < 0) {
        oled_clear();
        pos_display = (pos_display == 0) ? 0 : pos_display - 1; // Garante que não seja um número negativo
        oled_graph(data_sono, counter > 127 ? 127 : counter, pos_display * 127);
        oled_show();
        sleep_ms(500);
    } else {
        oled_graph(data_sono, counter > 127 ? 127 : counter, pos_display * 127);
        oled_show();
    }
    return pos_display;
}

/* Função para inicializar os botões */
void button_init() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // Garante que o LED comece apagado

    // Configura os pinos dos botões como entrada com pull-up
    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A);

    gpio_init(BUTTON_PIN_B);
    gpio_set_dir(BUTTON_PIN_B, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_B);

    // Configura interrupções para os botões nas bordas de subida e descida
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    last_press_time_a = get_absolute_time();
    last_press_time_b = get_absolute_time();
}