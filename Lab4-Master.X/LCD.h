#include <xc.h>
#define _XTAL_FREQ 8000000

//#define USE_CGRAM_LCD


#define RS PORTAbits.RA0
#define EN PORTAbits.RA1
#define D4 PORTAbits.RA2
#define D5 PORTAbits.RA3
#define D6 PORTAbits.RA4
#define D7 PORTAbits.RA5

#define RS_DIR TRISAbits.TRISA0
#define EN_DIR TRISAbits.TRISA1
#define D4_DIR TRISAbits.TRISA2
#define D5_DIR TRISAbits.TRISA3
#define D6_DIR TRISAbits.TRISA4
#define D7_DIR TRISAbits.TRISA5

void Lcd_Port(char a);
void Lcd_Cmd(char a);
void Lcd_Clear(void);
void Lcd_Set_Cursor(char a, char b);
void Lcd_Init(void);
void Lcd_Write_Char(char a);
void Lcd_Write_String(const char *a);
void Lcd_Shift_Right(void);
void Lcd_Shift_Left(void);
void Lcd_Blink(void);
void Lcd_NoBlink(void);

#ifdef USE_CGRAM_LCD
void Lcd_CGRAM_CreateChar(char add, const char* chardata);
void Lcd_CGRAM_Init(void);
void Lcd_CGRAM_Close(void);
#endif