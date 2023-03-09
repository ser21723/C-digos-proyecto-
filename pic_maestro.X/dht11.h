

#include <xc.h>
#define _XTAL_FREQ 8000000

#ifndef DHT11_H
#define DHT11_H

// Función de inicialización
void DHT11_Init();

// Función para leer el sensor
unsigned char DHT11_Read(unsigned char *humidity, unsigned char *temperature);

#endif
