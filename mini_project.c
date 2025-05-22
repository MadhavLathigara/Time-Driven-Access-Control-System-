//final project
//mini_project.c

#include <lpc21xx.h>
#include "project_defines.h"
#include "types.h"

#define SW1_AL 0
#define LED_FAIL 4
#define LED_PASS 2

#define EINT0_VIC_CHNO 14
#define EINT_PIN_FUNC 3

int mini_time = 9;
int max_time = 18;

void eint0_fiq_isr(void) __irq;
void BuildCGRAM_1(u8 *p,u8 nBytes);
void BuildCGRAM_2(u8 *p,u8 nBytes);
void BuildCGRAM_3(u8 *p,u8 nBytes);
int checkPassword(void);
void handlePasswordEntry(char key);
void beep(void);

u32 flag=1;
s32 hour=15,min = 45,sec = 00 ,date = 9 ,month = 4,year = 2025 ,day = 1;

char correctPassword[] = "2222"; // Set your password here
char enteredPassword[5];  // Array to store the entered password
int passwordIndex = 0;


u8 cgramLUT_Right[8]=
{
        0x00,0x04,0x02,0x1F,0x02,0x04,0x00,0x00
};

u8 cgramLUT_Left[8]=
{
        0x00,0x04,0x08,0x1F,0x08,0x04,0x00,0x00
};

u8 cgramLUT_Door_1[8]=
{
        0x1F,0x1F,0x1F,0x11,0x11,0x11,0x11,0x11
};

u8 cgramLUT_Door_2[8]=
{
        0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11
};

int main()
{
	s32 temp_pass ;
	char key;
	RTC_Init();
	InitLCD();
	InitKPM();
	SetRTCTime(hour,min,sec);
	SetRTCDate(date,month,year);
	SetRTCDay(day);
	
	cfgPortPinFunc(0,1,EINT_PIN_FUNC);
	VICIntSelect=0;
	VICIntEnable=(1<<EINT0_VIC_CHNO);
	VICVectCntl0=(1<<5)|14;
	VICVectAddr0=(u32)eint0_fiq_isr;
	EXTMODE=(1<<0);
	
	while(1)
	{
		if(1 == flag)
		{
			while(1)
			{
	
				if(((IOPIN0>>SW1_AL)&1)==1)
				{
					//active low switch
					StrLCD(" TIME: ");
					delay_ms(400);
					GetRTCTime(&hour,&min,&sec);
					DisplayRTCTime(hour,min,sec);
					GetRTCDate(&date,&month,&year);
					DisplayRTCDate(date,month,year);
					GetRTCDay(&day);
					DisplayRTCDay(day);
				}
				else
				{
					CmdLCD(CLEAR_LCD);
					CmdLCD(GOTO_LINE1_POS0);
					flag = 2;
					break;
				}
			}
		}
		
		if(2 == flag)
		{
			while(1)
			{
				if(((IOPIN0>>SW1_AL)&1)==0)
				{
					flag = 1;
					break;
				}
				
				key = keyscan();
				while(!colscan());
				
				CmdLCD(0x01);
				StrLCD("Password");
				CmdLCD(GOTO_LINE2_POS0);
				
				if(key)
				{
					handlePasswordEntry(key);
					//CmdLCD(CLEAR_LCD);
					
					for(temp_pass=0 ; temp_pass < passwordIndex ; temp_pass++)
					{
						CmdLCD(GOTO_LINE1_POS0+temp_pass);
						StrLCD("*");
					}
					
					if(key == 'E')
					{
						if(checkPassword())
						{
							if((hour>=mini_time) && (hour<=max_time))
							{
								CmdLCD(CLEAR_LCD);

								while(1)
								{
									CmdLCD(GOTO_LINE1_POS0+6);
									StrLCD("Correct");
									
									CmdLCD(GOTO_LINE1_POS0);
									BuildCGRAM_2(cgramLUT_Door_1,8);
									CharLCD(2);
									delay_s(1);
									CmdLCD(GOTO_LINE2_POS0);
									BuildCGRAM_3(cgramLUT_Door_2,8);
									CharLCD(3);	
									delay_s(1);
								}
							}
							else
							{
								CmdLCD(CLEAR_LCD);StrLCD("COME BACK AT 9AM");
								delay_s(1);
								flag = 1;
								CmdLCD(CLEAR_LCD);
								break;
							}
						}
						else
						{
							// Password is incorrect, reset entry
							passwordIndex = 0;
							enteredPassword[0] = '\0';  
							CmdLCD(GOTO_LINE1_POS0);
							delay_ms(100);
							StrLCD("INCORRECT PASS");
							beep();
							flag =1;
							CmdLCD(CLEAR_LCD);
							break;
						}
					}
				}
			}
		}
	}
}




// Function to handle password entry with backspace support
void handlePasswordEntry(char key)
{
		if((key == 'N')|| (key == 'U') || (key == 'D') || (key == 'R') || (key == 'L') )
    {
			return ;
    }
    // If the key is 'B' (backspace), remove the last character from the entered password
    if (key == 'B' && passwordIndex > 0)
    {
      passwordIndex--;  // Decrease index to "erase" last character
      enteredPassword[passwordIndex] = '\0';  // Null-terminate the string
    }
    else if ((key != 'B') && (passwordIndex < (sizeof(enteredPassword) - 1)))
    {
			enteredPassword[passwordIndex] = key;
      passwordIndex++;
      enteredPassword[passwordIndex] = '\0';
    }
}

int checkPassword()
{
     u32 i;
     for (i = 0; i < passwordIndex; i++)
     {
        if (enteredPassword[i] != correctPassword[i])
        {
             return 0;  // Password is incorrect
        }
     }
        return 1;  // Password is correct
}

void beep(void) 
{
    IODIR0|= 1<<LED_FAIL;
    
    IOPIN0 |= (1 << LED_FAIL);   // Set buzzer pin high
    delay_ms(200);
    IOPIN0 &= ~(1 << LED_FAIL);  // Set buzzer pin low
    
}

void BuildCGRAM(u8 *p,u8 nBytes)
{
        u32 i;
        //position cursor to cgram start;
        CmdLCD(GOTO_CGRAM_START);
        //CmdLCD(GOTO_LINE1_POS0+10);
        //write into cgram via data reg
        for(i=0;i<nBytes;i++)
        {
                CharLCD(p[i]);
        }
        //reposition curosr back to ddram
        CmdLCD(GOTO_LINE1_POS0+12);
}
void BuildCGRAM_1(u8 *p,u8 nBytes)
{
        u32 i;
        //position cursor to cgram start;
        CmdLCD( GOTO_CGRAM_START+8);
//      CmdLCD(GOTO_LINE2_POS0+12);
        //write into cgram via data reg
        for(i=0;nBytes;i++)
        {
                CharLCD(p[i]);
        }
        //reposition curosr back to ddram
        CmdLCD(GOTO_LINE1_POS0+14);
}

void BuildCGRAM_2(u8 *p,u8 nBytes)
{
        u32 i;
        //position cursor to cgram start;
        CmdLCD(GOTO_CGRAM_START+16);
				//CmdLCD(GOTO_LINE1_POS0);
        //write into cgram via data reg
        for(i=0;i<nBytes;i++)
        {
                CharLCD(p[i]);
        }
        //reposition curosr back to ddram
        CmdLCD(GOTO_LINE1_POS0);
}
void BuildCGRAM_3(u8 *p,u8 nBytes)
{
					u32 i;
					//position cursor to cgram start;
					CmdLCD(GOTO_CGRAM_START+24);
					//CmdLCD(GOTO_LINE2_POS0);
					//write into cgram via data reg
					for(i=0;i<nBytes;i++)
					{
									CharLCD(p[i]);
					}
					//reposition curosr back to ddram
					CmdLCD(GOTO_LINE2_POS0);
}


void eint0_fiq_isr(void) __irq
{
	s32 Line_1=0;
	u32 key;
	
		//isr activity starts
		{
			CmdLCD(0x01);

			while(1)
			{
				key = '\0';
				key = keyscan();
				
				if(key == 'A')
				{
					CmdLCD(0x01);
					StrLCD("BYEEEEEEEE");
					CmdLCD(0x01);
					break;
				}
				
				SetRTCTime(hour,min,sec);
				SetRTCDate(date,month,year);
				SetRTCDay(day);
				GetRTCTime(&hour,&min,&sec);
				DisplayRTCTime(hour,min,sec);
				GetRTCDate(&date,&month,&year);
				DisplayRTCDate(date,month,year);
				GetRTCDay(&day);
				DisplayRTCDay(day);
				BuildCGRAM(cgramLUT_Right,8);
				CharLCD(4);
				BuildCGRAM_1(cgramLUT_Left,8);
				CharLCD(5);

				
				if((key == 'R') || (key == 'L'))
				{
					if((key == 'L') && (Line_1 == 0))
					{
						//Line_1=0;
					}
					else if(key == 'L')
					{
						Line_1--;
						if((Line_1 == 2) || (Line_1 == 5) || (Line_1 == 18) || (Line_1 == 21))
						{
							Line_1-=1;
						}
						else if(Line_1 == 15)
						{
							Line_1 = 7;
						}
						else if(Line_1 == 27)
						{
							Line_1-=2;
						}
					}
					else if((key == 'R') && (Line_1 == 28))
					{
						
					}
					else if(key == 'R')
					{
						Line_1++;
						
						if((Line_1 == 2) || (Line_1 == 5) || (Line_1 == 18) || (Line_1 == 21))
						{
							Line_1+=1;
						}
						else if(Line_1 == 8)
						{
							Line_1 = 16;
						}
						else if(Line_1 == 26)
						{
							Line_1+=2;
						}
					}
					if(Line_1 < 0)
					{
						Line_1=0;
					}
				}
				
// Key == U and D
				
				if((key == 'U') || (key == 'D'))
				{
					if(Line_1<16)         // Line 1
					{
						if(key == 'D')			// Letter - D
						{
							if((Line_1 == 1) || (Line_1 == 0))
							{
								hour--;
								if(hour < 0)
								{
									hour = 23;
								}
							}
							else if((Line_1 == 3) || (Line_1 == 4))
							{
								min--;
								if(min < 0)
								{
									min = 59;
								}
							}
							else if ((Line_1 == 6) || (Line_1 == 7))
							{
								sec--;
								if(sec < 0)
								{
									sec = 59;
								}
							}
						}
						else if(key == 'U')  // Letter - U 
						{
							if((Line_1 == 1) || (Line_1 == 0))
							{
								hour++;
								if(hour > 23)
								{
									hour = 0;
								}
							}
							else if((Line_1 == 3) || (Line_1 == 4))
							{
								min++;
								if(min > 59)
								{
									min = 0;
								}
							}
							else if ((Line_1 == 6) || (Line_1 == 7))
							{
								sec++;
								if(sec > 59)
								{
									sec = 0;
								}
							}
						}
					}
					else if(Line_1 >= 16)		// Line 2			
					{
						if(key == 'D')
						{
							if((Line_1 == 16) || (Line_1 == 17))
							{
								date--;
								if(date < 0)
								{
									date = 31;
								}
							}
							else if((Line_1 == 19) || (Line_1 == 20))
							{
								month--;
								if(month < 0)
								{
									month = 12;
								}
							}
							else if((Line_1 >= 22) && (Line_1<=25))
							{
								year--;
								if(year < 0)
								{
									year = 2025;
								}
							}
							else if((Line_1 >= 28) && (Line_1<=30))
							{
								day--;
								if(day < 0)
								{
									day = 6;
								}
							}
						}
						else if(key == 'U') 
						{
							if((Line_1 == 16) || (Line_1 == 17))
							{
								date++;
								if(date > 31)
								{
									date = 0;
								}
							}
							else if((Line_1 == 19) || (Line_1 == 20))
							{
								month++;
								if(month > 12)
								{
									date = 0;
								}
							}
							else if((Line_1 >= 22) && (Line_1<=25))
							{
								year++;
							}
							else if((Line_1 >= 28) && (Line_1<=30))
							{
								day++;
								if(day > 6)
								{
									date = 0;
								}
							}
						}
					}
				}
				if(Line_1<=16)
				{
					CmdLCD(GOTO_LINE1_POS0+Line_1);
				}
				else if(Line_1>16)
				{
					CmdLCD(GOTO_LINE2_POS0+(Line_1-17));
				}
			}
			
		}
		//isr activity ends    
    
		EXTINT=1<<0;
    VICVectAddr=0;
		delay_ms(1000);
}

