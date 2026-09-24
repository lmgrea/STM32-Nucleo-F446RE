#include <string.h>
#include <stdio.h>

#include "bmp280.h"

static const uint8_t BMP280_ADDR = 0x76 << 1; // 8 bit address found in datasheet when SDL is GND and CSB is 3.3V
static const uint8_t BMP280_ID_ADDR = 0xD0;
static const uint8_t BMP280_TEMP_PRESS_ADDR = 0xF7;

uint8_t BMP280_RAW_TEMP_PRESS[6];
uint16_t dig_T1;
int16_t dig_T2, dig_T3;
uint32_t adc_T;
int32_t T;
uint8_t config = 0x27;

int bmp280_read_registers(I2C_HandleTypeDef *hi2c1, UART_HandleTypeDef *huart2)
{
	char msg[100];
	uint8_t BMP280_ID;

	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, BMP280_ID_ADDR, I2C_MEMADD_SIZE_8BIT, &BMP280_ID, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, BMP280_TEMP_PRESS_ADDR, I2C_MEMADD_SIZE_8BIT, BMP280_RAW_TEMP_PRESS, 6, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, 0x88, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_T1, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, 0x8A, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_T2, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, 0x8C, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_T3, 2, HAL_MAX_DELAY);

	sprintf(msg, "BMP280 ID: 0x%02X\r\n", BMP280_ID);

	HAL_UART_Transmit(huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	HAL_Delay(1000);

	return 0;
}

int bmp280_calc_temp_press()
{
	// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
	// t_fine carries fine temperature as global value
	int32_t t_fine;
	int32_t var1, var2;

	adc_T = ((uint32_t)BMP280_RAW_TEMP_PRESS[3] << 12)
	      | ((uint32_t)BMP280_RAW_TEMP_PRESS[4] << 4)
	      | ((uint32_t)BMP280_RAW_TEMP_PRESS[5] >> 4);

	var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1)))
	        * (int32_t)dig_T2) >> 11;

	var2 = (((((adc_T >> 4) - (int32_t)dig_T1)
	          * ((adc_T >> 4) - (int32_t)dig_T1)) >> 12)
	          * (int32_t)dig_T3) >> 14;

	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	T = T * 9 / 5 + 3200;
	return T;
}

void bmp280_print_temp(UART_HandleTypeDef *huart2)
{
	char msg[100];
	sprintf(msg, "Temperature: %ld.%02ld F\r\n", T / 100, T % 100);

	HAL_UART_Transmit(huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void bmp280_init(I2C_HandleTypeDef *hi2c1)
{
	HAL_I2C_Mem_Write(hi2c1, BMP280_ADDR, 0xF4,
	                  I2C_MEMADD_SIZE_8BIT, &config, 1, HAL_MAX_DELAY);
}
