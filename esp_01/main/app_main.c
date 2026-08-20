/*
Każda kolejna zmiana GitHub:
edycja → Stage → Commit → Sync Changes
*/

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki

#include "UART.h"

#define BUTTON_PIN GPIO_NUM_1

void app_main(void)
{
   UART_init();
   gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
   gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLDOWN_ONLY);
   bool ledState = false;

while(1) {
   
  if (gpio_get_level(BUTTON_PIN))
    {
        ledState ^= 1;
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    UART_write(ledState ? "ON\r\n" : "OFF\r\n");

    printf("ledState = %d\r\n", ledState);

    vTaskDelay(pdMS_TO_TICKS(100));
   }
}

