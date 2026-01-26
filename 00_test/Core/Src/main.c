/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DEBOUNCE_DELAY 50		// 50ms

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

uint32_t millis(){
	return HAL_GetTick();
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

//  uint32_t lastDebounceTime1 = 0;		// 마지막 디바운스 확인시간
//  uint32_t lastDebounceTime2 = 0;
//  GPIO_PinState lastButtonState1 = GPIO_PIN_SET;
//  GPIO_PinState lastButtonState2 = GPIO_PIN_SET;


//  LED_CONTROL led2[8]={
//  		{GPIOC, GPIO_PIN_8, GPIO_PIN_SET, GPIO_PIN_RESET},
//  		{GPIOC, GPIO_PIN_6, GPIO_PIN_SET, GPIO_PIN_RESET},
//  		{GPIOC, GPIO_PIN_5, GPIO_PIN_SET, GPIO_PIN_RESET},
//  		{GPIOA, GPIO_PIN_12, GPIO_PIN_SET, GPIO_PIN_RESET},
//  		{GPIOA, GPIO_PIN_11, GPIO_PIN_SET, GPIO_PIN_RESET},
//  		{GPIOB, GPIO_PIN_1, GPIO_PIN_SET, GPIO_PIN_RESET},
//  		{GPIOB, GPIO_PIN_11, GPIO_PIN_SET, GPIO_PIN_RESET},
//  		{GPIOB, GPIO_PIN_2, GPIO_PIN_SET, GPIO_PIN_RESET}
//  };
  uint8_t i = 0;
  static uint8_t firstPress = 1;
  static uint8_t firstPress2 = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  GPIO_PinState currentButtonSate1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9);
//	  GPIO_PinState currentButtonSate2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);

//	  // 상태가 변했는지?
//	  if(currentButtonSate1 != lastButtonState1){
//		  // 디바운스 시간 까지 잠시대기
//		  if(millis() - lastDebounceTime1 > DEBOUNCE_DELAY){
//			  /* ================= 정방향 버튼 ================= */
//			  if(currentButtonSate1 == GPIO_PIN_RESET){
	  	  if(buttonGetPressed(0))
	  	  {
	  		ledLeft(&firstPress, &firstPress2, &i);
	  	  }



//				  if (firstPress){
//					    HAL_GPIO_WritePin(led2[0].port, led2[0].number, led2[0].onState);
//					    firstPress = 0;
//					    if(firstPress2){
//							firstPress2 = 0;
//						}
//					}
//				  else{
//				  // 현재 LED OFF
//				  HAL_GPIO_WritePin(led2[i].port,led2[i].number,led2[i].offState);
//
//	            // 다음 인덱스 (0~7 순환)
//	            i = (i + 1) % 8;
//
//	            // 다음 LED ON
//	            HAL_GPIO_WritePin(led2[i].port,led2[i].number,led2[i].onState);
//				  }
//			  }
//
//			  lastDebounceTime1 = millis();
//		  }
//	  }
//	  lastButtonState1 = currentButtonSate1;
//	  if(currentButtonSate2 != lastButtonState2){
		  // 디바운스 시간 까지 잠시대기
//		  if(millis() - lastDebounceTime2 > DEBOUNCE_DELAY){
//			  /* ================= 역방향 버튼 ================= */
//			  if(currentButtonSate2 == GPIO_PIN_RESET){
		if(buttonGetPressed(1)){
			ledRight(&firstPress, &firstPress2, &i);
		}

//
////				  if (firstPress2){
////						HAL_GPIO_WritePin(led2[7].port, led2[7].number, led2[7].onState);
////						firstPress2 = 0;
////						if(firstPress){
////							firstPress = 0;
////						}
////					}
////				  else{
////					  HAL_GPIO_WritePin(led2[i].port,led2[i].number,led2[i].offState);
////
////					  // 이전 LED (역방향)
////					  i = (i + 7) % 8;
////
////					  // 이전 LED ON
////					  HAL_GPIO_WritePin(led2[i].port,led2[i].number,led2[i].onState);
////				  }
//			  }
//		  }
//	  }
//	  lastButtonState2 = currentButtonSate2;
}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
