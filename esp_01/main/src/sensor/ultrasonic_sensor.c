//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

/*
The HC-SR04 ultrasonic sensor has four pins: VCC and GND are used to power the sensor. 
The TRIG pin is used to send a short HIGH pulse (about 10 microseconds), which makes the sensor emit an ultrasonic sound wave.
The ECHO pin then goes HIGH and stays HIGH until the reflected sound wave returns to the sensor.
The duration of this HIGH signal represents the time taken for the sound to travel to an object and back.
Using this, we can calculate the distance by multiplying the duration by the speed of sound, then dividing by two.
 We divide by two because the measured  duration represents the time it takes for the sound wave to travel to the object and back,
  which is twice the actual distance.
*/

//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO

#include "esp_timer.h"      
#include "esp_rom_sys.h" 

// Standardowe biblioteki C
#include <stdio.h>
#include <string.h>

// Własne include
#include "ultrasonic_sensor.h"

//================================================
// DEFINITIONS
//================================================

#define LED_GREEN GPIO_NUM_1
#define LED_YELLOW GPIO_NUM_2
#define LED_RED GPIO_NUM_3

#define TRIG GPIO_NUM_4
#define ECHO GPIO_NUM_5

//================================================
// STATIC VARIABLES
//================================================

static float duration, distance;

//================================================
// INITIALIZATION FUNCTIONS
//================================================

static void output_init(gpio_num_t pin)
{
    if(pin == GPIO_NUM_NC){
        return;
    }
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
}

static void input_init(gpio_num_t pin)
{
    if(pin == GPIO_NUM_NC){
        return;
    }
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);
   
}

void ultrasonic_sensor_init(void)
{
//3 Leds to display distance ranges
output_init(LED_GREEN);
output_init(LED_YELLOW);
output_init(LED_RED);
output_init(TRIG);
input_init(ECHO);
}

//================================================
// MAIN FUNCTIONS
//================================================

void measure_distance(void)
{
    //Sekwencja do wysyłania trwajacego 10 us impulsu 
        gpio_set_level(TRIG, 0);
        esp_rom_delay_us(2);
        gpio_set_level(TRIG, 1);
        esp_rom_delay_us(10);
        gpio_set_level(TRIG, 0);
        printf("Trigger sent\n");

    //zmienne przechowujące czas
        uint32_t start_time = 0;
        uint32_t end_time = 0;
        uint32_t timeout = esp_timer_get_time();
    // Czekanie aż ECHO przejdzie na 1
    while (gpio_get_level(ECHO) == 0)
    {
        if (esp_timer_get_time() - timeout > 30000)   // 30 ms
        {
            printf("Timeout: ECHO never went HIGH\n");
            return;
        }
    }

    start_time = esp_timer_get_time();
    timeout = esp_timer_get_time();

    // Czekaj na koniec impulsu ECHO
    while (gpio_get_level(ECHO) == 1)
    {
        if (esp_timer_get_time() - timeout > 30000)   // 30 ms
        {
            printf("Timeout: ECHO stayed HIGH too long\n");
            return;
        }
    }
    end_time = esp_timer_get_time();

    //Rzutujemy wynik czasowy na typ zmiennej float i obliczamy dzieląc przez 2, mnożąc przez prędkość dźwięku
        duration = (float)(end_time - start_time);
        distance = (duration * 0.0343) / 2.0;

    //Zapalanie odpowiednich diod w zależności od odległości w cm
        if (distance >= 25) {  
            gpio_set_level(LED_GREEN, 1); 
            gpio_set_level(LED_YELLOW, 0); 
            gpio_set_level(LED_RED, 0);
        } else if (distance >= 15) {  
            gpio_set_level(LED_GREEN, 0); 
            gpio_set_level(LED_YELLOW, 1); 
            gpio_set_level(LED_RED, 0); 
        } else {  
            gpio_set_level(LED_GREEN, 0); 
            gpio_set_level(LED_YELLOW, 0);  
            gpio_set_level(LED_RED, 1); 
        }
    printf("THe distance is: %.2f cm \n", distance);
}