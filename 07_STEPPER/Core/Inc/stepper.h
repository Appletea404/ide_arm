///*
// * stepper.h
// *
// *  Created on: Feb 4, 2026
// *      Author: appletea
// */
#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_


#include "stm32f4xx_hal.h"


#define STEPS_PER_REVOLATION  4096
#define DIR_CW                0   // 시계 방향
#define DIR_CCW               1   // 반시계 방항

#define IN1_PIN     GPIO_PIN_1
#define IN2_PIN     GPIO_PIN_15
#define IN3_PIN     GPIO_PIN_14
#define IN4_PIN     GPIO_PIN_13

#define IN1_PORT    GPIOB
#define IN2_PORT    GPIOB
#define IN3_PORT    GPIOB
#define IN4_PORT    GPIOB

//#define IN_PORT    GPIOB


void stepMotor(uint8_t step);
void rotateSteps(uint16_t steps, uint8_t direction);
void rotateDegrees(uint16_t degrees, uint8_t direction);




#endif /* INC_STEPPER_H_ */

//
//#ifndef INC_STEPPER_H_
//#define INC_STEPPER_H_
//
//#include "stm32f4xx_hal.h"
//
//#define STEPS_PER_REVOLUTION	4096
//#define DIR_CW					0			//시계방향
//#define DIR_CCW					1			//반시계방향
//
//#define IN1_PIN					GPIO_PIN_1
//#define IN2_PIN					GPIO_PIN_15
//#define IN3_PIN					GPIO_PIN_14
//#define IN4_PIN					GPIO_PIN_13
//
//#define IN_PORT					GPIOB
//
//void stepMotor(uint8_t step);
//void rotateSteps(uint16_t steps, uint8_t direction);
//void rotateDegrees(uint16_t degrees, uint8_t direction);
//
//
//
//
//#endif /* INC_STEPPER_H_ */
