/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : HC-SR04 초음파 센서 + TIM Input Capture + I2C LCD 출력
  *
  * 동작 개요
  *  1) TRIG 핀에 10us 펄스를 줘서 HC-SR04 측정 시작
  *  2) ECHO 핀의 High 펄스 폭을 TIM3 CH1 Input Capture로 측정
  *     - 상승엣지: High 시작 시점 캡처
  *     - 하강엣지: High 끝 시점 캡처
  *  3) echoTime(=High 시간)을 거리(cm)로 변환 → LCD에 표시
  *
  * 중요 전제
  *  - echoTime이 "us 단위"가 되려면 TIM3의 tick이 1us가 되도록 PSC 설정이 필요
  *    (예: TIM3 clock = 100MHz라면 PSC=99 → 1MHz tick → 1us/count)
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"      // sprintf 사용(거리값 문자열 변환)
#include "delay.h"      // delay_us 사용(TIM11 기반 구현일 가능성 높음)
#include "i2c_lcd.h"    // i2c_lcd_init, move_cursor, lcd_string 등 LCD 함수
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/**
 * HC-SR04 TRIG 출력 핀 정의
 * - TRIG: MCU가 10us 펄스를 출력하는 핀
 * - ECHO: 입력캡처(TIM3 CH1)로 들어오는 핀 (여기 코드엔 정의 없고 CubeMX에서 설정되어 있어야 함)
 */
#define TRIG_PORT GPIOA
#define TRIG_PIN  GPIO_PIN_5

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/**
 * printf를 UART로 출력하기 위한 리타겟 설정
 * - GCC 환경에서는 __io_putchar()를 구현하면 printf가 이를 사용
 */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int  __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int  fputc(int ch, FILE *f)
#endif /* __GNUC__*/

/**
 * @brief printf 출력문자를 USART2로 전송하는 함수
 * @details
 * - printf("...\n") 출력 시 터미널에서 줄바꿈이 정상 동작하도록
 *   '\n' 앞에 '\r'을 한 번 더 보내 CRLF 형태로 맞춤
 *
 * 코드 흐름
 *  1) ch가 '\n'이면 '\r'을 먼저 보냄
 *  2) ch 문자 자체를 UART로 전송
 *  3) 리턴 ch (표준 fputc 스타일)
 */
PUTCHAR_PROTOTYPE
{
  if(ch == '\n')
  {
    HAL_UART_Transmit(&huart2, (uint8_t*) "\r", 1, 0xFFFF);
  }

  HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, 0xFFFF);

  // printf 내부에서 성공 여부 판단용으로 리턴해주는 게 정석
  return ch;
}

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/**
 * 입력 캡처값 저장 변수
 * - IC_Value1: ECHO 상승엣지(High 시작)에서 캡처한 타이머 CNT 값
 * - IC_Value2: ECHO 하강엣지(High 종료)에서 캡처한 타이머 CNT 값
 */
uint16_t IC_Value1 = 0;
uint16_t IC_Value2 = 0;

/**
 * echoTime: ECHO가 High였던 시간(펄스폭)
 * captureFlag:
 *   0 -> 상승엣지 캡처 대기 상태
 *   1 -> 하강엣지 캡처 대기 상태
 *
 * distance:
 *   echoTime를 cm로 변환한 값
 *   주의: uint8_t라 최대 255cm까지만 표시 가능
 *
 * ※ 권장:
 * distance/echoTime 등 ISR에서 갱신되고 main에서 읽는 변수는 volatile 추천
 * 예) volatile uint8_t distance;
 */
uint16_t echoTime = 0;
uint16_t captureFlag = 0;
uint8_t  distance = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

/**
 * @brief HC-SR04 TRIG 펄스를 생성하고, CC1 인터럽트를 enable 하는 함수
 * @details
 * - HC-SR04는 TRIG에 10us High 펄스를 주면 측정을 시작
 * - 그 이후 ECHO가 High로 올라갔다 내려오며 그 High 폭이 거리 정보
 *
 * - __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC1):
 *   이번 측정 구간에서만 CC1 인터럽트를 허용하여
 *   불필요한 인터럽트 발생(노이즈/유휴시)을 줄이는 용도
 */
void HCSR04_TRG(void)
{
  // TRIG 안정화를 위해 먼저 Low
  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
  delay_us(1);

  // TRIG High 10us 펄스
  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
  delay_us(10);

  // 펄스 종료(다시 Low)
  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

  // CC1 interrupt enable: 캡처 이벤트 발생 시 콜백 들어오게 함
  __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC1);
}

/**
 * @brief TIM Input Capture 콜백 함수
 * @details
 * - TIM3 CH1에서 캡처가 발생할 때마다 호출됨
 *
 * 동작 방식(상승→하강 두 번 캡처):
 *  1) captureFlag=0 일 때:
 *     - 상승엣지 시점의 CNT값(IC_Value1)을 저장
 *     - 다음 캡처는 하강엣지에서 잡도록 폴라리티를 FALLING으로 변경
 *
 *  2) captureFlag=1 일 때:
 *     - 하강엣지 시점의 CNT값(IC_Value2)을 저장
 *     - IC_Value2 - IC_Value1로 펄스폭 echoTime 계산(오버플로 고려)
 *     - echoTime을 거리로 변환(distance = echoTime/58)
 *     - 다음 측정을 위해 폴라리티를 RISING으로 복구
 *     - 측정이 끝났으니 CC1 인터럽트를 disable (다음 TRIG 때 다시 enable)
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  // 현재 활성 캡처 채널이 CH1인지 확인
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    // 1) 상승엣지 캡처 단계
    if(captureFlag == 0)
    {
      // 상승엣지에서 캡처된 타이머 CNT값 저장
      IC_Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

      // 다음은 하강엣지를 잡을 차례
      captureFlag = 1;

      // 하강엣지 캡처로 폴라리티 변경
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
    }
    // 2) 하강엣지 캡처 단계
    else if(captureFlag == 1)
    {
      // 하강엣지에서 캡처된 타이머 CNT값 저장
      IC_Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

      // 다음 측정 대비 CNT를 0으로 리셋 (선택사항이지만 재측정 안정화)
      __HAL_TIM_SET_COUNTER(htim, 0);

      // 오버플로 없이 정상적으로 증가한 경우
      if(IC_Value2 > IC_Value1)
      {
        echoTime = IC_Value2 - IC_Value1;
      }
      // 타이머가 중간에 오버플로우(0xFFFF->0) 된 경우
      else if(IC_Value1 > IC_Value2)
      {
        echoTime = (0xffff - IC_Value1) + IC_Value2;
      }

      /**
       * 거리 변환 공식
       * - HC-SR04: ECHO High 시간(us) 기준
       *   distance(cm) = echoTime / 58
       *
       * ※ echoTime이 us 단위라는 전제 하에 맞는 공식
       */
      distance = echoTime / 58;

      // 다음 측정은 다시 상승엣지부터 시작
      captureFlag = 0;

      // 폴라리티 원복(상승엣지 캡처로 변경)
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);

      // 이번 측정 완료 → CC1 인터럽트 끄기(다음 TRIG 때 켬)
      __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
    }
  }
}

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
/* 필요 시 사용자 함수/전역 변수 추가 영역 */
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();            // HAL 및 SysTick 등 초기화
  SystemClock_Config();  // 시스템 클럭 설정

  // 주변장치 초기화(CubeMX 생성 함수)
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM11_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */

  /**
   * TIM11 시작: delay_us()가 TIM11 카운터 기반으로 동작한다는 전제
   * - delay_us가 busy-wait로 TIM11 CNT 변화를 보는 방식일 가능성 높음
   */
  HAL_TIM_Base_Start(&htim11);

  /**
   * TIM3 Input Capture 인터럽트 시작
   * - 채널1(ECHO)을 캡처하도록 시작
   * - 실제로는 CC1 인터럽트를 TRG 때 enable/완료 후 disable 하는 구조를 함께 사용
   */
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);

  // I2C LCD 초기화
  i2c_lcd_init();

  // LCD 출력용 버퍼(거리값 문자열을 담는다)
  char lcd_buf[20];

  /* USER CODE END 2 */

  while (1)
  {
    /**
     * 초음파 측정 시작
     * - TRIG 펄스 출력
     * - 이후 ECHO 상승/하강이 들어오면 인터럽트 콜백에서 distance가 갱신됨
     */
    HCSR04_TRG();

    /**
     * distance(숫자)를 문자열로 변환
     * - lcd_string은 printf가 아니라서 "완성된 문자열"만 넣어야 함
     * - sprintf로 미리 문자열을 만들어서 전달
     */
    sprintf(lcd_buf, " %3d cm", distance);

    // 1행(0행) 첫 칸에 "Distance" 출력
    move_cursor(0, 0);
    lcd_string("Distance");

    // 2행(1행) 첫 칸에 측정값 출력
    move_cursor(1, 0);
    lcd_string(lcd_buf);

    // 측정 주기(너무 짧으면 이전 측정과 겹치거나 노이즈 증가 가능)
    HAL_Delay(500);



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
