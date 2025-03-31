#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUZZER_PIN 21
#define BUZZER_FREQUENCY 100

// Definição das notas do toque de alarme (frequências em Hz)
const uint alarm_notes[] = {
    880, 440, 880, 440, 880, 440, 1000, 500, 1000, 500
};

// Duração das notas em milissegundos
const uint alarm_durations[] = {
    200, 200, 200, 200, 200, 200, 300, 300, 300, 300
};

void buzzer_init() {
    // Configurar o pino como saída de PWM
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(BUZZER_PIN, 0);
}


// Toca uma nota com a frequência e duração especificadas
void play_tone(uint frequency, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / frequency - 1;

    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(BUZZER_PIN, top / 2); // 50% de duty cycle

    sleep_ms(duration_ms);

    pwm_set_gpio_level(BUZZER_PIN, 0); // Desliga o som após a duração
    sleep_ms(50); // Pausa entre notas
}


// Função para tocar o toque de alarme
void play_alarm() {
    for (int i = 0; i < sizeof(alarm_notes) / sizeof(alarm_notes[0]); i++) {
        play_tone(alarm_notes[i], alarm_durations[i]);
    }
    sleep_ms(500); // Pequena pausa antes de repetir o alarme
}
