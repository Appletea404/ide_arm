/*
 * delay.c
 *
 *  Created on: Jan 27, 2026
 *      Author: appletea
 */


#include "delay.h"

void delay_us(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim10,0);
	while((__HAL_TIM_GET_COUNTER(&htim10)) < us);
}
