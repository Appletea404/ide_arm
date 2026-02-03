/*
 * lcd_4bit.h
 *
 *  Created on: Feb 3, 2026
 *      Author: appletea
 */

#ifndef INC_LCD_4BIT_H_
#define INC_LCD_4BIT_H_


#include "stm32f4xx_hal.h"



//RS pb15


#define LCD_RS_GPIO GPIOB
#define LCD_RS_PIN GPIO_PIN_15



//Enalbe pb14

#define LCD_E_GPIO GPIOB
#define LCD_E_PIN GPIO_PIN_14

//데이터 포트 (상위비트 4개)

#define LCD_D4_GPIO GPIOA
#define LCD_D4_PIN GPIO_PIN_11 //4

#define LCD_D5_GPIO GPIOB
#define LCD_D5_PIN GPIO_PIN_12 //5

#define LCD_D6_GPIO GPIOB
#define LCD_D6_PIN GPIO_PIN_2 //6

#define LCD_D7_GPIO GPIOB
#define LCD_D7_PIN GPIO_PIN_1 //7

//FUNCTION
void LCD_Data4bit(uint8_t data);

//Enable 핀에 Pulse를 주는 Function
//데이터 명령을 LCD내부로 Latch 시키는 역할
void LCD_EnablePin(void);

// 1바이트를 상/하 4비트로 나눠서 전송
void LCD_SendByte(uint8_t data);

//명령어 전송
void LCD_WriteCommand(uint8_t commandData);

//문자(데이터)전송
void LCD_WriteData(uint8_t charData);

//커서 이동 함수
void LCD_GotoXY(uint8_t row, uint8_t col);

// 문자열 출력 함수
void LCD_WriteString(char *string);

//좌표 이동후에 문자열 출력
void LCD_WriteStringXY(uint8_t row,uint8_t col, char *string);

//초기화 함수
void LCD_Init(void);



#endif /* INC_LCD_4BIT_H_ */
