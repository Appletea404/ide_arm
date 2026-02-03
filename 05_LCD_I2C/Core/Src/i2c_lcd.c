/*
 * i2c_lcd.c
 *
 *  Created on: Feb 3, 2026
 *      Author: appletea
 */


#include "i2c_lcd.h"
#include "i2c.h"

void lcd_command(uint8_t command)
{
	uint8_t high_nibble, low_nibble;	//4bit
	uint8_t i2c_buffer[4];				//저장용 Array
	high_nibble = command & 0xf0;
	low_nibble = (command << 4) & 0xf0;
	i2c_buffer[0] = high_nibble | 0x04 | 0x08;	// en = 1, rs = 0, rw = 0, backlight = 1
	i2c_buffer[1] = high_nibble | 0x00 | 0x08;	// en = 0, rs = 0, rw = 0, backlight = 1
	i2c_buffer[2] = low_nibble  | 0x04 | 0x08;	// en = 1, rs = 0, rw = 0, backlight = 1
	i2c_buffer[3] = low_nibble  | 0x00 | 0x08;	// en = 0, rs = 0, rw = 0, backlight = 1
	/*
	 * bit 0 (0x01) = RS
	 * bit 1 (0x02) = RW
	 * bit 2 (0x04) = EN
	 * bit 3 (0x08) = Backlight
	 * en 0 => en 1 = Latch LCD
	 * rs = 0 Command
	 * rs = 1 Data
	 */

	while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS, i2c_buffer, 4, 100) != HAL_OK);
}

void lcd_data(uint8_t data)
{
	uint8_t high_nibble, low_nibble;	//4bit
	uint8_t i2c_buffer[4];				//저장용 Array
	high_nibble = data & 0xf0;
	low_nibble = (data << 4) & 0xf0;
	i2c_buffer[0] = high_nibble | 0x05 | 0x08; // en = 1, rs = 1, rw = 0, backlight = 1
	i2c_buffer[1] = high_nibble | 0x01 | 0x08; // en = 0, rs = 1, rw = 0, backlight = 1
	i2c_buffer[2] = low_nibble  | 0x05 | 0x08; // en = 1, rs = 1, rw = 0, backlight = 1
	i2c_buffer[3] = low_nibble  | 0x01 | 0x08; // en = 0, rs = 1, rw = 0, backlight = 1
	while (HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS, i2c_buffer, 4, 100)!= HAL_OK);

}

void i2c_lcd_init(void)
{
	HAL_Delay(50);
	lcd_command(0x33);
	HAL_Delay(5);
	lcd_command(0x32);
	HAL_Delay(5);
	lcd_command(0x28);
	HAL_Delay(5);
	lcd_command(DISPLAY_ON);
	HAL_Delay(5);
	lcd_command(0x06);
	HAL_Delay(5);
	lcd_command(CLEAR_DISPLAY);
	HAL_Delay(2);
}

void lcd_string(char *str)
{
	while(*str)lcd_data(*str++);
}

void move_cursor(uint8_t row, uint8_t col)
{
	lcd_command(0x80 | row << 6 | col);
}
