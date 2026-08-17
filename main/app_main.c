/*
Każda kolejna zmiana GitHub:
edycja → Stage → Commit → Sync Changes
*/

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#include "ULN2003.h"


void app_main(void)
{

    uln2003_init();

while(1)
{
        uln2003_step(512);
        vTaskDelay(pdMS_TO_TICKS(1000));
        uln2003_step(-512);
        vTaskDelay(pdMS_TO_TICKS(1000));
}
}
