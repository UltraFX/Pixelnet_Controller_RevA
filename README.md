# Pixelnet_Controller_RevA
Software to control WS2812B LEDs and TLC5947 PWM controllers by DMX-Data from UART

This program is written for STM32F103RBT6 with STM32CubeF1 Library.

To get this code working, you'll need to create a folder "Drivers" and inside that the folders "CMSIS" and "STM32F1xx_HAL_Driver" containing all library files.
You also need the folder "startup" with the file "startup_stm32f103xb.s" inside.
All needed files are included in the STM32CubeF1 package. 
You can also open the PixelMaster.ioc file with STM32CubeMX and generate all needed files automatically.

The IDE used is Software Workbench 4 STM32.