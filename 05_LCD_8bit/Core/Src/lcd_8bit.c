/*
 * lcd_8bit.c
 *
 *  Created on: Feb 2, 2026
 *      Author: appletea
 */

#include "lcd_8bit.h"


//FUNCTION
void LCD_Data(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_D0_GPIO, LCD_D0_PIN, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D1_GPIO, LCD_D1_PIN, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D2_GPIO, LCD_D2_PIN, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D3_GPIO, LCD_D3_PIN, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D4_GPIO, LCD_D4_PIN, (data & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D5_GPIO, LCD_D5_PIN, (data & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D6_GPIO, LCD_D6_PIN, (data & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D7_GPIO, LCD_D7_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

//Enable 핀에 Pulse를 주는 Function
//데이터 명령을 LCD내부로 Latch 시키는 역할
void LCD_EnablePin(void)
{
	HAL_GPIO_WritePin(LCD_E_GPIO, LCD_E_PIN, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_E_GPIO, LCD_E_PIN, GPIO_PIN_SET);
	HAL_Delay(1);
	//LOW로 떨어질때 Data 전달
	HAL_GPIO_WritePin(LCD_E_GPIO, LCD_E_PIN, GPIO_PIN_RESET);
	HAL_Delay(2);	//1.6ms LOW로 유지해야 데이터가 전달됨 그래서 2


}

//명령어 전송
void LCD_WriteCommand(uint8_t commandData)
{
	//명령어 모드일때는 RS -> 0
	HAL_GPIO_WritePin(LCD_RS_GPIO, LCD_RS_PIN, GPIO_PIN_RESET);
	//명령어 전송
	LCD_Data(commandData);
	//Enable 실행
	LCD_EnablePin();
}

//문자(데이터)전송
void LCD_WriteData(uint8_t charData)
{
	//데이터모드 RS -> 1
	HAL_GPIO_WritePin(LCD_RS_GPIO, LCD_RS_PIN, GPIO_PIN_SET);
	LCD_Data(charData);
	LCD_EnablePin();
}

//커서 이동 함수
void LCD_GotoXY(uint8_t row, uint8_t col)
{
	//row 범위 제한 (0,1)
	if(row > 1) row = 1;
	//col 제한 (0~15)
	if(col > 15) col = 15;

	//LCD DRAM 주소
	//1행 : 0x00 ~ 0x0F
	//2행 : 0x40 ~ 0x4F

	uint8_t addr = (row == 0) ? col : (0x40 + col);
	// 커서 이동 명령
	LCD_WriteCommand(0x80 | addr);
}

// 문자열 출력 함수
void LCD_WriteString(char *string)
{
	while(*string)
	{
		LCD_WriteData(*string++);
	}
}

//좌표 이동후에 문자열 출력
void LCD_WriteStringXY(uint8_t row,uint8_t col, char *string)
{
	LCD_GotoXY(row, col);
	LCD_WriteString(string);
}

//초기화 함수
void LCD_Init(void)	//PDF초기화 과정 참조
{
	HAL_Delay(50);		//안정화를 위해서
	LCD_WriteCommand(0x38);
	HAL_Delay(5);
	LCD_WriteCommand(0x38);
	HAL_Delay(6);
	LCD_WriteCommand(0x38);

	LCD_WriteCommand(0x38);

	LCD_WriteCommand(0x0C);	//화면 ON 커서 OFF 점멸 OFF

	LCD_WriteCommand(0x01);	//LCD Clear

	HAL_Delay(5);

	LCD_WriteCommand(0x06);	//커서는 오른쪽이동, 화면이동 없음

}




