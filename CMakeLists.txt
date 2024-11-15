# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Pico_TFT C CXX ASM)


set(PICO_DEOPTIMIZED_DEBUG 1)      # Disable optimizations. ne marche pas fuck

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# to locate all configuration files (def.h, lv_conf.h).
include_directories(./) 

# LVGL
#set(LVGL_PATH  "./lvgl/")
#add_subdirectory(${LVGL_PATH})
#include_directories(${LVGL_PATH})
#add_definitions(-DLVGL=1)


# Add executable. Default name is the project name, version 0.1
set(DRIVERS_FILES 
        Drivers/Graphics.c
        Drivers/PIO_RGB.c
        Drivers/SPI_RGB.c
        Drivers/Chipsets/GC9A01A.c
        Drivers/Chipsets/ILI9488.c)

add_executable(Pico_TFT main.c
        #lv_port_disp.c
        ${DRIVERS_FILES})


pico_set_program_name(Pico_TFT "Pico_TFT")
pico_set_program_version(Pico_TFT "0.1")


# Generate PIO header
#pico_generate_pio_header(Pico_TFT ${CMAKE_CURRENT_LIST_DIR}/Drivers/PIO_RGB.pio)


# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Pico_TFT 0)
pico_enable_stdio_usb(Pico_TFT 0)

# Add the standard library to the build
target_link_libraries(Pico_TFT
        pico_stdlib
        #lvgl
        ) 


# Add the standard include files to the build
target_include_directories(Pico_TFT PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/..) # for our common lwipopts or any other standard includes, if required


# Add any user requested libraries
target_link_libraries(Pico_TFT 
        hardware_spi
        hardware_i2c
        hardware_dma
        hardware_pio
        hardware_clocks)

pico_add_extra_outputs(Pico_TFT)




