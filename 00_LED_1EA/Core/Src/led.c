/*
 * led.c
 *
 *  Created on: Jan 22, 2026
 *      Author: mujukpopo
 */


#include "led.h"



LED_CONTROL led[8]=
    {
        {GPIOC, GPIO_PIN_8, GPIO_PIN_SET, GPIO_PIN_RESET},      // 0
        {GPIOC, GPIO_PIN_6, GPIO_PIN_SET, GPIO_PIN_RESET},      // 1
        {GPIOC, GPIO_PIN_5, GPIO_PIN_SET, GPIO_PIN_RESET},      // 2
        {GPIOA, GPIO_PIN_12, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOA, GPIO_PIN_11, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOB, GPIO_PIN_1, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOB, GPIO_PIN_11, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOB, GPIO_PIN_2, GPIO_PIN_SET, GPIO_PIN_RESET}       // 7
    };




void ledOn(uint8_t num)
{
  for(uint8_t i = 0; i < num; i++)    // led를 num개 만큼 켜는거
  {
    HAL_GPIO_WritePin(led[i].port, led[i].number, led[i].onState);
  }
}

void ledOff(uint8_t num)
{
  for(uint8_t i = 0; i < num; i++)    // led를 num개 만큼 끄는거
    {
      HAL_GPIO_WritePin(led[i].port, led[i].number, led[i].offState);
    }
}

void ledSingleOn(uint8_t num)
{
    HAL_GPIO_WritePin(led[num].port, led[num].number, led[num].onState);
}

void ledSingleOff(uint8_t num)
{
    HAL_GPIO_WritePin(led[num].port, led[num].number, led[num].offState);
}

void ledToggle(uint8_t num)   // 지정된 핀만 토글
{
  HAL_GPIO_TogglePin(led[num].port, led[num].number);
}

void ledLeftShift(uint8_t num)
{
  for(uint8_t i = 0; i <= num; i++)
  {
    ledOn(i);
    HAL_Delay(100);
  }
  HAL_Delay(500);
  for(uint8_t i = 0; i <= num; i++)
  {
    ledOff(i);			//
    HAL_Delay(100);
  }
  HAL_Delay(500);
}

void ledRightShift(uint8_t num)
{
	ledOn(num);
	HAL_Delay(500);

	for (int i = (int) num - 1; i >= 0; i--) {
		ledSingleOff((uint8_t) (i));
		HAL_Delay(100);
	}
	HAL_Delay(500);

	for (int i = (int) num - 1; i >= 0; i--) {
		ledSingleOn((uint8_t) (i));
		HAL_Delay(100);
	}
	HAL_Delay(500);
}

void ledLeftSingleShift(uint8_t num) {
	ledOff(num);
	HAL_Delay(500);

	for (int i = 0; i < num; i++) {
		ledSingleOn(i);
		HAL_Delay(500);
		ledSingleOff(i);
	}
}

void ledRightSingleShift(uint8_t num) {
	ledOff(num);
	HAL_Delay(500);

	for (int i = (int) num - 1; i >= 0; i--) {
		ledSingleOn((uint8_t) (i));
		HAL_Delay(500);
		ledSingleOff((uint8_t) (i));
	}
}

void ledMiddleShift(uint8_t num) {
	ledOff(num);
	HAL_Delay(500);
	for (int i = 0; i < 4; i++) {
			ledSingleOn((uint8_t) (3-i));
			ledSingleOn((uint8_t) (4+i));
			HAL_Delay(500);
			ledSingleOff((uint8_t) (3-i));
			ledSingleOff((uint8_t) (4+i));
		}

}

void ledEdgeShift(uint8_t num) {
	ledOff(num);
	HAL_Delay(500);
	for (int i = 0; i < 4; i++) {
			ledSingleOn((uint8_t) (i));
			ledSingleOn((uint8_t) (7-i));
			HAL_Delay(500);
			ledSingleOff((uint8_t) (i));
			ledSingleOff((uint8_t) (7-i));
		}

}
