#ifndef MATRIZ_LEDS_H
#define MATRIZ_LEDS_H
#define COLOR_INTENSITY(color, intensity) ((uint8_t)(color * intensity))

#define IT 0.3

#define RED      COLOR_INTENSITY(0, IT), COLOR_INTENSITY(255, IT), COLOR_INTENSITY(0, IT)
#define GREEN    COLOR_INTENSITY(255, IT), COLOR_INTENSITY(0, IT), COLOR_INTENSITY(0, IT)
#define BLUE     COLOR_INTENSITY(0, IT), COLOR_INTENSITY(0, IT), COLOR_INTENSITY(255, IT)
#define WHITE    COLOR_INTENSITY(255, IT), COLOR_INTENSITY(255, IT), COLOR_INTENSITY(255, IT)
#define BLACK    COLOR_INTENSITY(0, IT), COLOR_INTENSITY(0, IT), COLOR_INTENSITY(0, IT)
#define YELLOW   COLOR_INTENSITY(255, IT), COLOR_INTENSITY(255, IT), COLOR_INTENSITY(0, IT)
#define CYAN     COLOR_INTENSITY(255, IT), COLOR_INTENSITY(0, IT), COLOR_INTENSITY(255, IT)
#define MAGENTA  COLOR_INTENSITY(0, IT), COLOR_INTENSITY(255, IT), COLOR_INTENSITY(255, IT)
#define ORANGE   COLOR_INTENSITY(165, IT), COLOR_INTENSITY(255, IT), COLOR_INTENSITY(0, IT)
#define PINK     COLOR_INTENSITY(192, IT), COLOR_INTENSITY(255, IT), COLOR_INTENSITY(203, IT)
#define PURPLE   COLOR_INTENSITY(128, IT), COLOR_INTENSITY(0, IT), COLOR_INTENSITY(128, IT)
#define LIME     COLOR_INTENSITY(255, IT), COLOR_INTENSITY(50, IT), COLOR_INTENSITY(0, IT)
#define TEAL     COLOR_INTENSITY(128, IT), COLOR_INTENSITY(0, IT), COLOR_INTENSITY(128, IT)
#define NAVY     COLOR_INTENSITY(0, IT), COLOR_INTENSITY(0, IT), COLOR_INTENSITY(128, IT)
#define SKY_BLUE COLOR_INTENSITY(135, IT), COLOR_INTENSITY(206, IT), COLOR_INTENSITY(235, IT)
#define TURQUOISE COLOR_INTENSITY(64, IT), COLOR_INTENSITY(224, IT), COLOR_INTENSITY(208, IT)
#define GOLD     COLOR_INTENSITY(215, IT), COLOR_INTENSITY(175, IT), COLOR_INTENSITY(0, IT)
#define BROWN    COLOR_INTENSITY(139, IT), COLOR_INTENSITY(69, IT), COLOR_INTENSITY(19, IT)


// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

extern const npLED_t ALL_RED[5][5];
extern const npLED_t CORRECT[5][5];
extern const npLED_t NOTHING[5][5];
extern const npLED_t UP_ARROW[5][5];
extern const npLED_t UP_INDICATOR[5][5];
extern const npLED_t DOWN_ARROW[5][5];
extern const npLED_t DOWN_INDICATOR[5][5];
extern const npLED_t RIGHT_ARROW[5][5];
extern const npLED_t RIGHT_INDICATOR[5][5];
extern const npLED_t LEFT_ARROW[5][5];
extern const npLED_t LEFT_INDICATOR[5][5];
extern const npLED_t JOYSTICK_ARROW[5][5];

extern const npLED_t (*lookup_table_arrow[4])[5];
extern const npLED_t (*lookup_table_indicator[4])[5];

void npInit();
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b);
void npClear();
void npWrite();
void npMatrix(const npLED_t matrix[5][5]);
void show_arrow(int which_arrow);

#endif