#include "pico/stdlib.h"
#include "hardware/gpio.h"
//#include "hardware/vreg.h"

#include "one_chip_in.pio.h"
#include "one_chip_out.pio.h"
#include "prg_data.h"
//#include "prg_ram.h"

// set up an empty 8kB for prg_ram. comment if using prg_ram.h
uint8_t prg_ram[8192];

int main(void){
    //vreg_set_voltage(VREG_VOLTAGE_1_15);
    set_sys_clock_khz(270000, true);

    // if clock works and pico will be ready, turn on LED
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);


    // setup pio program for handling input
    PIO pioin = pio0;
    uint offsetin = pio_add_program(pioin, &one_chip_in_program);
    uint smin = pio_claim_unused_sm(pioin, true);
    one_chip_in_program_init(pioin, smin, offsetin, 0);

    // setup pio program for handling output
    PIO pioout = pio1;
    uint offsetout = pio_add_program(pioout, &one_chip_out_program);
    uint smout = pio_claim_unused_sm(pioout, true);
    one_chip_out_program_init(pioout, smout, offsetout, 15);

    while(1) {
        uint32_t pointer = pio_sm_get_blocking(pioin, smin);
        
        if (!(pointer & 67108864)) {        // check if prg rom space
            if (pointer & 134217728) {       // is read
                pio_sm_put(pioout, smout, prg_data[pointer & 32767]);
            }
            else {                          // is write
                uint8_t data = pointer >> 15; // push address off, store in int8
                prg_data[pointer & 32767] = data;
            }
        } else if ((pointer & 24576) == 24576) {       // check if prg ram space
            if (pointer & 134217728) {       // is read
                pio_sm_put(pioout, smout, prg_ram[pointer & 8191]);
            }
            else {                          // is write
                uint8_t data = pointer >> 15; // push address off, store in int8
                prg_ram[pointer & 8191] = data;
            }
        }

        pio_sm_clear_fifos(pioin, smin);
        
    }

    return 0;
}