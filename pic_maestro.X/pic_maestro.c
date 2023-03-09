/*
 ********************************************************************************
Universidad del Valle de Guatemala
 * curso: Electrónica Digital 2 
Autor: Gabriel Andrade
compilador: XC8
proyecto: Incubadora con red de sensores 
hardware: PIC 16F887
última modificación: 9-03-2023
 ********************************************************************************
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#define _XTAL_FREQ 8000000
#include <xc.h>
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "LCD.h"    //lIBRERIA LCD
#include "I2C.h"    //LIBRERIA I2C
#include "adc.h"    //LIBRERIA ADC
#include "ds3231.h" //LIBRERIA RTC
#include "dht11.h"  //LIBRERIA DHT11

////////////////////////////////////////////////////////////////////////////////
// PROTOTIPOS DE FUNCIONES
////////////////////////////////////////////////////////////////////////////////
void setupINTOSC(void);
void CLK_CONFIG();
void RST_CLK();
void CLCK ();
    void convertirHora(void);
    void convertirFecha(void);
    void enviar_hora (void);
    void leer_hora(void);
    void enviar_fecha(void);
    void leer_fecha(void);
    void mostrar_hora(void);
    void mostrar_fecha(void);
    void configuracion(void);

////////////////////////////////////////////////////////////////////////////////
// PROTOTIPOS DE FUNCIONES
////////////////////////////////////////////////////////////////////////////////
char buffer[20];    //buffer para guardar la cadena de la LCD
    
//variables para uso del DHT11
unsigned char temperature;
unsigned char humidity;

//ADC para lectura de motores 
unsigned char valor_ADC1;   
unsigned char valor_ADC2; 

//variables para uso del RTC
//int8_t dia;
//int8_t mes;
//int8_t ano; 
//int8_t dow;
//int8_t horas;
//int8_t minutos;
//int8_t segundos;
//
unsigned char selector;
    char buffer[20];
    char Hora[] = "000000";
    char Fecha[] = "000000";

    uint8_t sec, min, hora; 
    uint8_t dia, mes, anio;


////////////////////////////////////////////////////////////////////////////////
// CODIGO PRINCIPAL
////////////////////////////////////////////////////////////////////////////////

void main(void) {
    setupINTOSC();
    
    //Inicialización de los ADCs
    ADC_Init(AN5);
    ADC_Init(AN6);
    
    ADCON1 = 0X06;

    convertirHora();
    enviar_hora();
      
    convertirFecha();
    enviar_fecha();
    
    while (1) 
    {
        //lectura de los valores de ADCs
        valor_ADC1 = ADC_Read(5);
        valor_ADC2 = ADC_Read(6);
        
        //Lectura i2C de los ADC para los motores 
        I2C_Master_Start();
        I2C_Master_Write(0xA0); //seleccionamos el esclavo
        I2C_Master_Write(valor_ADC1);
        //I2C_Master_Write(valor_ADC2);
        I2C_Master_Stop();
        __delay_ms(200);
        
        // Leer la temperatura y la humedad del sensor
        if (DHT11_Read(&humidity, &temperature))
        {}
        __delay_ms(1000); //delay de lectura
        
          if(PORTBbits.RB0 == 0) // verificamos que el PB de cambio de estado este presionado
        {
            while(PORTBbits.RB0 == 0);
            __delay_ms(20);      //antirebotes
            Lcd_Clear();        //limpiamos la pantalla
            selector++;         //incrementamos el selector 
        }
        //CLK_CONFIG();
        //RST_CLK();
        CLCK ();
    }
}

////////////////////////////////////////////////////////////////////////////////
// FUNCIONES
////////////////////////////////////////////////////////////////////////////////

void setupINTOSC(void) {
    //Seleccion de Oscilador interno
    OSCCONbits.SCS = 1;

    // oscilador a 8MHz
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    
    //PUERTOS DIGITALES Y ANALOGICOS 
    ANSELbits.ANS0 = 0;
    ANSELbits.ANS1 = 0;
    ANSELbits.ANS2 = 0;
    ANSELbits.ANS3 = 0;
    ANSELbits.ANS4 = 0;
    ANSELbits.ANS5 = 1;
    ANSELHbits.ANS8 = 0;
    ANSELHbits.ANS9 = 0;
    ANSELHbits.ANS10 = 0;
    ANSELHbits.ANS12 = 0;
    
    //LCD
    TRISA = 0;
    PORTA = 0; 
    
    //Selectores 
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;
    PORTB = 0; 
    
    //pullups para selectores 
    INTCONbits.RBIE = 1; //interrupciones del puerto B
    OPTION_REGbits.nRBPU = 0; // habilitar pull ups individuales en B
    WPUBbits.WPUB0 = 1;//pullup en B0
    WPUBbits.WPUB1 = 1;//pullup en B1
    WPUBbits.WPUB2 = 1;//pullup en B2
    WPUBbits.WPUB2 = 1;//pullup en B3
    
    //inicializacion de la LCD
    Lcd_Init();
    

    //CONFIGURACION DEL I2C PARA EL MAESTRO
    I2C_Master_Init(100000);
}

//void CLK_CONFIG (void)
//{
//      switch(selector)
//        {
//            case 0: //cuando no se esta configurando ningun parametro
//                //coloca los datos del reloj en variables 
//                DS3231_Get_Date((uint8_t)&dia, (uint8_t)&mes, (uint8_t)&ano, (uint8_t)&dow);
//                DS3231_Get_Time((uint8_t)&horas, (uint8_t)&minutos, (uint8_t)&segundos);
//                
//                //escribe la fecha y hora en la LCD
//                Lcd_Set_Cursor(1,1);
//                sprintf(buffer, "T:%uC H:%u%%",temperature, humidity);
//                Lcd_Write_String(buffer);
//                sprintf(buffer, "%02u:%02u:%02u",dia, horas, minutos);
//                Lcd_Set_Cursor(2,8);
//                Lcd_Write_String(buffer);
//                __delay_ms(200);
//                break;
//
//            case 1:
//                Lcd_Set_Cursor(1,1);
//                Lcd_Write_String("elija el tiempo");
//                Lcd_Set_Cursor(2,1);
//                Lcd_Write_String("de incubacion");
//                break;
//                
//            case 2:  //selector de dia 
//                
//                //mensaje del selector 
//                Lcd_Set_Cursor(1,1);
//                sprintf(buffer, "Dia: %02u", dia);
//                Lcd_Write_String(buffer);
//                
//                //push button de incremento 
//                if(PORTBbits.RB1 == 0)
//                {
//                    while(PORTBbits.RB1 == 0);
//                    __delay_ms(20);
//                    dia++;
//                    if(dia > 31){
//                        dia = 31;
//                    }
//                }
//                
//                //push button de decremento 
//                if(PORTBbits.RB2 == 0)
//                {
//                    while(PORTBbits.RB2 == 0);
//                    __delay_ms(20);
//                    dia--;
//                    if(dia < 1){
//                        dia = 1;
//                    }
//                }
//                break;
//
//            case 3:  //selector de hora 
//                
//                //mensaje del selector
//                Lcd_Set_Cursor(1,1);
//                sprintf(buffer, "Hora: %02u", horas);
//                Lcd_Write_String(buffer);
//                
//                //push button de incremento 
//                if(PORTBbits.RB1 == 0)
//                {
//                    while(PORTBbits.RB1 == 0);
//                    __delay_ms(20);
//                    horas++;
//                    if(horas > 23){
//                        horas = 23;
//                    }
//                }
//                
//                //push button de decremento 
//                if(PORTBbits.RB2 == 0)
//                {
//                    while(PORTBbits.RB2 == 0);
//                    __delay_ms(20);
//                    horas--;
//                    if(horas == -1){
//                        horas = 0;
//                    }
//                }
//                break;
//
//            case 4:  //selector de minutos
//                
//                //mensaje del selector
//                Lcd_Set_Cursor(1,1);
//                sprintf(buffer, "minutosuto: %02u", minutos);
//                Lcd_Write_String(buffer);
//                
//                //push button de incremento
//                if(PORTBbits.RB1 == 0)
//                {
//                    while(PORTBbits.RB1 == 0);
//                    __delay_ms(20);
//                    minutos++;
//                    if(minutos > 59){
//                        minutos = 59;
//                    }
//                }
//                
//                //push button de decremento 
//                if(PORTBbits.RB2 == 0)
//                {
//                    while(PORTBbits.RB2 == 0);
//                    __delay_ms(20);
//                    minutos--;
//                    if(minutos == -1){
//                        minutos = 0;
//                    }
//                }
//                break;
//
//            /* cuando no se este realizando ninguna de las anteriores se pone
//             el default que es el guardado de las variables y el valor 
//             del selector en 0, es decir, desplegado en la LCD*/
//            default:
//            	dia = (uint8_t)dia;
//            	mes = (uint8_t)mes;
//            	ano = (uint8_t)ano;
//            	dow = (uint8_t)dow;
//            	horas = (uint8_t)horas;
//            	minutos = (uint8_t)minutos;
//            	segundos = (uint8_t)segundos;
//                DS3231_Set_Date_Time(dia,mes,ano,dow,horas,minutos,segundos);
//                selector = 0;
//                break;
//        }
//}
//
//void RST_CLK (void)
//{
//    if (PORTBbits.RB3 == 0)
//    {
//        while(PORTBbits.RB3 == 0);
//        __delay_ms(20);      //antirebotes
//        dia = 0;
//        mes = 0;
//        ano = 0;
//        dow = 0;
//        horas = 0;
//        minutos = 0;
//        segundos = 0;
//        DS3231_Set_Date_Time(dia,mes,ano,dow,horas,minutos,segundos);
//        selector = 0;
//    }   
//}


void CLCK (void) {
        
        Lcd_Set_Cursor(1, 7);
        leer_hora();
        mostrar_hora();
        
        Lcd_Set_Cursor(2, 7);
        leer_fecha();
        mostrar_fecha();

        __delay_ms(100);

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
    
    I2C_Master_Start();            //Incia comunicaión I2C
    I2C_Master_Write(0xD0);        //Escoje dirección del reloj
    I2C_Master_Write(0x00);        //Posición de los segundos
    I2C_Master_Write(sec);         //Posición donde va leer
    I2C_Master_Write(min);         //Posición donde va leer
    I2C_Master_Write(hora);        //Posición donde va leer
    I2C_Master_Stop();             //Detiene la comunicaión I2C
}

void leer_hora(void){
    
    I2C_Master_Start();            //Incia comunicaión I2C
    I2C_Master_Write(0xD0);        //Escoje dirección del reloj
    I2C_Master_Write(0x00);        //Posición donde va leer
    I2C_Master_RepeatedStart();          //Reinicia la comuniación I2C
    I2C_Master_Write(0xD1);        //Leer posición
    sec = I2C_Master_Read(0);      //lee posicion de reloj
    I2C_Ack();
    min = I2C_Master_Read(0);      //lee posicion de reloj
    I2C_Ack();
    hora = I2C_Master_Read(0);      //lee posicion de reloj
    I2C_Nack();
    I2C_Master_Stop();             //Termina comunicaion I2C
}

void enviar_fecha(void){
    
    I2C_Master_Start();            //Incia comunicaión I2C
    I2C_Master_Write(0xD0);        //Escoje dirección del reloj
    I2C_Master_Write(0x04);        //Posición de los segundos
    I2C_Master_Write(dia);         //Posición donde va leer
    I2C_Master_Write(mes);         //Posición donde va leer
    I2C_Master_Write(anio);        //Posición donde va leer
    I2C_Master_Stop();             //Detiene la comunicaión I2C
}

void leer_fecha(void){
    
    I2C_Master_Start();            //Incia comunicaión I2C
    I2C_Master_Write(0xD0);        //Escoje dirección del reloj
    I2C_Master_Write(0x04);        //Posición donde va leer
    I2C_Master_RepeatedStart();          //Reinicia la comuniación I2C
    I2C_Master_Write(0xD1);        //Leer posición
    dia = I2C_Master_Read(0);      //lee posicion de reloj
    I2C_Ack();
    mes = I2C_Master_Read(0);      //lee posicion de reloj
    I2C_Ack();
    anio = I2C_Master_Read(0);      //lee posicion de reloj
    I2C_Nack();
    I2C_Master_Stop();             //Termina comunicaion I2C
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
