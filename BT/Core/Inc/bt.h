/*
 * bt.h
 *
 *  Created on: Jan 28, 2026
 *      Author: appletea
 */

#ifndef INC_BT_H_
#define INC_BT_H_

#include "main.h"
#include "usart.h"
#include "led.h"

extern uint8_t rxData[10];

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Check();


#endif /* INC_BT_H_ */
