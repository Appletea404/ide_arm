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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define SER_PIN GPIO_PIN_7
#define SRCLK_PIN GPIO_PIN_8
#define RCLK_PIN GPIO_PIN_9
#define GPIO_PORT GPIOC


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

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

void delay_us(uint16_t us)
{
	uint16_t start = __HAL_TIM_GET_COUNTER(&htim11);
	while((__HAL_TIM_GET_COUNTER(&htim11) - start) < us);
}

//HAL_gettick을 안쓰는 이유?



/*
 * dataOut()
 *
 * SN74HC595(8bit Shift Register + Latch)에
 * 1바이트(8비트) 데이터를 직렬로 전송한 뒤,
 * 래치 클럭을 통해 출력(QA~QH)에 한 번에 반영하는 함수
 *
 * 핀 역할 요약
 * ------------------------------------------------
 * SER   : 직렬 데이터 입력 (DS)
 * SRCLK : 시프트 클럭 (SH_CP)
 *         → 상승엣지에서 SER 값이 내부 쉬프트 레지스터로 이동
 * RCLK  : 래치 클럭 (ST_CP)
 *         → 상승엣지에서 쉬프트 레지스터 값이 출력 래치로 복사
 *
 * 동작 흐름
 * ------------------------------------------------
 * 1. data의 MSB(bit7)부터 LSB(bit0)까지 한 비트씩 처리
 * 2. 현재 비트를 SER 핀에 출력
 * 3. SRCLK을 토글하여 해당 비트를 쉬프트 레지스터로 밀어넣음
 * 4. 총 8비트 전송 완료 후
 * 5. RCLK을 토글하여 출력 QA~QH에 한 번에 반영
 *
 * 특징
 * ------------------------------------------------
 * - MSB First 방식
 * - GPIO bit-banging 방식 (SPI 미사용)
 * - 래치 구조 덕분에 전송 중 출력이 변하지 않아 LED 깜빡임 없음
 */
/*
 * dataOut()
 *
 * SN74HC595(8bit Shift Register + Latch)에
 * 1바이트(8비트) 데이터를 직렬로 전송한 뒤,
 * 래치 클럭을 통해 출력(QA~QH)에 한 번에 반영하는 함수
 *
 * 핀 역할 요약
 * ------------------------------------------------
 * SER   : 직렬 데이터 입력 (DS)
 * SRCLK : 시프트 클럭 (SH_CP)
 *         → 상승엣지에서 SER 값이 내부 쉬프트 레지스터로 이동
 * RCLK  : 래치 클럭 (ST_CP)
 *         → 상승엣지에서 쉬프트 레지스터 값이 출력 래치로 복사
 *
 * 동작 흐름
 * ------------------------------------------------
 * 1. data의 MSB(bit7)부터 LSB(bit0)까지 한 비트씩 처리
 * 2. 현재 비트를 SER 핀에 출력
 * 3. SRCLK을 토글하여 해당 비트를 쉬프트 레지스터로 밀어넣음
 * 4. 총 8비트 전송 완료 후
 * 5. RCLK을 토글하여 출력 QA~QH에 한 번에 반영
 *
 * 특징
 * ------------------------------------------------
 * - MSB First 방식
 * - GPIO bit-banging 방식 (SPI 미사용)
 * - 래치 구조 덕분에 전송 중 출력이 변하지 않아 LED 깜빡임 없음
 */
void dataOut(uint8_t data)
{
    /*
     * i = 7 → 0
     * MSB부터 LSB까지 순차적으로 전송
     */
	for(int i = 7; i >= 0; i--)
	{
        /*
         * 현재 전송할 비트를 SER 핀에 출력
         * data의 i번째 비트가 1이면 HIGH, 0이면 LOW
         *
         * 이 시점에서는 아직 74HC595 내부로 들어가지 않고
         * 단순히 입력 핀에 값만 세팅된 상태
         */
		if(data & (1 << i))
		{
			HAL_GPIO_WritePin(GPIO_PORT, SER_PIN, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIO_PORT, SER_PIN, GPIO_PIN_RESET);
		}

        /*
         * SRCLK(Shift Clock) 펄스 생성
         *
         * - SRCLK의 상승엣지에서 SER 값이
         *   내부 쉬프트 레지스터로 이동
         * - 이후 비트들은 한 칸씩 밀려남
         *
         * delay_us()는 setup/hold time 확보용 여유
         */
		HAL_GPIO_WritePin(GPIO_PORT, SRCLK_PIN, GPIO_PIN_SET);
		delay_us(5);

		HAL_GPIO_WritePin(GPIO_PORT, SRCLK_PIN, GPIO_PIN_RESET);
		delay_us(5);
	}

    /*
     * RCLK(Latch Clock) 펄스 생성
     *
     * - 지금까지 쉬프트 레지스터에 쌓인 8비트 데이터를
     *   출력 래치로 복사
     * - QA~QH 핀이 이 순간에만 갱신됨
     *
     * 장점:
     * - 데이터 전송 중에는 출력이 바뀌지 않음
     * - LED/FND 사용 시 중간 상태가 보이지 않음
     */
	HAL_GPIO_WritePin(GPIO_PORT, RCLK_PIN, GPIO_PIN_SET);
	delay_us(10);
	HAL_GPIO_WritePin(GPIO_PORT, RCLK_PIN, GPIO_PIN_RESET);
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
  MX_USART2_UART_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start(&htim11);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  for(uint8_t i = 0; i < 8; i++)
	  {
		  dataOut(1 << i);
		  HAL_Delay(500);
	  }



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
