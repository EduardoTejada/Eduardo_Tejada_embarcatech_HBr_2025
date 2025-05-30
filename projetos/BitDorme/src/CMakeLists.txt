set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(BitDorme C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(BitDorme BitDorme.c inc/joystick.c inc/ssd1306_i2c.c inc/Display_Oled.c inc/Buzzer.c inc/matriz_leds.c inc/microphone_dma.c)

pico_set_program_name(BitDorme "BitDorme")
pico_set_program_version(BitDorme "0.1")

# Generate PIO header
pico_generate_pio_header(BitDorme ${CMAKE_CURRENT_LIST_DIR}/ws2818b.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(BitDorme 0)
pico_enable_stdio_usb(BitDorme 1)

# Add the standard library to the build
target_link_libraries(BitDorme
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(BitDorme PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(BitDorme 
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        hardware_adc
        hardware_i2c
        hardware_irq
        hardware_timer
        hardware_gpio
        hardware_pwm
        hardware_clocks
        hardware_rtc
        pico_util
        hardware_pio
        hardware_dma
        hardware_flash
        )

pico_add_extra_outputs(BitDorme)
