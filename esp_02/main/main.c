#include <stdio.h>
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO

#include "UART.h"

#define LED_PIN GPIO_NUM_1

void app_main(void)
{
    // GPIO1 jako wyjście dla diody
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    UART_init();
while (1)
    {
        UART_read();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    

}
