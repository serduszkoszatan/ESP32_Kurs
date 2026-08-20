/*
Każda kolejna zmiana GitHub:
edycja → Stage → Commit → Sync Changes
*/

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#include "lcd_16x2.h"


void app_main(void)
{
   gpio_set_direction(1, GPIO_MODE_INPUT);
   gpio_set_pull_mode(1,GPIO_PULLDOWN_ONLY);
   bool ledState = false;

   
}
