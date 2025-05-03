cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(blink_led_modular C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(blink_led_modular)

target_sources(blink_led_modular PRIVATE
    app/main.c
    drivers/led_embutido.c
    hal/hal_led.c
)

target_include_directories(blink_led_modular PRIVATE
    include
)

pico_enable_stdio_usb(blink_led_modular 1)
pico_enable_stdio_uart(blink_led_modular 0)

pico_add_extra_outputs(blink_led_modular)