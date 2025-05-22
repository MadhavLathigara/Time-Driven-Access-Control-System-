//project_defines.h
#include "types.h"
//delay
void delay_us(u32 dlyUS);
void delay_ms(u32 dlyMS);
void delay_s(u32 dlyS);

//pin connect block

void cfgPortPinFunc(u32 PortNo, u32 PinNo, u32 PinF);

//keypad
u32 colscan(void);
u32 rowcheck(void);
u32 colcheck(void);
u32 keyscan(void);

void InitKPM(void);
#define ROW0 16 // @p1.16 
#define ROW1 17 // @p1.17 
#define ROW2 18 // @p1.18 
#define ROW3 19 // @p1.19
#define COL0 20 // @p1.20
#define COL1 21 // @p1.21
#define COL2 22 // @p1.22
#define COL3 23 // @p1.23

//lcd 
void WriteLCD(u8 Dat);
void CmdLCD(u8 cmd);
void CharLCD(u8 ascii);
void InitLCD(void);
void StrLCD(s8 *);
void U32LCD(u32);
void S32LCD(s32);
void F32LCD(f32,u32);
void HexLCD(u32);
void OctLCD(u32);
void BinLCD(u32,u32);
void BuildCGRAM(u8*,u8);

#define CLEAR_LCD 0x01
#define RET_CUR_HOME 0x02
#define SHIFT_CUR_RIGHT 0x06
#define SHIFT_CUR_LEFT 0x07
#define DSP_OFF 0x08
#define DSP_ON_CUR_OFF 0x0C
#define DSP_ON_CUR_ON 0x0E
#define DSP_ON_CUR_BLK 0x0F
#define SHIFT_DSP_LEFT 0x10
#define SHIFT_DSP_RIGHT 0x14
#define MODE_8BIT_1LINE 0x30
#define MODE_4BIT_1LINE 0x20
#define MODE_8BIT_2LINE 0x38
#define MODE_4BIT_2LINE 0x28
#define GOTO_LINE1_POS0 0x80
#define GOTO_LINE2_POS0 0xC0
#define GOTO_LINE3_POS0 0x94
#define GOTO_LINE4_POS0 0xD4
#define GOTO_CGRAM_START 0x40
#define LCD_DATA 8 //P0.8 TO 15 A TO DP
#define LCD_RS 16
#define LCD_RW 17
#define LCD_EN 18

//rtc
void RTC_Init(void);
void GetRTCTime(s32 *,s32 *,s32 *);
void DisplayRTCTime(u32,u32,u32);
void GetRTCDate(s32 *,s32 *,s32 *);
void DisplayRTCDate(u32,u32,u32);

void SetRTCTime(u32,u32,u32);
void SetRTCDate(u32,u32,u32);

void GetRTCDay(s32 *);
void DisplayRTCDay(u32);
void SetRTCDay(u32);

void SetRTCHour(u32);
void SetRTCMin(u32);
void SetRTCSec(u32);
void SetRTCMonth(u32);
void SetRTCYear(u32);
void SetRTCDay(u32);

void GetRTCHour(s32 *);
void GetRTCMin(s32 *);
void GetRTCSec(s32 *);
void GetRTCMonth(s32 *);
void GetRTCYear(s32 *);
void GetDay(void);

// System clock and peripheral clock Macros
#define FOSC	12000000 
#define CCLK  (5*FOSC)   	
#define PCLK  (CCLK/4)

// RTC Macros
#define PREINT_VAL	 (int)((PCLK / 32768) - 1) 
#define PREFRAC_VAL  (PCLK -((PREINT + 1) * 32768))
#define RTC_ENABLE	  (1<<0)  
#define RTC_RESET     (1<<1) 
#define RTC_CLKSRC_EX (1<<4)

