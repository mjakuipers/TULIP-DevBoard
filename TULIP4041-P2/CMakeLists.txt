# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.28)

#to be able to find tulip.h for the board type header
set(PICO_BOARD_HEADER_DIRS ${CMAKE_SOURCE_DIR}) 

# set for tulip board
set(PICO_BOARD tulip CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

message("Found pico-sdk version: ${PICO_SDK_VERSION_STRING}")

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set (CMAKE_BUILD_TYPE Debug)


project(tulip4041_module C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(SOURCES
                tulip4041.cpp           # main file              
                ffmanager.cpp           # Flash File Manager
                userinterface.cpp       # functions for the userinterface
                emulation.cpp           # all core1 hardware related functions
                fram.c                  # control of the FRAM
                cli-binding.c           # file with bindings for the CLI
                embedded_cli.c          # CLI engine
                cdc_helper.cpp          # helper functions for the multi-CDC USB interface
                globalsettings.cpp      # class for managing global settings
                usb-descriptors.cpp     # descriptors for the multi-CDC USB interface
                tracer.cpp              # all fdunctions for the HP41 bus tracer and disassembler
                powermodes.cpp          # control of RP2040 power modes, removed for RP2350
                hw_config.c             # for use of the uSD card FATFS library
                peripherals.cpp         # HP41 peripheral communication (Wand, Printers, HP-IL), non-time critical
                rosc.cpp                # helper for the power mode control"
                sdcard.cpp              # helper functions for the micro sdcard
                module.cpp              # embedded modules and functions for ROM management
                msc_device_disk.c       # functions to implement the uSDCard msc USB device
                i2c_devices.cpp         # I2C devices (RTC, display)
)

set(LINK_LIBRARIES
                pico_stdlib
                pico_bootsel_via_double_reset
                pico_unique_id
                hardware_pio
                hardware_clocks
                hardware_flash
                hardware_spi      
                hardware_i2c
                hardware_clocks
                hardware_powman
                hardware_dma
                hardware_adc
                pico_multicore
                tinyusb_device
                no-OS-FatFS-SD-SDIO-SPI-RPi-Pico)

set(INCLUDE_DIRECTORIES
                .
                lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
                lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
                lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
                $ENV{PICO_SDK_PATH}/lib/tinyusb/hw
                $ENV{PICO_SDK_PATH}/lib/tinyusb/src
)


###########################################################
##  Compile mode for the Module version, TULIP_HARDWARE = 1

# set for TULIP board
set(PICO_BOARD tulip CACHE STRING "Board type")
set(pico_cmake_set PICO_PLATFORM=rp2350)  

## Add executable. Default name is the project name
add_executable( tulip4041_module        # main application                
                ${SOURCES}              # all source files
)
target_compile_definitions(tulip4041_module PUBLIC 
                           TULIP_HARDWARE=1                           # define the TULIP_HARDWARE macro
                           VERSION_STRING="0.96 BETA"
)

pico_set_program_name(tulip4041_module "tulip4041")
pico_set_program_version(tulip4041_module VERSION_STRING)

# include the FatFS library
add_subdirectory(lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src)  

# Generate PIO header
pico_generate_pio_header(tulip4041_module ${CMAKE_CURRENT_LIST_DIR}/hp41_pio.pio)

# Modify the below lines to enable/disable output over UART/USB
# not used in the tulip project
pico_enable_stdio_uart(tulip4041_module 1)      # enable UART stdio
pico_enable_stdio_usb(tulip4041_module 0)       # no USB stdio used

# Add the linker script to the build
# added to force the queue functions in RAM, otherwise they are in flash
# pico_set_linker_script(tulip4041_module ${CMAKE_CURRENT_LIST_DIR}/memmap_custom.ld)

# Add the standard library to the build
target_link_libraries(tulip4041_module
                      ${LINK_LIBRARIES}       # all libraries
)


# Add the standard include files to the build
target_include_directories(tulip4041_module PRIVATE
                           ${INCLUDE_DIRECTORIES}
)

pico_add_extra_outputs(tulip4041_module)

###########################################################
##  Compile mode for the DevBoard version, TULIP_HARDWARE = 0

# set for pico board
set(PICO_BOARD pico2 CACHE STRING "Board type")
set(pico_cmake_set PICO_PLATFORM=rp2350)  

## Add executable. Default name is the project name
add_executable( tulip4041_devboard      # main application                
                ${SOURCES}              # all source files
)
target_compile_definitions(tulip4041_devboard PUBLIC 
                           TULIP_HARDWARE=0     # define the TULIP_HARDWARE macro
                           VERSION_STRING="0.96 BETA"
                          ) 

pico_set_program_name(tulip4041_devboard "tulip4041")
pico_set_program_version(tulip4041_devboard VERSION_STRING)

# include the FatFS library
## add_subdirectory(lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src)  

# Generate PIO header
pico_generate_pio_header(tulip4041_devboard ${CMAKE_CURRENT_LIST_DIR}/hp41_pio.pio)

# Modify the below lines to enable/disable output over UART/USB
# not used in the tulip project
pico_enable_stdio_uart(tulip4041_devboard 1)
pico_enable_stdio_usb(tulip4041_devboard 0)        # no USB stdio used

# Add the linker script to the build
# added to force the queue functions in RAM, otherwise they are in flash
# pico_set_linker_script(tulip4041_devboard ${CMAKE_CURRENT_LIST_DIR}/memmap_custom.ld)

# Add the standard library to the build
target_link_libraries(tulip4041_devboard
                      ${LINK_LIBRARIES}       # all libraries
)


# Add the standard include files to the build
target_include_directories(tulip4041_devboard PRIVATE
                           ${INCLUDE_DIRECTORIES}
)

pico_add_extra_outputs(tulip4041_devboard)