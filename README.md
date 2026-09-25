# Embedded C on STM32-Nucleo-F446RE

This is a personal project that documents my first non-academic foray into  embedded C programming. This project uses the following softwares and technologies to read temperature, pressure, and accelerometer data and display them on an LCD:

## Software
- STM32CubeIDE
- STM32CubeMX
- PuTTY

## Hardware
- STM32 Nucleo-F446RE Microcontroller
- MPU-6050 3-Axis Accelerometer Gyroscope Module
- HiLetGo USB Logic Analyzer
- HiLetgo 6 Pin SPI Micro SD TF Card Adapter Reader Module
- Nucleo LCD-1602
- HiLetgo BMP280 High Precision Atmospheric Pressure Sensor

## Step 1
After a quick "Hello World!" esque test program that made an on-board LED blink on and off with a delay, I needed to set up one of the sensors that I planned to use this project.

I decided to start with the BMP280, as it had the fewest pins and therefore seemed the simplest.

As you can see in the below image, the sensor was shipped without its header pins soldered to the board.

![Shows and image of the BMP280 temperature and pressure sensor fresh out of the packaging with its unsoldered header pins.](images/unsoldered_bmp280.jpg)

This would be my first attempt at soldering electroincs, so after a quick youtube tutorial, I did the best I could and now have a (remarkably) undamaged and functioning BMP280.

![Shows and image of the BMP280 temperature and pressure sensor after I soldered on the header pins.](images/soldered_bmp280.jpg)

## Step 2
Next, I had to configure the Nucleo board using STM32CubeMX. This software is a graphical tool that helps to initialize peripherals for the STM32 microcontroller.

Using this interface, you can select different pins and configure their functionality by selecting peripherals. To start, I'll be using the I2C and USART protocals.

When a project is initialzed using a Nucleo board, many of the peripherals are defined for the user (this sets up clocks and timing diagrams that can be configured manually if someone were making their own board for the STM32 microcontroller, but the presets available for Nucleo boards abstracts this for the user).

Now that CubeMX has initialzed the board for me, I need to configure some pins that I'll be using for I2C and USART. To do this, I set PB9 and PB8 to SDA and SCL respectively.

This is a screenshot from CubeMX that shows the entire chip configured.

![This is a screenshot from CubeMX that shows the entire chip configured.](images/general_configuration.png)

I personally configured PB8 and PB9 to use the two communications protocals I2C and USART.

![This is what I had to personally configure to use the two communications protocals, I2C and USART.](images/com_protocals_pins.png)

## References
"If I have seen further it is by standing on the shoulders of Giants."