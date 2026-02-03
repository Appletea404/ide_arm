/*
 * lcd_4bit.c
 *
 * HD44780 호환 캐릭터 LCD를 4-bit 모드(D4~D7)로 구동하는 드라이버
 *
 * 핵심 개념
 * - LCD는 원래 8비트 데이터 버스(D0~D7)를 지원하지만, 핀 절약을 위해 4비트 모드(D4~D7)로 사용 가능
 * - 1바이트(8비트)를 상위 4비트(nibble) → 하위 4비트(nibble) 순서로 나눠 전송
 * - RS 핀으로 "명령(Command)"인지 "데이터(문자)"인지 구분
 * - E(Enable) 핀의 펄스(특히 1→0 하강 에지)에서 LCD가 데이터를 래치(latch)하여 내부에 확정
 *
 * 주의
 * - RW는 여기서 사용하지 않음(대부분 실무에서 RW=0 고정 후 delay로 busy flag 대체)
 */

#include "lcd_4bit.h"


/**
 * @brief LCD D4~D7 4개의 데이터 핀에 상위 4비트 데이터를 출력한다.
 *
 * @param data : 반드시 상위 nibble이 유효한 형태(0x10,0x20,0x40,0x80 비트 조합)로 들어오는 것이 일반적.
 *
 * 동작 원리
 * - 4bit 모드에서는 LCD의 D4~D7만 사용한다.
 * - data의 bit4~bit7을 각각 D4~D7에 매핑한다.
 *
 * 예) data=0xA0(1010 0000) 이면
 * - D7=1, D6=0, D5=1, D4=0 으로 출력
 */
void LCD_Data4bit(uint8_t data)
{
    // data의 bit4(0x10)가 1이면 D4 HIGH, 아니면 LOW
    HAL_GPIO_WritePin(LCD_D4_GPIO, LCD_D4_PIN, (data & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // data의 bit5(0x20)가 1이면 D5 HIGH
    HAL_GPIO_WritePin(LCD_D5_GPIO, LCD_D5_PIN, (data & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // data의 bit6(0x40)가 1이면 D6 HIGH
    HAL_GPIO_WritePin(LCD_D6_GPIO, LCD_D6_PIN, (data & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // data의 bit7(0x80)가 1이면 D7 HIGH
    HAL_GPIO_WritePin(LCD_D7_GPIO, LCD_D7_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


/**
 * @brief Enable(E) 핀에 펄스를 주어 LCD가 현재 RS/RW/D4~D7 상태를 읽어가도록 한다.
 *
 * HD44780 규칙(중요)
 * - RS/RW/D4~D7 값은 E가 HIGH인 동안 안정적이어야 한다.
 * - 보통 E의 하강 에지(1→0) 시점에 LCD가 데이터를 래치(latch)한다.
 *
 * 현재 구현 흐름
 * 1) E=0 (안정)
 * 2) E=1 (데이터 유효 구간 시작)
 * 3) E=0 (이 순간 데이터 확정/래치)
 *
 * 딜레이 이유
 * - LCD가 신호를 안정적으로 샘플링할 수 있도록 펄스 폭을 확보
 * - 이 코드에서는 HAL_Delay(ms)를 사용하므로 매우 넉넉한 펄스 폭을 가지게 된다.
 *
 * 참고
 * - 실제 데이터시트 요구는 us 단위인데, ms로 주면 느려지지만 안전하다.
 * - 성능이 필요하면 delay_us로 줄일 수 있다.
 */
void LCD_EnablePin(void)
{
    // E를 LOW로 내려서 시작 (정돈된 상태)
    HAL_GPIO_WritePin(LCD_E_GPIO, LCD_E_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);

    // E HIGH: LCD가 데이터 라인(D4~D7)을 "읽을 준비/유효구간"
    HAL_GPIO_WritePin(LCD_E_GPIO, LCD_E_PIN, GPIO_PIN_SET);
    HAL_Delay(1);

    // E LOW: 하강 에지에서 LCD가 현재 데이터를 내부에 래치함
    HAL_GPIO_WritePin(LCD_E_GPIO, LCD_E_PIN, GPIO_PIN_RESET);

    // 명령에 따라 LCD가 처리할 시간 필요
    // 일반 명령은 짧지만, clear/home 같은 명령은 더 오래 걸림.
    // 여기서는 일괄적으로 2ms 기다려 안전하게 처리하도록 함.
    HAL_Delay(2);
}


/**
 * @brief 1바이트(8bit)를 4bit 모드로 LCD에 전송한다.
 *
 * 4bit 모드 전송 규칙
 * - 상위 4비트 먼저, 하위 4비트 나중
 * - 각 nibble마다 Enable 펄스를 1번씩 줘서 래치해야 한다.
 *
 * @param data : 전송할 8비트(명령 또는 문자 데이터)
 */
void LCD_SendByte(uint8_t data)
{
    // 1) 상위 4비트 전송
    // data & 0xF0 : 상위 nibble만 남기기 (xxxx 0000)
    LCD_Data4bit(data & 0xF0);
    LCD_EnablePin();  // 상위 nibble 래치

    // 2) 하위 4비트 전송
    // (data << 4) & 0xF0 : 하위 nibble을 상위 위치로 올려서 D4~D7로 보내기
    LCD_Data4bit((data << 4) & 0xF0);
    LCD_EnablePin();  // 하위 nibble 래치
}


/**
 * @brief LCD에 "명령어(Command)"를 전송한다.
 *
 * RS 의미
 * - RS=0 : Instruction Register(IR)로 들어감 (명령)
 * - RS=1 : Data Register(DR)로 들어감 (문자 데이터)
 *
 * @param commandData : LCD 명령(예: 0x01 clear, 0x0C display on, 0x28 function set 등)
 */
void LCD_WriteCommand(uint8_t commandData)
{
    // 명령 모드: RS=0
    HAL_GPIO_WritePin(LCD_RS_GPIO, LCD_RS_PIN, GPIO_PIN_RESET);

    // 8bit를 4bit로 쪼개 전송
    LCD_SendByte(commandData);
}


/**
 * @brief LCD에 "문자 데이터(Data)"를 전송한다.
 *
 * @param charData : 표시할 문자(ASCII)
 * 예: 'A', '0', ' ' 등
 */
void LCD_WriteData(uint8_t charData)
{
    // 데이터 모드: RS=1
    HAL_GPIO_WritePin(LCD_RS_GPIO, LCD_RS_PIN, GPIO_PIN_SET);

    // 문자 1바이트 전송
    LCD_SendByte(charData);
}


/**
 * @brief 커서를 (row, col) 위치로 이동한다.
 *
 * HD44780 DDRAM 주소
 * - 1행: 0x00 ~ 0x0F
 * - 2행: 0x40 ~ 0x4F
 *
 * 커서 이동 명령
 * - 0x80 | addr  를 보내면 DDRAM 주소 설정(Set DDRAM Address)
 *
 * @param row : 0(1행), 1(2행)
 * @param col : 0~15 (16x2 기준)
 */
void LCD_GotoXY(uint8_t row, uint8_t col)
{
    // row 범위 제한: 0 또는 1만 허용
    if (row > 1) row = 1;

    // col 범위 제한: 0~15
    if (col > 15) col = 15;

    // 행에 따라 DDRAM 시작 주소가 다름
    // 1행은 0x00부터, 2행은 0x40부터
    uint8_t addr = (row == 0) ? col : (0x40 + col);

    // DDRAM 주소 설정 명령 전송 (0x80 | addr)
    LCD_WriteCommand(0x80 | addr);
}


/**
 * @brief 문자열을 현재 커서 위치부터 출력한다.
 *
 * @param string : null-terminated 문자열
 *
 * 동작
 * - 문자 하나씩 LCD_WriteData()로 전송
 * - LCD는 Entry Mode Set(0x06 설정 시)로 인해 커서가 자동으로 오른쪽으로 이동
 */
void LCD_WriteString(char *string)
{
    while (*string)
    {
        LCD_WriteData(*string++);  // 현재 문자 출력 후 포인터 증가
    }
}


/**
 * @brief (row, col)로 커서를 이동한 후 문자열을 출력한다.
 *
 * @param row : 0 or 1
 * @param col : 0~15
 * @param string : 출력 문자열
 */
void LCD_WriteStringXY(uint8_t row, uint8_t col, char *string)
{
    LCD_GotoXY(row, col);
    LCD_WriteString(string);
}


/**
 * @brief LCD 초기화(Initialization) 시퀀스.
 *
 * 매우 중요: HD44780은 전원 인가 직후 "4bit 모드"가 확정되어 있지 않을 수 있다.
 * 그래서 데이터시트/표준 시퀀스에 따라 8bit 모드 가정으로 0x30을 여러 번 보내고,
 * 이후 0x20을 보내 4bit 모드로 전환한다.
 *
 * 흐름 요약
 * 1) 전원 안정화 대기(>= 40ms 권장, 여기서는 50ms)
 * 2) 0x30을 3번 (8bit 모드 가정으로 초기화 강제)
 * 3) 0x20을 1번 (4bit 모드로 전환)
 * 4) Function set / Display on / Clear / Entry mode 설정
 */
void LCD_Init(void)
{
    // 전원 인가 후 LCD 내부 안정화 시간 확보
    HAL_Delay(50);

    // (1) 0x30 전송: 8bit 모드 기준 초기화 명령(상위 nibble만)
    // 4bit 모드 전환 전에는 "상위 4비트만" 보내는 방식으로 접근한다.
    LCD_Data4bit(0x30);
    LCD_EnablePin();
    HAL_Delay(5);

    // (2) 반복 전송: LCD가 확실히 초기 상태로 들어오도록 함
    LCD_Data4bit(0x30);
    LCD_EnablePin();
    HAL_Delay(1);

    LCD_Data4bit(0x30);
    LCD_EnablePin();

    // (3) 0x20 전송: 4bit 모드로 전환
    LCD_Data4bit(0x20);
    LCD_EnablePin();

    // 이제부터는 LCD가 4bit 모드로 동작하므로 LCD_SendByte/WriteCommand 사용 가능

    // Function Set: 4bit, 2라인, 5x8 dot
    LCD_WriteCommand(0x28);

    // Display ON/OFF Control: Display ON, Cursor OFF, Blink OFF
    LCD_WriteCommand(0x0C);

    // Clear Display: 화면 지우기 (처리 시간이 긴 편)
    LCD_WriteCommand(0x01);
    HAL_Delay(5);

    // Entry Mode Set: 커서 오른쪽 이동, 화면 이동 없음
    LCD_WriteCommand(0x06);
}
