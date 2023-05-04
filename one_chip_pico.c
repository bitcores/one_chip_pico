#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"

#include "one_chip_pico.pio.h"
#include "prg_data.h"


void core1_main() {

    PIO pio = pio0;
    uint offset = pio_add_program(pio, &one_chip_pico_program);
    uint sm = pio_claim_unused_sm(pio, true);
    one_chip_pico_program_init(pio, sm, offset, 0);

    while(1) {
        uint16_t pointer = pio_sm_get_blocking(pio, sm);
        //uint8_t data = prg_data[pointer];
        
        // printf("Addr %X Data %X\n", pointer, data);
        
        //pio_sm_put(pio, sm, data);*/

        // are we shaving cycles here?
        pio_sm_put(pio, sm, prg_data[pointer]);
        
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