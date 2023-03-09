

#include "dht11.h"
#include <xc.h>

// Definir los pines de entrada y salida
#define DHT11_PIN      RC0
#define DHT11_TRIS     TRISC0

// Definir constantes de tiempo de espera
#define DHT11_STARTUP_DELAY_MS   18
#define DHT11_DATA_BIT_DELAY_US  40

// Función de inicialización
void DHT11_Init()
{
    // Establecer el pin del sensor como entrada
    DHT11_TRIS = 1;
}

// Función para leer el sensor
unsigned char DHT11_Read(unsigned char *humidity, unsigned char *temperature)
{
    unsigned char data[5] = {0};
    unsigned char i, j;

    // Paso 1: enviar señal de inicio
    DHT11_TRIS = 0;
    DHT11_PIN = 0;
    __delay_ms(DHT11_STARTUP_DELAY_MS);
    DHT11_PIN = 1;
    __delay_us(30);
    DHT11_TRIS = 1;

    // Paso 2: esperar la respuesta del sensor
    while (DHT11_PIN == 1);
    while (DHT11_PIN == 0);
    while (DHT11_PIN == 1);

    // Paso 3: leer los datos del sensor
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 8; j++)
        {
            // Esperar hasta que el siguiente bit esté disponible
            while (DHT11_PIN == 0);

            // Medir el tiempo que el bit está en alto
            __delay_us(DHT11_DATA_BIT_DELAY_US);

            // Si el bit sigue estando en alto, entonces es un 1, de lo contrario es un 0
            if (DHT11_PIN == 1)
            {
                data[i] |= (1 << (7 - j));
            }

            // Esperar hasta que el bit haya terminado
            while (DHT11_PIN == 1);
        }
    }

    // Paso 4: verificar la suma de comprobación
    if ((data[0] + data[1] + data[2] + data[3]) != data[4])
    {
        return 0; // Error de suma de comprobación
    }

    // Paso 5: guardar los datos
    *humidity = data[0];
    *temperature = data[2];

    return 1; // Éxito
}
