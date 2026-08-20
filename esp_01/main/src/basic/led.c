//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "led_strip.h"  //Biblioteka do obsługi diody RGB na płytce, bo jest RGB, więc nie można jak zwykłej obsłużyć
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO
#include "esp_log.h"            //Biblioteki do obsługi logów, zamiast printf do obsługi bledow, ostrzezn, info itd.

// Standardowe biblioteki C
#include <stdio.h>              
#include <string.h>
#include <stdbool.h>

// Własne include
#include "led.h"

//================================================
// DEFINITIONS
//================================================

#define RGB_BOARD GPIO_NUM_48

#define LED_1 GPIO_NUM_NC
#define LED_2 GPIO_NUM_NC
#define LED_3 GPIO_NUM_NC

#define LED_PIN GPIO_NUM_1
#define BUTTON_PIN GPIO_NUM_2

//================================================
// STATIC VARIABLES
//================================================

//Zmienne stanu do włączania diod za pomocą klawiatury
static bool state1 = 0;
static bool state2 = 0;
static bool state3 = 0;

//================================================
// INITIALIZATION FUNCTIONS
//================================================

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

void led_init(void)
{
led_output_init(LED_1);
led_output_init(LED_2);
led_output_init(LED_3);
led_output_init(LED_PIN);

led_input_init(BUTTON_PIN);
}


//================================================
// MAIN FUNCTIONS
//================================================

/*
Funkcja, która wyłącza diodę na płytce 
*/
void rgb_board_off(void)
{
 led_strip_handle_t strip;
    led_strip_config_t cfg = {
        .strip_gpio_num = RGB_BOARD,
        .max_leds = 1,
    };
    led_strip_rmt_config_t rmt = {
        .resolution_hz = 10 * 1000 * 1000,
    };
    led_strip_new_rmt_device(&cfg, &rmt, &strip);
    led_strip_set_pixel(strip, 0, 0, 0, 0); // czerwony
    led_strip_refresh(strip);
}

/*
Funkcja do liczenia czasu od początku
*/
void time_from_start(void)
{
  int seconds =0;
  int minutes = 0;

  if ( seconds == 60){ 
    minutes += 1;
    seconds = 0;
  }

  if(minutes ==1){
    ESP_LOGI("TIME", "Time from start: %d minute, %d seconds", minutes, seconds);
  } else {
    ESP_LOGI("TIME", "Time from start: %d minutes, %d seconds", minutes, seconds);
  }
  seconds++;
}

/*
Funkcja do włączania diod na 3 pinach za pomocą odpowiedniego znaku z UART
*/
void switch_led(void)
{

  char character = 0;
  
  character =getchar();

  switch (character) {
    case '1':
      state1 = !state1;
      if (LED_1 != GPIO_NUM_NC) {
    gpio_set_level(LED_1, state1);
      }
      printf("Wcisnieto 1\n");
      break;

    case '2':
      state2 = !state2;
      if (LED_2 != GPIO_NUM_NC) {
    gpio_set_level(LED_2, state2);
      }
      printf("Wcisnieto 2\n");
      break;

    case '3':
      state3 = !state3;
      if (LED_3 != GPIO_NUM_NC) {
    gpio_set_level(LED_3, state3);
      }
      printf("Wcisnieto 3\n");
      break;
    default:
  }
}

/*
  LOGI wykorzystywane są do monitorowania działania programu zamiast printf
*/
void logs(void)
{
  ESP_LOGE("LOG_ERROR", "This is an error");
  ESP_LOGW("LOG", "This is a warning");
  ESP_LOGI("LOG", "This is an info");
  ESP_LOGD("LOG", "This is a debug");
  ESP_LOGV("LOG", "This is a verbose");
}