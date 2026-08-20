#include <stdio.h>
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO

#define LED_PIN GPIO_NUM_8

void app_main(void)
{

    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while(1) 
    {
    gpio_set_level(LED_PIN,1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(LED_PIN,0);
    vTaskDelay(pdMS_TO_TICKS(1000));

    }

}
