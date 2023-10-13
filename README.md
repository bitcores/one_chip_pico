# one_chip_pico

### Overview
A Famicom cartridge running on a Raspberry Pi Pico, heavily derived from the PicoCart64 project.

This is the program driving the custom cartridge seen in https://forums.nesdev.org/viewtopic.php?p=287730&sid=39f16afe21ae319c02cf7025c29bdbed#p287730

nes2h.py is used to convert your PRG .nes file to the prg_data.h file used to compile the PRG data into the program. This could be improved.

As this is a PRG only solution, with no CHR-ROM present and using half the nametable RAM as CHR-RAM, no official titles will run on it. A few homebrew games will run, like MagicFloor. This is for running custom code.

### Writing to the cartridge
New in this verison is the ability to read and write to the 6000 to 7FFF address space equivalent to PRG_RAM, as well as the ability to write to the 8000 to FFFF PRG_ROM address space.  Writing to PRG_ROM should be done with care.

There are two ways to use PRG_RAM on this cartridge; first, as purely RAM; second, prefilled with data. The default is to use the PRG_RAM purely as RAM. In this case, the Pico seems to reliably clear RAM on power cycle, so PRG_RAM should be empty when the Pico is powered on. However, the Pico does not detect console Reset signal, so the PRG_RAM contents __will__ persist through a console Reset, __as will any writes to PRG_ROM__.

If prefilling the PRG_RAM (using a prg_ram.h) the software must be compiled with the no_flash binary type. This will load the program directly into the Pico RAM and the program will not persist through a power cycle. The program will need to be copied to the Pico each time it is power cycled.

### Hardware
The v2+writes cart has address pins from GPIO pins 0 - 14 on the Pico, data pins from 15 - 22 with the ROMSEL on pin 26, the CPU R/W on pin 27 and the M2 signal on pin 28.

ROMSEL, R/W and M2 are all connected through a voltage divider to protect the ADC GPIO pins from higher than ideal voltages (tighter voltage targets might improve stability)

![](images/v2+wfront.JPG)
![](images/v2+wback.JPG)

When tapping slot contacts directly, the console pins will ride up on the wires which can cause dodgy connections if the wires are too thick. I squashed mine flat to help mitigate this, also make sure there are no loose strands of wire that could short to nearby pads.

 This cart is compatible with the v2 builds, but prg_data.h files generated with the const tag won't compile with this build (just remove the const).

 Presumably this can be adapted to an NES cartridge, so long as the cartridge retains the CIC chip. Note the difference in pin assignments between the Famicom and NES, here is a pinout sheet https://www.benheck.com/Downloads/NES_Famicom_Pinouts.pdf
