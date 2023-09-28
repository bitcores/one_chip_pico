#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"

#include "one_chip_in.pio.h"
#include "one_chip_out.pio.h"
#include "prg_data.h"
#include "prg_ram.h"


void core1_main() {
    
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

    //uint32_t bugger[105];
    //int b = 0;

    while(1) {
        uint32_t pointer = pio_sm_get_blocking(pioin, smin);

        /*if (b < 100) {
            bugger[b] = pointer;
            b++;
        }*/
        
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


        /*if (b == 100) {
            for (int c = 0; c < 100; c++) {
                int prg = 0;

                if (!(bugger[c] & 67108864)) {
                    printf("PRG-ROM ");
                    printf("%X ", ((bugger[c] & 32767) + 32768));
                    prg = 1;
                } else if ((bugger[c] & 24576) == 24576) {
                    printf("PRG-RAM ");
                    printf("%X ", (bugger[c] & 32767));
                    prg = 2;
                } else {
                    printf("ADDR    ");
                    printf("%X ", (bugger[c] & 32767));
                }


                uint8_t dbugdata = bugger[c] >> 15;

                if (bugger[c] & 134217728) {
                    printf("read  ");
                    if (prg == 1)
                        dbugdata = prg_data[bugger[c] & 32767];
                    else if (prg == 2)
                        dbugdata = prg_ram[bugger[c] & 8191];
                } else {
                    printf("write ");
                }

                printf("%X \n", dbugdata);

            }

            b = 1000;
        }*/
        
        
    }
}


int main(void){
    set_sys_clock_khz(270000, true);
    stdio_init_all();

    // if clock works and pico will be ready, turn on LED
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    multicore_launch_core1(core1_main);

    while(1) {
        tight_loop_contents();
    }

    return 0;
}