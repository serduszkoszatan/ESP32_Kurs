#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dht11.h"

void app_main(void)
{
    uint8_t humidity;
    uint8_t temperature;

    dht11_init();

    vTaskDelay(pdMS_TO_TICKS(2000));

    while(1){

        int result = dht11_read(&humidity,&temperature);

        if(result == DHT11_OK){
            printf("Temperature: %d C\n",temperature);
            printf("Humidity: %d %%\n",humidity);
        }
        else{
            printf("DHT11 error: %d\n",result);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}