//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

/*
For a standard servo motor, and 50Hz:
A 0.5 ms pulse rotates the shaft to approximately −90°
A 1.45 ms pulse sets the shaft to 0° (center position)
A 2.5 ms pulse rotates the shaft to approximately +90°

czas = 1.5 + kąt/90 [ms] , kąt od −90° do +90°

PWM value = 4095 / 20 * czas

Dla:
12 bitów - 4095
50Hz - 20
*/

//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO
#include "driver/ledc.h"

// Standardowe biblioteki C

// Własne include
#include "servo_drive.h"

//================================================
// DEFINITIONS
//================================================

#define SERVO_PIN_1 GPIO_NUM_21

//================================================
// STATIC VARIABLES
//================================================



//================================================
// INITIALIZATION FUNCTIONS
//================================================

void servo_drive_init (void)
{
    
    ledc_timer_config_t servo_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_12_BIT, 
        .freq_hz          = 50, // 50 Hz
        .clk_cfg          = LEDC_AUTO_CLK
    };

    ledc_timer_config(&servo_timer);

    ledc_channel_config_t servo_channel_1 = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .gpio_num       = SERVO_PIN_1,
        .duty           = 0,  
        .hpoint         = 0
    };

    ledc_channel_config(&servo_channel_1);

}

//================================================
// MAIN FUNCTIONS
//================================================

void servo_set_angle(int8_t angle)
{
    uint32_t pulse;
    uint32_t duty;
    if(angle < -90){
        angle = -90;
    }
    if(angle > 90){
        angle = 90;
    }
    pulse = 1500 + ((int32_t)angle * 1000) / 90;
    duty = (pulse * 4095) / 20000;
    
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}