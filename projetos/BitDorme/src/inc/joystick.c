#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define VRX_PIN 27
#define VRY_PIN 26

// Configurações iniciais do joystick
void joystick_init(){
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(VRY_PIN);
    adc_gpio_init(VRX_PIN);
}

// Retorna o valor cru de 0 até adc_max do eixo x do joystick
uint get_joystick_raw_x_value(){
    adc_select_input(1);
    uint adc_x_raw = adc_read();
    return adc_x_raw;
}

// Retorna o valor cru de 0 até adc_max do eixo y do joystick
uint get_joystick_raw_y_value(){
    adc_select_input(0);
    uint adc_y_raw = adc_read();
    return adc_y_raw;
}

// Retorna a porcentagem do eixo x do joystick
uint get_joystick_x_percentage(){
    const uint adc_max = (1 << 12) - 1;
    uint adc_x_percentage = (((float) get_joystick_raw_x_value()) / adc_max) * 100;
    return adc_x_percentage;
}

// Retorna a porcentagem do eixo y do joystick
uint get_joystick_y_percentage(){
    const uint adc_max = (1 << 12) - 1;
    uint adc_y_percentage = (((float) get_joystick_raw_y_value()) / adc_max) * 100;
    return adc_y_percentage;
}

// Retorna um inteiro entre -2 e +2 indicando a posição do joystick horizontalmente
int get_joystick_horizontal_pos(){
    return ((int) (get_joystick_x_percentage() / 20) - 2);
}

// Retorna um inteiro entre -2 e +2 indicando a posição do joystick verticalmente
int get_joystick_vertical_pos(){
    return ((int) (get_joystick_y_percentage() / 20) - 2);
}

// Retorna -1, 0, 1, 2, 3 para joystick parado, cima, baixo, direita, esquerda, respectivamente
int get_joystick_position(){
    int x = get_joystick_horizontal_pos();
    int y = get_joystick_vertical_pos();
    if(y != 0){
        if(y > 0)
            return 0;
        return 1;
    }
    else if(x != 0){
        if(x > 0)
            return 2;
        return 3;
    } else {
        return -1;
    }
}