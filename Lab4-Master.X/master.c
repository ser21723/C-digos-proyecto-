////******************************************************************************
//   UNIVERSIDAD DEL VALLE DE GUATEMALA
//   IE3054 ELECTRÓNICA DIGITAL 2 
//   AUTOR: MICHELLE SERRANO
//   COMPILADOR: XC8 (v1.41), MPLAB X IDE (v6.00)
//   PROYECTO: LABORATORIO 4
//   HARDWARE: PIC16F887
//   CREADO: 10/02/2023

#pragma config FOSC = INTRC_NOCLKOUT  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF             // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF            // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF            // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF               // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF            // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF             // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF            // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF              // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V         // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF              // Flash Program Memory Self Write Enable bits (Write protection off)

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <pic16f887.h>
#include <stdlib.h>
#include "I2C.h"
#include "LCD.h"

#define _XTAL_FREQ 8000000
//*****************************************************************************
// Definición de variables
//*****************************************************************************
    void setup(void);
    void convertirHora(void);
    void convertirFecha(void);
    void enviar_hora (void);
    void leer_hora(void);
    void enviar_fecha(void);
    void leer_fecha(void);
    void mostrar_hora(void);
    void mostrar_fecha(void);
    void configuracion(void);


    char buffer[20];
    char Hora[] = "000000";
    char Fecha[] = "000000";
    unsigned char ADC;

    uint8_t sec, min, hora;
    uint8_t dia, mes, anio;

    int caso = 0;

//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
        Lcd_Init();    
        Lcd_Clear();
    
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("ADC:");
    __delay_ms(1);
        
    ADCON1 = 0X06;

    convertirHora();
    enviar_hora();
      
    convertirFecha();
    enviar_fecha();
    
    while(1){
        Lcd_Set_Cursor(2,1);
        sprintf(buffer, " %u  ", ADC);
        Lcd_Write_String(buffer);
        __delay_ms(1);   
        
        Lcd_Set_Cursor(1, 7);
        leer_hora();
        mostrar_hora();
        
        Lcd_Set_Cursor(2, 7);
        leer_fecha();
        mostrar_fecha();

        __delay_ms(100);
        
        configuracion();
        
    }
    return; 
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    TRISB = 0;
    TRISD = 0;
    TRISE = 0b00000111; //RE0, RE1 Y RE2 como entradas
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    
    I2C_Init_Master(I2C_100KHZ);
}

void convertirHora(void){
    
    for(uint8_t i=0; i<7; i++){
        Hora[i] &= 0x0F;
    }
    
    hora = ((Hora[0]<<4)+ Hora[1]);
    min = ((Hora[2]<<4)+ Hora[3]);
    sec = ((Hora[4]<<4)+ Hora[5]);
}

void convertirFecha(void){
    
    for(uint8_t i=0; i<7; i++){
        Fecha[i] &= 0x0F;
    }
    
    dia = ((Fecha[0]<<4)+ Fecha[1]);
    mes = ((Fecha[2]<<4)+ Fecha[3]);
    anio = ((Fecha[4]<<4)+ Fecha[5]);
}

void enviar_hora(void){
    
    I2C_Start();            //Incia comunicaión I2C
    I2C_Write(0xD0);        //Escoje dirección del reloj
    I2C_Write(0x00);        //Posición de los segundos
    I2C_Write(sec);         //Posición donde va leer
    I2C_Write(min);         //Posición donde va leer
    I2C_Write(hora);        //Posición donde va leer
    I2C_Stop();             //Detiene la comunicaión I2C
}

void leer_hora(void){
    
    I2C_Start();            //Incia comunicaión I2C
    I2C_Write(0xD0);        //Escoje dirección del reloj
    I2C_Write(0x00);        //Posición donde va leer
    I2C_Restart();          //Reinicia la comuniación I2C
    I2C_Write(0xD1);        //Leer posición
    sec = I2C_Read();      //lee posicion de reloj
    I2C_Ack();
    min = I2C_Read();      //lee posicion de reloj
    I2C_Ack();
    hora = I2C_Read();      //lee posicion de reloj
    I2C_Nack();
    I2C_Stop();             //Termina comunicaion I2C
}

void enviar_fecha(void){
    
    I2C_Start();            //Incia comunicaión I2C
    I2C_Write(0xD0);        //Escoje dirección del reloj
    I2C_Write(0x04);        //Posición de los segundos
    I2C_Write(dia);         //Posición donde va leer
    I2C_Write(mes);         //Posición donde va leer
    I2C_Write(anio);        //Posición donde va leer
    I2C_Stop();             //Detiene la comunicaión I2C
}

void leer_fecha(void){
    
    I2C_Start();            //Incia comunicaión I2C
    I2C_Write(0xD0);        //Escoje dirección del reloj
    I2C_Write(0x04);        //Posición donde va leer
    I2C_Restart();          //Reinicia la comuniación I2C
    I2C_Write(0xD1);        //Leer posición
    dia = I2C_Read();      //lee posicion de reloj
    I2C_Ack();
    mes = I2C_Read();      //lee posicion de reloj
    I2C_Ack();
    anio = I2C_Read();      //lee posicion de reloj
    I2C_Nack();
    I2C_Stop();             //Termina comunicaion I2C
}


void mostrar_hora(void){
    Lcd_Write_Char((hora>>4)+0x30);
    Lcd_Write_Char((hora & 0x0F)+0x30);
    Lcd_Write_Char(':');
    Lcd_Write_Char((min>>4)+0x30);
    Lcd_Write_Char((min & 0x0F)+0x30);
    Lcd_Write_Char(':');
    Lcd_Write_Char((sec>>4)+0x30);
    Lcd_Write_Char((sec & 0x0F)+0x30);
}

void mostrar_fecha(void){
    
    Lcd_Write_Char((dia>>4)+0x30);    
    Lcd_Write_Char((dia & 0x0F)+0x30);
    Lcd_Write_Char('/');
    Lcd_Write_Char((mes>>4)+0x30);
    Lcd_Write_Char((mes & 0x0F)+0x30);
    Lcd_Write_Char ('/');   
    Lcd_Write_Char ((anio>>4)+0x30);
    Lcd_Write_Char ((anio & 0x0F)+0x30);
}

void configuracion (void){
    
    if (caso == 0) {
        if (PORTEbits.RE0 == 1){
       {
        caso = caso +1;
        while(PORTEbits.RE0 == 1);
        __delay_ms(20);
    int num = atoi(&Fecha[5]);
    num ++;
    sprintf(&Fecha[5], "%u", num);
    convertirFecha();
    enviar_fecha();
    leer_fecha();
    mostrar_fecha();
                }
}
        else
        convertirFecha();
        enviar_fecha();
    }   
}
        
