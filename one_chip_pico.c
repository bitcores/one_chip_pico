#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "one_chip_in.pio.h"
#include "one_chip_out.pio.h"
#include "prg_data.h"
//#include "prg_cmp.h"
//#include "prg_ram.h"

// set up empty 32kB for prg_data. comment if using prg_data.h
//uint8_t prg_data[32768];

// set up an empty 8kB for prg_ram. comment if using prg_ram.h
uint8_t prg_ram[8192];

int main(void){
    //vreg_set_voltage(VREG_VOLTAGE_1_15);
    set_sys_clock_khz(270000, true);

    // unpack the prg data from the prg_cmp
    // uncomment this block if using prg_cmp.h
    /*int y = 0;
    int i = 0;
    bool tictoc = true;

    while (i < sizeof(prg_cmp)-1) {
        uint8_t t = prg_cmp[i];
        uint8_t b = prg_cmp[i+1];
        uint8_t v = 0;
        uint8_t cnt = 0;
        
        if (tictoc) {
            v = t;
            cnt = b >> 4;
            i += 1;
        } else {
            v = (t & 15) << 4;
            v += b >> 4;
            cnt = b & 15;
            i += 2;
        }

        for (int c = 0; c < cnt+1; c++){
            prg_data[y] = v;
            y++;
        }

        tictoc = !tictoc;
    }*/

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