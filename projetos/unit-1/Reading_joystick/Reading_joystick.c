#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/timer.h" // Biblioteca para gerenciamento de temporizadores de hardware.
#include "inc/Display_Oled.h"
#include "hardware/i2c.h"

#define VRX_PIN 27
#define VRY_PIN 26

int64_t massage_callback(alarm_id_t id, void *user_data);

// Configurações iniciais do joystick
void joystick_init(){
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(VRY_PIN);
    adc_gpio_init(VRX_PIN);
}

int get_joystick_raw_x_value(){
    adc_select_input(1);
    int adc_x_raw = adc_read();
    return adc_x_raw;
}

// Retorna o valor cru de 0 até adc_max do eixo y do joystick
int get_joystick_raw_y_value(){
    adc_select_input(0);
    int adc_y_raw = adc_read();
    return adc_y_raw;
}

// Essa função é chamada pelo temporizador quando o alarme expira.
int64_t massage_callback(alarm_id_t id, void *user_data) {
  printf("x: %d \ty: %d\n", get_joystick_raw_x_value(), get_joystick_raw_y_value());
  
  char str1[10], str2[10];
  sprintf(str1, "%d", get_joystick_raw_x_value());
  sprintf(str2, "%d", get_joystick_raw_y_value());

  oled_clear();
  oled_text(0, 0, str1);
  oled_text(0, 20, str2);
  oled_show();

  return 1000000; // Repete o alarme em 1 segundo
}

int main(){
  stdio_init_all(); // Inicializa a comunicação serial para permitir o uso de printf para depuração.
  joystick_init();
  oled_init();
  oled_clear();
  add_alarm_in_ms(1000, massage_callback, NULL, false);
  while(1){
    tight_loop_contents();
  }
}
