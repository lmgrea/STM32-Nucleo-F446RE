#ifndef LCD_1602
#define LCD_1602

#include "stm32f4xx_hal.h"

#include <stdint.h>

void lcd1602_init(I2C_HandleTypeDef *hi2c1);
void lcd_write_byte(GPIO_TypeDef *port, uint16_t D7, uint16_t D6, uint16_t D5, uint16_t D4, uint8_t nibble);
void lcd_write_nibble(GPIO_TypeDef *port, uint16_t D7, uint16_t D6, uint16_t D5, uint16_t D4, uint8_t nibble);
void lcd_display_string(char * string, int position);
void clear_display();

#endif