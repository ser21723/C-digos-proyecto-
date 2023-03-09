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
#include "ADC.h"

#define _XTAL_FREQ 4000000 

#define MQ7_PIN AN7  // Pin analógico del sensor MQ-7
#define CO_PPM_MAX 255  // Máxima concentración de CO detectada por el sensor MQ-7 (en ppm)
#define VREF 5.0  // Voltaje de referencia para la conversión analógica a digital (ADC)

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

    int adc_value = 0;
    int co_concentration = 0;
    char buffer[20];
    char Hora[] = "000000";
    char Fecha[] = "090323";
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
    
    ADCON1 = 0X06;

    convertirHora();
    enviar_hora();
      
    convertirFecha();
    enviar_fecha();
    
    while(1){
        
        Lcd_Set_Cursor(1, 8);
        leer_hora();
        mostrar_hora();
        
        Lcd_Set_Cursor(2, 8);
        leer_fecha();
        mostrar_fecha();

        __delay_ms(100);
        
         // Lectura del valor analógico del sensor MQ-7
        int adc_value = 0;
        for (int i = 0; i < 16; i++) { // Realiza una media de 16 muestras
            ADC_Init(AN7); // Inicio de la conversión analógica a digital
            adc_value += ADC_Read(7); // Lectura del valor digitalizado
        }
        adc_value /= 16; // Media de las 16 muestras

        // Cálculo de la concentración de CO en ppm
        float co_voltage = (float) adc_value * VREF / 4096.0; // Conversión del valor digital a voltaje
        int co_concentration = co_voltage * CO_PPM_MAX / 1.4; // Cálculo de la concentración de CO en ppm (basado en la hoja de especificaciones del MQ-7)

        // Impresión de la concentración de CO en la pantalla LCD
        char co_string[16];
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("CO:");
        sprintf(co_string, "%d ppm", co_concentration); // Conversión de la concentración de CO a una cadena de caracteres
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(co_string); // Impresión de la concentración de CO en la pantalla LCD

    }
    return; 
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    TRISA = 0b01011100; // Puerto A7 como entrada para la lectura del sensor MQ-7
    TRISA = 0; 
    PORTA = 0;
    ANSEL = 0;
    ANSELH = 0;
    OSCCONbits.IRCF2 = 0; // Bit IRCF2 en 0
    OSCCONbits.IRCF1 = 1; // Bit IRCF1 en 1
    OSCCONbits.IRCF0 = 0; // Bit IRCF0 en 0
    
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
