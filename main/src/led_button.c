//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki

// Standardowe biblioteki C
#include <stdio.h>              
#include <string.h>
#include <stdbool.h>

//Własna biblioteka
#include "led_button.h"

//================================================
// DEFINITIONS
//================================================

#define LED_PIN GPIO_NUM_1
#define BUTTON_PIN GPIO_NUM_2

//====================
// STATIC VARIABLES
//====================

//Zmienne do obsługi przycisku
static bool led_state = false;
static bool last_button_state = false;

//================================================
// INITIALIZATION FUNCTIONS
//================================================

/*
Funkcja inicjalizująca wszystkie urządzenia GPIO
*/
static void led_output_init(gpio_num_t pin)
{
    if(pin == GPIO_NUM_NC)
        return;

    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
}

static void led_input_init(gpio_num_t pin)
{
    if(pin == GPIO_NUM_NC)
        return;

    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);
   
}

void led_button_init(void)
{
led_output_init(LED_PIN);
led_input_init(BUTTON_PIN);
}

//================================================
// MAIN FUNCTIONS
//================================================

/*
Funkcja do obsługi przycisku do włączania diody
*/
void button_led (void) {

 bool button = gpio_get_level(BUTTON_PIN);

    // Reakcja tylko na zbocze narastające (0 -> 1)
    if (button && !last_button_state)
    {
        led_state = !led_state;
        gpio_set_level(LED_PIN, led_state);
    }

    last_button_state = button;
    }




