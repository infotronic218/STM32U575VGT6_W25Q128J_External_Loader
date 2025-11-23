# STM32U575VGT6_W25Q128J_External_Loader

This project is an adaption of OctoSPI driver for STM32H743 MCU from ---
for STM32U575VGT6 based board with an W25Q128J flash memory.

The cube mx file have been modified to use the correct pins on the custom board.
I found some trouble to get the driver working well for my board.

I added some modification on the Flash driver to make it compatible for board.

A command line to copy the hex into the stm32cubeprogrammer exteral loader directory.

``` bash
cp  "STM32U575VGT6_W25Q128J_ExternalLoader.elf" "/home/$USER/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/ExternalLoader/W25Q128JVE_STM32U575VGT6.stldr"
```


