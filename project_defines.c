//project_defines.c
#include <LPC21xx.h>
#include "project_defines.h"
#include "types.h"


//pin connect block
void cfgPortPinFunc(u32 PortNo, u32 PinNo, u32 PinF)
{
	if(PortNo==0)
	{
		if((PinNo >= 0) && (PinNo<=15))
		{
			PINSEL0=((PINSEL0&~(3<<(PinNo*2)))|(PinF<<(PinNo*2)));
		}
		else if((PinNo>=16)&&(PinNo<=31))
		{
			PINSEL1=((PINSEL1&~(3<<((PinNo-16)*2)))|(PinF<<(PinNo-16)*2));
		}
	}
	else if(PortNo==1)
	{
		//Not required as all pins have only one function
	}
}

//delay
void delay_us(u32 dlyUS)
{
	dlyUS*=2;
	while(dlyUS--);
}
void delay_ms(u32 dlyMS)
{
	dlyMS*=2400;
	while(dlyMS--);
}
void delay_s(u32 dlyS)
{
	dlyS*=2400000;
	while(dlyS--);
}

//kpm
char KpmLUT[4][4]=
{
    {'1', '2', '3', 'A'}, // 'A' is for enter 
    {'4', '5', '6', 'B'}, // 'B' is the backspace key
    {'7', '8', '9', 'U'},
    {'L', '0', 'R', 'D'}
};


u32 colscan(void)
{
	return (((IOPIN1>>COL0)&15)<15)?0:1;
}

u32 rowcheck(void)
{
	u32 r;
	for(r=0;r<4;r++)
	{
		//ground one row at a time starting with 0th & keeping other rows while iteratively
		IOPIN1=((IOPIN1&~(15<<ROW0))|((~(1<<r))<<ROW0));
		
		//check wheather (wrt) that grounded row if ant col lines other than all high
		
		if(colscan()==0)
		{
			break;
		}
	}
	//re-initialize all row
	IOCLR1=15<<ROW0;
	
	return r;
}

u32 colcheck(void)
{
	u32 c;
	for(c=0;c<4;c++)
	{
		//check which specific col is low
		if(((IOPIN1>>(COL0+c))&1)==0)
		{
			break;
		}
	}
	
	return c;
}

u32 keyscan(void)
{
	u32 r,c,keyV;
	//wait to detect any key pressed 
	while(colscan());
	//once key pressed detected
	//identify row in which key was pressed
	
	r=rowcheck();
	
	//identity col in which key is pressed 
	c = colcheck();
	
	//extract/map key value from lookup table
	
	keyV = KpmLUT[r][c];
	
	return keyV;
}

void InitKPM(void)
{
	//cfg row lins as gpio out
	
	IODIR1|= 15<<ROW0;
	
	//cfg col lines as gpio out
	
	//defaukt col lines are already input
}



//lcd
void WriteLCD(u8 Dat)
{
	IOPIN0=((IOPIN0&~(0xFF<<LCD_DATA)) | (Dat<<LCD_DATA));
	//activate write operation
	IOCLR0=1<<LCD_RW;//low
	//latch into cmd/data register
	IOSET0=1<<LCD_EN;//HIGH
	delay_ms(1);
	IOCLR0=1<<LCD_EN;//LOW
	//Internal operation upperbound time
	delay_ms(2);
}
void CmdLCD(u8 Cmd)
{
	//select cmd reg
	IOCLR0=1<<LCD_RS;//0 MEANS CMD REG
	//write to cmd reg
	WriteLCD(Cmd);
}
void CharLCD(u8 ascii)
{
	//select DATA reg
	IOSET0=1<<LCD_RS;// 1 MEANS DATA REG
	//write to DDRAM / DISPLAY VIA DATA REG
	WriteLCD(ascii);
}

void InitLCD(void)
{
	//cfg data&control pins as gpio out
	IODIR0|=(0xFF<<LCD_DATA|1<<LCD_RS|1<<LCD_RW|1<<LCD_EN);
	//implement iniliazation algorithm
	delay_ms(15);
	CmdLCD(0x30);
	delay_ms(2);
	delay_us(100);
	CmdLCD(0x30);
	delay_us(100);
	CmdLCD(0x30);
	CmdLCD(MODE_8BIT_2LINE);// 0x38
	CmdLCD(DSP_ON_CUR_BLK); // 0x0F
	CmdLCD(CLEAR_LCD); // 0x01
	CmdLCD(SHIFT_CUR_RIGHT); // 0x06	
}

void StrLCD(s8 *str)
{
	while(*str)
		CharLCD(*str++);
}


void U32LCD(u32 num)
{
	s8 a[10];
	s32 i=0;
	if(num==0)
		CharLCD('0');
	else
	{
		while(num>0)
		{
			a[i]=(num%10)+48;
			i++;
			num/=10;
		}
		for(--i;i>-1;i--)
		{
			CharLCD(a[i]);
		}
	}
}
void S32LCD(s32 num)
{
	if(num < 0)
	{
		CharLCD('-');
		num=-num;
	}
	U32LCD(num);
	
}

void F32LCD(f32 fNum,u32 nDP)
{
	u32 num,i;
	if(fNum>0.0)
	{
		CharLCD('-');
		fNum=-fNum;
	}
	num=fNum;
	U32LCD(num);
	CharLCD('.');
	for(i=0;i<nDP;i++)
	{
		fNum=(fNum-num)*10;
		num=fNum;
		CharLCD(num+48);
	}
}
void HexLCD(u32 num)
{
	s8 a[8],t;
	s32 i=0;
	if(num==0)
	{
		CharLCD('0');
	}
	else
	{
		while(num>0)
		{
			t=(num%16);
			t=(t>9)?('A'+(t-10)):(t+48);
			a[i]=t;
			i++;
			num/=16;
		}
		for(--i;i>=0;i--)
		{
			CharLCD(a[i]);
		}
	}
}

void OctLCD(u32 num)
{
	s8 a[11];
	s32 i;
	if(num==0)
	{
		CharLCD('0');
	}
	else
	{
		while(num>0)
		{
			a[i]=(num%8)+48;
			i++;
			num/=8;
		}
		for(--i;i>=0;i--)
		{
			CharLCD(a[i]);
		}
	}
}

void BinLCD(u32 num,u32 nBD)
{
	s32 i;
	for(i=(nBD-1);i>=0;i++)
	{
		CharLCD(((num>1)&1)+48);
	}
}

//rtc
char week[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};

// RTC Initialization: Configures and enables the RTC
void RTC_Init(void) 
{
    // Disable & Reset the RTC
		CCR = RTC_RESET;
	
	
    // Set prescaler integer part
		PREINT = PREINT_VAL;	
    // Set prescaler fractional part
		PREFRAC = PREFRAC_VAL;

    // Enable the RTC
		//CCR = RTC_ENABLE;	//for LPC2129
		CCR = RTC_ENABLE | RTC_CLKSRC_EX;	//For LPC2148
	
}

void GetRTCTime(s32 *hour,s32 *min,s32 *sec)
{
	*hour = HOUR;
	*min = MIN;
	*sec = SEC;	
}

void DisplayRTCTime(u32 hour,u32 min,u32 sec)
{
	CmdLCD(GOTO_LINE1_POS0);
	CharLCD((hour/10)+48);
	CharLCD((hour%10)+48);
	CharLCD(':');
	CharLCD((min/10)+48);
	CharLCD((min%10)+48);
	CharLCD(':');
	CharLCD((sec/10)+48);
	CharLCD((sec%10)+48);
}

void SetRTCTime(u32 hr,u32 mi,u32 se)
{
	HOUR = hr;
	MIN = mi;
	SEC = se;	
}

void GetRTCDate(s32 *date,s32 *month,s32 *year)
{
	*date = DOM;
	*month = MONTH;
	*year = YEAR;	
}

void DisplayRTCDate(u32 date,u32 month,u32 year)
{
	CmdLCD(GOTO_LINE2_POS0);
	CharLCD((date/10)+48);
	CharLCD((date%10)+48);
	CharLCD('/');
	CharLCD((month/10)+48);
	CharLCD((month%10)+48);
	CharLCD('/');
	U32LCD(year);
}

void SetRTCDate(u32 date,u32 month,u32 year)
{
	DOM = date;
	MONTH = month;
	YEAR = year;	
}

void GetRTCDay(s32 *day)
{
	*day = DOW;
}

void DisplayRTCDay(u32 day)
{
	CmdLCD(GOTO_LINE2_POS0+12);
	StrLCD(week[day]);	
}

void SetRTCDay(u32 day)
{
	DOW = day;
}






