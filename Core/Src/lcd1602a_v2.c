#include <string.h>
#include <stdio.h>

#include "lcd1602a_v2.h"

/*
	The first step is to write to the instruction register to set data length (4 bits),
	the number of display lines (2), and character font (5x10 dots).

	Register Select		Read/Write		DB7-DB0
	RS	RW	DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	0	0	0	0	1	DL	N	F	-	-

	DL = Data Lines;		 		DL = 0 = 4 bits; 		DL = 1 = 8 bits
	N  = Number of Display Lines;	N  = 0 = 1 line; 		N  = 1 = 2 lines
	F  = Character Fonts;			F  = 0 = 5 x 8 dots;	F  = 1 = 5 x 10 dots

	GPIO_PIN_12 = D7
	GPIO_PIN_13 = D6
	GPIO_PIN_14 = D5
	GPIO_PIN_15 = D4
	GPIO_PIN_4 =  E
*/

static char* last_string1;
static char* last_string2;

void lcd1602_init(I2C_HandleTypeDef *hi2c1)
{
	// RS and RW low for initialization
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // RS
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET); // RW
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET); // E

	lcd_write_nibble(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, 0x03);

	HAL_Delay(5);

	lcd_write_nibble(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, 0x03);
	lcd_write_nibble(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, 0x03);

	lcd_write_nibble(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, 0x02);

	HAL_Delay(30);

	// function set pg 24 covers the instructions instructions
	lcd_write_byte(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, 0x28);

	HAL_Delay(30);

	// turns on display, turns on cursor, and makes the cursor blink
	lcd_write_byte(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, 0x0C);// 0x0C for no cursor/blink; 0x0F for both

	HAL_Delay(30);

	lcd_write_byte(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, 0x06);

	HAL_Delay(30);

	lcd_write_byte(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, 0x01);

	HAL_Delay(300);
}

void lcd_write_byte(GPIO_TypeDef *port, uint16_t D7, uint16_t D6, uint16_t D5, uint16_t D4, uint8_t nibble)
{
	HAL_Delay(1);

	HAL_GPIO_WritePin(port, D7, (nibble & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(port, D6, (nibble & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(port, D5, (nibble & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(port, D4, (nibble & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

    HAL_Delay(1);

	HAL_GPIO_WritePin(port, D7, (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(port, D6, (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(port, D5, (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(port, D4, (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}

void lcd_write_nibble(GPIO_TypeDef *port, uint16_t D7, uint16_t D6, uint16_t D5, uint16_t D4, uint8_t nibble)
{
    HAL_Delay(1);

	HAL_GPIO_WritePin(port, D7, (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(port, D6, (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(port, D5, (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(port, D4, (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}

void lcd_display_string(char * string, int position)
{
	if((strcmp(last_string1, string) != 0) && (position == 1))
	{
		clear_display();
		last_string1 = string;
	}
	else if (strcmp(last_string2, string) != 0 && (position == 2))
	{
		clear_display();
		last_string2 = string;
	}

	int length = strlen(string);
	int starting_offset = (16 - length) / 2;

	uint8_t addr = 0x80 + starting_offset;

	if (position == 2)
	{
		addr = 0xC0 + starting_offset;
	}

	HAL_Delay(1);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // RS
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET); // RW

	HAL_Delay(1);

	lcd_write_byte(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, addr);

	HAL_Delay(1);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);

	for (int i = 0; i < 16; i++)
	{
		if (string[i] == '\n' || string[i] == '\0') break;
		uint8_t ascii = (uint8_t)string[i];

		HAL_Delay(1);

		lcd_write_byte(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, ascii);
	}
}

void clear_display()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // RS
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET); // RW

	HAL_Delay(1);

	lcd_write_byte(GPIOB, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, 1);

	HAL_Delay(1);
}
