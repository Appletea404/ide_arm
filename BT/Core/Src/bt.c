/*
 * bt.c
 *
 *  Created on: Jan 28, 2026
 *      Author: appletea
 */
#include "bt.h"

uint8_t rxData[10];


//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	HAL_UART_Receive_IT(&huart1, rxData, 10);
//}

void Check()
{
	if(rxData[0] == '0')
		  {
			  ledOn(8);
		  }
		  if(rxData[0] =='1')
		  {
			  ledOff(8);
		  }

}
