#include <string.h>
#include <stdio.h>

#include "bmp280.h"

static const uint8_t BMP280_ADDR 			= 0x76 << 1; // 8 bit address found in datasheet when SDL is GND and CSB is 3.3V

static const uint8_t CONFIG_REG 			= 0xF4;
static const uint8_t BMP280_TEMP_PRESS_ADDR = 0xF7;

static const uint8_t DIG_T1_ADDR 			= 0x88;
static const uint8_t DIG_T2_ADDR 			= 0x8A;
static const uint8_t DIG_T3_ADDR 			= 0x8C;

static const uint8_t DIG_P1_ADDR 			= 0x8E;
static const uint8_t DIG_P2_ADDR 			= 0x90;
static const uint8_t DIG_P3_ADDR 			= 0x90;
static const uint8_t DIG_P4_ADDR 			= 0x92;
static const uint8_t DIG_P5_ADDR 			= 0x94;
static const uint8_t DIG_P6_ADDR 			= 0x96;
static const uint8_t DIG_P7_ADDR 			= 0x98;
static const uint8_t DIG_P8_ADDR 			= 0x9A;
static const uint8_t DIG_P9_ADDR 			= 0x9C;

uint8_t BMP280_RAW_TEMP_PRESS[6];
uint16_t dig_T1, dig_P1;
int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
uint32_t adc_T, adc_P;
int32_t t_fine;
int32_t T;
uint8_t config = 0x27;

int result[2];

void bmp280_init(I2C_HandleTypeDef *hi2c1)
{
	HAL_I2C_Mem_Write(hi2c1, BMP280_ADDR, CONFIG_REG, I2C_MEMADD_SIZE_8BIT, &config, 1, HAL_MAX_DELAY);
}

int bmp280_read_registers(I2C_HandleTypeDef *hi2c1)
{
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, BMP280_TEMP_PRESS_ADDR, I2C_MEMADD_SIZE_8BIT, BMP280_RAW_TEMP_PRESS, 6, HAL_MAX_DELAY);

	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_T1_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_T1, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_T2_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_T2, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_T3_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_T3, 2, HAL_MAX_DELAY);

	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_P1_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_P1, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_P2_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_P2, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_P3_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_P3, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_P4_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_P4, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_P5_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_P5, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_P6_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_P6, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_P7_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_P7, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_P8_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_P8, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, DIG_P9_ADDR, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&dig_P9, 2, HAL_MAX_DELAY);

	return 0;
}

int* bmp280_calculate_temperature()
{
	// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
	// t_fine carries fine temperature as global value
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

	result[0] = (int)T;
	result[1] = (int)bmp280_calculate_pressure();
	return result;
}

int bmp280_calculate_pressure()
{
	// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
	// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
	int64_t var1, var2, p;

	adc_P = ((uint32_t)BMP280_RAW_TEMP_PRESS[0] << 12)
	      | ((uint32_t)BMP280_RAW_TEMP_PRESS[1] << 4)
	      | ((uint32_t)BMP280_RAW_TEMP_PRESS[2] >> 4);

	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
	var2 = var2 + (((int64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p<<31)-var2)*3125) / var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);

	return p;
}

void bmp280_print_temp(UART_HandleTypeDef *huart2)
{
	char msg[100];
	sprintf(msg, "Temperature: %ld.%02ld F\r\n", T / 100, T % 100);

	HAL_UART_Transmit(huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

