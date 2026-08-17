/*
Każda kolejna zmiana GitHub:
edycja → Stage → Commit → Sync Changes
*/

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#include "servo_drive.h"


void app_main(void)
{

    servo_drive_init();

while(1)
{
    for(int i = -90; i <=90; i+=15){
        servo_set_angle(i);
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    servo_set_angle(-90);
    vTaskDelay(pdMS_TO_TICKS(1000));
    
}
}
