/*
 * led.h
 *
 *  Created on: Jan 22, 2026
 *      Author: mujukpopo
 */

#ifndef INC_LED_H_
#define INC_LED_H_


//#include "main.h"
#include "stm32f1xx_hal.h"




typedef struct
{
  GPIO_TypeDef  *port;
  uint16_t      number;
  GPIO_PinState onState;
  GPIO_PinState offState;
}LED_CONTROL;


void ledOn(uint8_t num);
void ledOff(uint8_t num);
void ledSingleOn(uint8_t num);
void ledSingleOff(uint8_t num);
void ledToggle(uint8_t num);

void ledLeftShift(uint8_t num);
void ledRightShift(uint8_t num);
void ledLeftSingleShift(uint8_t num);
void ledRightSingleShift(uint8_t num);
void ledMiddleShift(uint8_t num);
void ledEdgeShift(uint8_t num);

void ledLeft(uint8_t *firstPress, uint8_t *firstPress2, uint8_t *i);
void ledRight(uint8_t *firstPress, uint8_t *firstPress2, uint8_t *i);






#endif /* INC_LED_H_ */
