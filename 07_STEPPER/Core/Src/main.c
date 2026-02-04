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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stepper.h"   // 사용자 스텝모터 제어 함수/매크로(DIR_CW, DIR_CCW, rotateDegrees 등)
#include <string.h>    // strncmp 등 문자열 비교
#include <stdlib.h>    // atoi: 문자열 -> 정수 변환

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* (사용자 typedef 필요 시 여기에 작성) */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* (사용자 define 필요 시 여기에 작성) */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* (사용자 macro 필요 시 여기에 작성) */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/**
 * rxData:
 *  - UART로 들어온 한 줄(line) 명령을 저장하는 버퍼
 *  - 예: "CW 180" / "CCW 90" 같은 문자열이 여기에 누적됨
 *
 * 크기 256:
 *  - 한 줄 명령 최대 길이를 255바이트로 제한(마지막 '\0' 포함해야 하므로)
 */
uint8_t rxData[256];

/**
 * idx:
 *  - rxData에 몇 글자 쌓였는지 가리키는 인덱스
 *  - interrupt callback(수신 콜백)에서 계속 증가함

 */
uint16_t idx = 0;

/**
 * rxChar:
 *  - 인터럽트로 1바이트 수신할 때 그 1바이트를 저장하는 변수
 *  - HAL_UART_Receive_IT(&huart2, &rxChar, 1) 로 매번 1byte씩 받음
 */
uint8_t rxChar;

/**
 * Flag:
 *  - "한 줄 명령 수신 완료" 신호
 *  - 콜백(인터럽트)에서 '\r' 또는 '\n'을 만나고 idx>0이면 Flag=1로 세팅
 *  - main 루프에서 Flag를 보고 파싱/모터제어 수행 후 Flag=0으로 클리어
 */
uint8_t Flag = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

/**
 * @brief UART 수신 완료 인터럽트 콜백
 *
 * 동작 개요:
 *  - USART2에서 1바이트 수신될 때마다 이 콜백이 호출됨
 *  - rxChar에 들어온 문자를 보고:
 *      1) '\r' 또는 '\n'이면 "한 줄 끝"으로 판단
 *         - idx>0 (빈 줄이 아니면) rxData를 문자열로 마감('\0')하고 Flag=1
 *         - idx를 0으로 되돌려 다음 줄을 받을 준비
 *      2) 일반 문자면 rxData[idx]에 저장하고 idx 증가
 *  - 마지막에 HAL_UART_Receive_IT()를 다시 호출해서 다음 1바이트 수신을 재등록
 *
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 여러 UART가 있을 수 있으니 "USART2 수신"일 때만 처리
    if (huart->Instance == USART2)
    {
        // 줄바꿈 문자를 만나면 한 줄 입력이 끝난 것으로 처리
        if (rxChar == '\r' || rxChar == '\n')
        {
            // 빈 줄(연속 줄바꿈 등)은 무시: idx>0인 경우만 명령으로 인정
            if (idx > 0)
            {
                // C 문자열 처리를 위해 널 종료 문자 추가
                rxData[idx] = '\0';

                // main 루프에 "명령 하나 도착"을 알림
                Flag = 1;

                // 다음 명령을 새로 받기 위해 인덱스 초기화
                idx = 0;
            }
        }
        else
        {
            // 줄바꿈이 아닌 일반 문자라면 버퍼에 누적
            if (idx < sizeof(rxData) - 1)
            {
                // 버퍼에 저장하고 다음 위치로 이동
                rxData[idx++] = rxChar;
            }
            else
            {
            	//Overflow 방지
                idx = 0;
            }
        }

        // ★ 핵심: 다음 1바이트 수신 인터럽트를 다시 걸어줘야 계속 수신됨
        // 이 호출이 없으면 1바이트 받고 끝나버림
        HAL_UART_Receive_IT(&huart2, &rxChar, 1);
    }
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* (main 이전에 사용자 함수/변수 필요 시 여기에 작성) */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* (HAL_Init 전에 필요한 사용자 초기화가 있으면 여기에) */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /**
   * HAL_Init():
   *  - HAL 라이브러리 초기화
   *  - SysTick(1ms tick) 설정
   *  - NVIC priority 그룹 등 기본 설정
   */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* (추가 초기화 필요 시 여기에) */
  /* USER CODE END Init */

  /**
   * 시스템 클럭 설정:
   *  - PLL 등 설정을 통해 CPU/버스 클럭을 구성
   *  - 타이밍(Delay, UART baud 등)에 직접 영향
   */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* (클럭 설정 직후 추가 설정 필요 시 여기에) */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  /**
   * GPIO 초기화:
   *  - CubeMX에서 설정한 핀 모드/풀업/속도/출력상태 적용
   */
  MX_GPIO_Init();

  /**
   * USART2 초기화:
   *  - CubeMX 설정 기반으로 baudrate, word length, stop bit 등 적용
   */
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */

  /**
   * UART 인터럽트 수신 시작:
   *  - 1바이트 수신 완료 시 HAL_UART_RxCpltCallback()이 호출됨
   *  - 이후 콜백 안에서 다시 Receive_IT를 걸어 연속 수신 유지
   */
  HAL_UART_Receive_IT(&huart2, &rxChar, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      /**
       * Flag == 1:
       *  - 콜백에서 "한 줄 수신 완료"로 만들어준 상태
       *  - 여기서 문자열을 파싱하고 스텝모터 동작 수행
       */
	  if (Flag)
	  {
	      // 일단 플래그를 즉시 내림(중복 처리 방지)
	      // (주의) 처리 중 새 명령이 들어오면 Flag가 다시 1이 될 수 있음.
	      Flag = 0;

	      uint8_t direction;  // DIR_CW / DIR_CCW 저장
	      int value;          // "CW 180"이면 180 같은 숫자 값

	      /**
	       * 명령 포맷(예시):
	       *  - "CW 180"
	       *  - "CCW 90"
	       *
	       * strncmp:
	       *  - 문자열 앞부분이 특정 패턴인지 비교
	       *  - "CW "는 앞 3글자, "CCW "는 앞 4글자를 비교
	       */

	      // "CW "로 시작하면 시계방향
	      if (strncmp((char *)rxData, "CW ", 3) == 0)
	      {
	          direction = DIR_CW;

	          // "CW " 다음부터 숫자 문자열이 오므로 +3
	          // 예: "CW 180" -> atoi("180") -> 180
	          value = atoi((char *)rxData + 3);

	          // value도를 direction 방향으로 회전
	          // (rotateDegrees 내부가 blocking이면 이 동안 다음 명령 처리는 늦어짐)
	          rotateDegrees(value, direction);
	      }
	      // "CCW "로 시작하면 반시계방향
	      else if (strncmp((char *)rxData, "CCW ", 4) == 0)
	      {
	          direction = DIR_CCW;

	          // "CCW " 다음부터 숫자 파싱이므로 +4
	          value = atoi((char *)rxData + 4);

	          rotateDegrees(value, direction);
	      }
	      else
	      {
	          /**
	           * 정의되지 않은 명령이면 여기로 옴
	           * 필요하면:
	           *  - "ERR\r\n" 같은 응답을 UART로 보내기
	           *  - 사용 가능한 명령 안내 출력
	           */
	      }
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      // 메인 루프에서 계속 반복 수행할 코드가 있으면 여기에 추가
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  *
  * 클럭 구성 요약(현재 설정 기준):
  *  - HSE = BYPASS (외부 클럭 입력, 보드에서 MCO나 외부 오실레이터가 들어오는 형태)
  *  - PLL 사용: PLLM=4, PLLN=100, PLLP=2
  *  - SYSCLK = PLLCLK
  *  - AHB = SYSCLK/1
  *  - APB1 = HCLK/2, APB2 = HCLK/1
  *
  * (정확한 주파수는 HSE 입력 주파수에 따라 달라집니다.)
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // 전원 컨트롤 클럭 활성화(전압 스케일 설정을 위해 필요)
  __HAL_RCC_PWR_CLK_ENABLE();

  // 전압 스케일 설정(클럭 속도에 맞는 전력/성능 설정)
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /**
   * RCC 오실레이터/PLL 설정
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE; // 외부 고속 클럭(HSE) 사용
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;              // 크리스탈이 아니라 외부 클럭 입력을 bypass로 받음
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;              // PLL 사용
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;      // PLL 입력은 HSE
  RCC_OscInitStruct.PLL.PLLM = 4;                           // PLL 입력 분주
  RCC_OscInitStruct.PLL.PLLN = 100;                         // PLL 배수
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;               // PLL 출력 분주(시스템 클럭용)
  RCC_OscInitStruct.PLL.PLLQ = 4;                           // USB/SDIO/RNG 등에 쓰는 분주 값(필요 시)
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /**
   * 버스 클럭 설정(AHB/APB)
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // SYSCLK=PLLCLK
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;        // HCLK=SYSCLK/1
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;         // PCLK1=HCLK/2 (저속 APB)
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;         // PCLK2=HCLK/1 (고속 APB)

  // FLASH latency(대기 사이클)도 클럭에 맞게 설정해야 안정적으로 동작
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/* (추가 사용자 코드 필요 시 여기에) */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  *
  * 에러 발생 시:
  *  - 인터럽트 비활성화 후 무한 루프
  *  - 디버깅 시 여기 브레이크 걸어 원인 추적
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
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
  *
  * assert 실패 시 호출(디버그용)
  * printf로 출력하려면 retargeting 설정(USART 등)이 추가로 필요
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
