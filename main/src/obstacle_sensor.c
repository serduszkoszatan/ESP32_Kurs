//================================================
// READ ME
//================================================
/*
The output pin becomes LOW (0) when an obstacle is detected and HIGH (1) when no obstacle is present.
*/

//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO

// Standardowe biblioteki C
#include <stdbool.h>

// Własne include
#include "obstacle_sensor.h"

//================================================
// DEFINITIONS
//================================================

#define led GPIO_NUM_1

#define sensor_out GPIO_NUM_4

//================================================
// STATIC VARIABLES
//================================================



//================================================
// INITIALIZATION FUNCTIONS
//================================================

static void output_init(gpio_num_t pin)
{
    if(pin == GPIO_NUM_NC)
        return;

    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
}

static void input_init(gpio_num_t pin)
{
    if(pin == GPIO_NUM_NC)
        return;

    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);
   
}

void obstacle_sensor_init(void)
{
    input_init(sensor_out);

    output_init(led);

}


//================================================
// MAIN FUNCTIONS
//================================================

void obstacle_sensor(void)
{
    if(gpio_get_level(sensor_out)==0) {
        gpio_set_level(led,1);
    } else {
        gpio_set_level(led,0);
    }
}