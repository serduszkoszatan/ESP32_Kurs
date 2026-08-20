//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO

// Standardowe biblioteki C

// Własne include
#include "template_file.h"

//================================================
// DEFINITIONS
//================================================



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

void template_file_init(void)
{

led_output_init(LED_1);

}


//================================================
// MAIN FUNCTIONS
//================================================

