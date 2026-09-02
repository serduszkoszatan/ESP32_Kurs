#include <stdio.h>
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO

#include "SPI.h"

#define LED_PIN GPIO_NUM_1

void app_main(void)
{
    // GPIO1 jako wyjście dla diody
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    spi_init();

while (1)
    {
        spi_read();

        char *data = spi_get_buffer();

        if (data[0] == 'O' && data[1] == 'N') {            
            gpio_set_level(LED_PIN, 1);        
        } else if (data[0] == 'O' && data[1] == 'F' && data[2] == 'F') {
            gpio_set_level(LED_PIN, 0);       
        } 
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    

}
