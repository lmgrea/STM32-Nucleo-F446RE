#ifndef BMP_280
#define BMP_280

#include "stm32f4xx_hal.h"

#include <stdint.h>

int bmp280_read_registers(I2C_HandleTypeDef *hi2c1, UART_HandleTypeDef *huart2);
int bmp280_calc_temp_press();
void bmp280_print_temp(UART_HandleTypeDef *huart2);
void bmp280_init(I2C_HandleTypeDef *hi2c1);

#endif
