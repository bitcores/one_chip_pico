cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(one_chip_pico)

pico_sdk_init()

set(PICO_COPY_TO_RAM 1)

add_executable(one_chip_pico
        one_chip_pico.c
        )

# pull in common dependencies
target_link_libraries(one_chip_pico 
        pico_stdlib
        pico_multicore
        hardware_pio
        )

pico_generate_pio_header(one_chip_pico
        ${CMAKE_CURRENT_LIST_DIR}/one_chip_in.pio
        )
pico_generate_pio_header(one_chip_pico
        ${CMAKE_CURRENT_LIST_DIR}/one_chip_out.pio
        )

pico_set_binary_type (one_chip_pico no_flash)

# enabling serial output will definitely cause the cart to not work properly
# but you may get some useful debugging information
#pico_enable_stdio_usb(one_chip_pico 1)
#pico_enable_stdio_uart(one_chip_pico 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(one_chip_pico)

