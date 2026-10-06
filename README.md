# Embedded C on STM32-Nucleo-F446RE

This is a personal project that documents my first non-academic foray into embedded C programming. This project uses the following software and technologies to read temperature, pressure, and accelerometer data and display them on an LCD:

## Software
- STM32CubeIDE
- STM32CubeMX
- PuTTY
- PulseView

## Hardware
- STM32 Nucleo-F446RE Development Board
- MPU-6050 3-Axis Accelerometer Gyroscope Module
- HiLetGo USB Logic Analyzer
- HiLetgo 6 Pin SPI Micro SD TF Card Adapter Reader Module
- Nucleo LCD-1602
- HiLetgo BMP280 High Precision Atmospheric Pressure Sensor

## Step 1 - Soldering
After a quick "Hello World!"-esque test program that made an on-board LED blink on and off with a delay, I needed to set up one of the sensors that I planned to use for this project.

I decided to start with the BMP280, as it had the fewest pins and therefore seemed the simplest.

As you can see in the image below, the sensor was shipped without its header pins soldered to the board.

![Shows an image of the BMP280 temperature and pressure sensor fresh out of the packaging with its unsoldered header pins.](images/unsoldered_bmp280.jpg)

This would be my first attempt at soldering electronics, so after a quick YouTube tutorial, I did the best I could and now have a (remarkably) undamaged and functioning BMP280.

![Shows an image of the BMP280 temperature and pressure sensor after I soldered on the header pins.](images/soldered_bmp280.jpg)

## Step 2 - Configuring Peripherals
Next, I had to configure the Nucleo board using STM32CubeMX. This software is a graphical tool that helps to initialize peripherals for the STM32 microcontroller.

Using this interface, you can select different pins and configure their functionality by selecting peripherals.

When a project is initialized using a Nucleo board, CubeMX provides a board-specific starting configuration, including relevant clock and peripheral settings. These settings can still be modified manually. To start out, I'm using the default configuration.

Now that CubeMX has configured the initialization code for me, I need to configure some pins that I'll be using for I2C. To do this, I set PB9 and PB8 to SDA and SCL respectively.

To find which pins in CubeMX correspond to which peripherals you need, this diagram from Mbed's old website was useful:

![From Mbed's old site, which now just redirects to their github page now but this is the old site: https://os.mbed.com/platforms/ST-Nucleo-F446RE/](images/pinout_diagram.png)

This is a screenshot from CubeMX that shows the entire chip configured.

![This is a screenshot from CubeMX that shows the entire chip configured.](images/general_configuration.png)

I configured PB8 and PB9 for I2C1, using PB8 as SCL and PB9 as SDA.

![This is what I had to personally configure to use the I2C communication protocol.](images/com_protocols_pins.png)

## Step 3 - Writing Code
Now that the pins are configured, I generate code from CubeMX. This updates all of the code in the project files that are necessary to set up my Nucleo board.

While there are many files that are generated in a project, the main application code is written in `Core/Src/main.c`. In an effort to organize the code, I am writing most of my drivers for different sensors in different files. For example, I have `bmp280.c` in `Core/Src/bmp280.c` and the corresponding header file in `Core/Inc/bmp280.h`.

Throughout `main.c` there are comments that show where the user can inject code. It is important to adhere to these comments, because if the pin configurations need to be changed to add or alter functionality, the project code has to be regenerated. Code placed outside these designated user-code sections may be overwritten when the project is regenerated.

For example, these are where you include libraries you need for your program:
```
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "bmp280.h"
/* USER CODE END Includes */
```

This section runs before the infinite loop, so sensor initialization can be placed here:
```
/* USER CODE BEGIN 2 */
bmp280_init(&hi2c1);
/* USER CODE END 2 */
```
This is the infinite loop, so read from sensors and update the display here:
```
/* USER CODE BEGIN WHILE */
while (1)
{
    bmp280_read_registers(&hi2c1, &huart2);
    bmp280_calc_temp_press();
    bmp280_print_temp(&huart2);
/* USER CODE END WHILE */

/* USER CODE BEGIN 3 */
}
/* USER CODE END 3 */
```

## Step 4 - Inspecting the I2C Protocol
Now that I have the program running, I wanted to learn to use PulseView to view signals.

PulseView is a very neat program. You can view signals that the microcontroller, the master device in this instance, is sending and receiving to and from the slave devices, which is just the BMP280 for now.

This image shows a complete exchange between the microcontroller and sensor.
![](images/bmp280_addr_signal.png)

## References
*"If I have seen further it is by standing on the shoulders of Giants."* - Isaac Newton

This [YouTube tutorial by Shawn Hymel for the DigiKey YouTube channel](https://www.youtube.com/watch?v=hyZS2p1tW-g&list=PLEBQazB0HUyRYuzfi4clXsKUSgorErmBv) was essential for my introduction to the STM32 development environment.

[ST's user manual](https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo32-boards-mb1180-stmicroelectronics.pdf) shows what pins are compatible with what peripherals. This website was helpful to relate things like PB_8 to I2C1_SCL and PB_9 to I2C1_SDA, which told me where to look in CubeMX for the signals I needed.

This is the [datasheet from Bosch for the BMP280](https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf). It contains the information needed to access the temperature and pressure data from the sensor's registers. 

I found this [paper written by Joseph Wu for Texas Instruments](https://www.ti.com/lit/an/sbaa565/sbaa565.pdf?ts=1791207882130&ref_url=https%253A%252F%252Fwww.google.com%252F) that details how the I2C protocol works. It was topical when I was inspecting signals on the logic analyzer.

Here is the [Hitachi HD44780 datasheet](https://cdn-shop.adafruit.com/datasheets/HD44780.pdf) that I used.