/* Detectou escravo na linha
Codigo da Familia: 40
Numero de Serie  : 63 161 4 5 0 0
CRC=             : 18
*/
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "serial.h"
#include "DS18B20.h"
#include "delay.h"
#include <inttypes.h> 
#include "digital.h"

float temperatura1;
float temperatura2;

 

extern "C" void app_main() ;

void printFloat (float f)
{
  char str[20];
 sprintf(str, "%d.%04d\n", (int)f, (int)(f*10000)%10000);
 printf(str);

}

void app_main()
{
  delay_ms(2000);
  serial.begin(9600);
  printf("\n\nRODANDO\n\n");

  DS18B20 meuSensor = DS18B20(PIN16);

 char v1[]={29,3,24,151,121,79,26,40};
 char v2[]={112,3,24,151,121,69,116,40};
 
    //meuSensor.init(v);

  char vetor[8*8] = {0};
  uint8_t qtde_sensores = meuSensor.fazScan(vetor);
  printf("Numero de sensores: %d\n",qtde_sensores);
  for (uint8_t x=0;x<qtde_sensores*8;x++) {
    if (x % 8 == 0) printf("\n");
    printf("%d\n",vetor[x]);
  }
 /*   while(1){
      
      temperatura1 = meuSensor.readTargetTemp(v1);
      temperatura2 = meuSensor.readTargetTemp(v2);
      printf("Sensor 1 ");printFloat(temperatura1);
      printf("Sensor 2 ");printFloat(temperatura2);
      delay_ms(500);

    }
  */
 }

