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
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================

# set for pico board
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(tulip4041 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable( tulip4041               # main application
                tulip4041.cpp           # main file              
                userinterface.cpp       # functions for the userinterface
                emulation.cpp           # all core1 hardware related functions
                fram.c                  # control of the FRAM
                cli-binding.c           # file with bindings for the CLI
                embedded_cli.c          # CLI engine
                cdc_helper.cpp          # helper functions for the multi-CDC USB interface
                globalsettings.cpp      # class for managing global settings
                usb-descriptors.cpp     # descriptors for the multi-CDC USB interface
                tracer.cpp              # all fdunctions for the HP41 bus tracer and disassembler
                powermodes.cpp          # control of RP2040 power modes
                hw_config.c             # for use of the uSD card FATFS library
                peripherals.cpp         # HP41 peripheral communication (Wand, Printers, HP-IL), non-time critical
                rosc.cpp                # helper for the power mode control"
                sdcard.cpp              # helper functions for the micro sdcard
                module.cpp              # embedded modules and functions for ROM management
                msc_device_disk.c       # functions to implement the uSDCard msc USB device
                ffmanager.cpp           # Flash File Manager
        )

pico_set_program_name(tulip4041 "tulip4041")
pico_set_program_version(tulip4041 "0.1")

# include the FatFS library
add_subdirectory(lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src)  

# Generate PIO header
pico_generate_pio_header(tulip4041 ${CMAKE_CURRENT_LIST_DIR}/hp41_pio.pio)

# Modify the below lines to enable/disable output over UART/USB
# not used in the tulip project
#pico_enable_stdio_uart(tulip4041 0)
#pico_enable_stdio_usb(tulip4041 0)

# Add the standard library to the build
target_link_libraries(  tulip4041
                        pico_stdlib
                        hardware_pio
                        hardware_clocks
                        hardware_flash
                        hardware_spi      
                        hardware_clocks
                        hardware_rtc
                        hardware_dma
                        pico_multicore
                        tinyusb_device
                        no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
                        )

# Add the standard include files to the build
target_include_directories(tulip4041 PRIVATE
                        .
                        lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
                        lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
                        lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
                        $ENV{PICO_SDK_PATH}/lib/tinyusb/hw
                        $ENV{PICO_SDK_PATH}/lib/tinyusb/src
                        )

pico_add_extra_outputs(tulip4041)

