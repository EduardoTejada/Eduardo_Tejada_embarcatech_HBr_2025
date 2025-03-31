// Configurações iniciais do joystick
void joystick_init();

// Retorna o valor cru de 0 até adc_max do eixo x do joystick
uint get_joystick_raw_x_value();

// Retorna o valor cru de 0 até adc_max do eixo y do joystick
uint get_joystick_raw_y_value();

// Retorna a porcentagem do eixo x do joystick
uint get_joystick_x_percentage();

// Retorna a porcentagem do eixo y do joystick
uint get_joystick_y_percentage();

// Retorna um inteiro entre -2 e +2 indicando a posição do joystick horizontalmente
int get_joystick_horizontal_pos();

// Retorna um inteiro entre -2 e +2 indicando a posição do joystick verticalmente
int get_joystick_vertical_pos();

// Retorna 0, 1, 2, 3 para cima, baixo, direita, esquerda, respectivamente
int get_joystick_position();
