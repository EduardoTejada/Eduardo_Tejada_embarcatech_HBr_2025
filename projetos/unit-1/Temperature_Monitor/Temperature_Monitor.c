#include <stdio.h>             // Biblioteca padrão para entrada e saída, utilizada para printf.
#include "pico/stdlib.h"       // Biblioteca padrão para funções básicas do Pico, como GPIO e temporização.
#include "hardware/adc.h"      // Biblioteca para controle do ADC (Conversor Analógico-Digital).
#include "hardware/timer.h" // Biblioteca para gerenciamento de temporizadores de hardware.
#include "hardware/i2c.h"
#include "inc/Display_Oled.h"

// Definições
#define ADC_TEMPERATURE_CHANNEL 4   // Canal ADC que corresponde ao sensor de temperatura interno

int64_t massage_callback(alarm_id_t id, void *user_data);

// Função para converter o valor lido do ADC para temperatura em graus Celsius
float adc_to_temperature(uint16_t adc_value) {
    // Constantes fornecidas no datasheet do RP2040
    const float conversion_factor = 3.3f / (1 << 12);  // Conversão de 12 bits (0-4095) para 0-3.3V
    float voltage = adc_value * conversion_factor;     // Converte o valor ADC para tensão
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;  // Equação fornecida para conversão
    return temperature;
}

// Função para converter o valor lido do ADC para temperatura em graus Fahrenheit
float adc_to_fahrenheit(uint16_t adc_value) {
    const float conversion_factor = 3.3f / (1 << 12);  // Conversão de 12 bits (0-4095) para 0-3.3V
    float voltage = adc_value * conversion_factor;     // Converte o valor ADC para tensão
    float temperature_f = 80.6f - (1.8f * (voltage - 0.706f) / 0.001721f);  // Equação para conversão
    return temperature_f;
}

// Essa função é chamada pelo temporizador quando o alarme expira.
int64_t massage_callback(alarm_id_t id, void *user_data) {
    // Lê o valor do ADC no canal selecionado (sensor de temperatura)
    uint16_t adc_value = adc_read();

    // Converte o valor do ADC para temperatura em graus Celsius
    float temperature_c = adc_to_temperature(adc_value);

    // Converte o valor do ADC para temperatura em graus Fahrenheit
    float temperature_f = adc_to_fahrenheit(adc_value);
        
    // Imprime a temperatura na comunicação serial
    printf("Temperatura: %.2f °C ou %.2f °F\n", temperature_c, temperature_f);

    char str1[10], str2[10];
    sprintf(str1, "%.2f", temperature_c);
    sprintf(str2, "%.2f", temperature_f);
  
    oled_clear();
    oled_text(0, 0, str1);
    oled_text(0, 20, str2);
    oled_show();

    return 1000000; // Repete o alarme em 1 segundo
}

int main() {
    // Inicializa a comunicação serial para permitir o uso de printf
    stdio_init_all();
    oled_init();
    oled_clear();

    // Inicializa o módulo ADC do Raspberry Pi Pico
    adc_init();

    // Seleciona o canal 4 do ADC (sensor de temperatura interno)
    adc_set_temp_sensor_enabled(true);  // Habilita o sensor de temperatura interno
    adc_select_input(ADC_TEMPERATURE_CHANNEL);  // Seleciona o canal do sensor de temperatura

    add_alarm_in_ms(1000, massage_callback, NULL, false);
    // Loop infinito para leitura contínua do valor de temperatura
    while (true) {
        tight_loop_contents();
    }

    return 0;
}
