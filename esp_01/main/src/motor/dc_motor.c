//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

/*
The speed of the motor depends on the supplied voltage,
and the direction of rotation depends on the polarity of the voltage applied to the motor terminals.
The Esp32s3 can only supply about 20 mA of current, which can't directly power most DC motors. 
To solve this we use a Motor Driver (like the L298N or L293D) or Transistor circuit.
*/

/*
L298N motor driver
IN1 and IN2 control the rotation direction of motor A. 
We set one HIGH and the other LOW to select the direction, we connect IN1 to GPIO 20 and IN2 to GPIO 19
ENA controls the motor A speed using PWM, we use the GPIO 21 pin for that
Zworka przy ENA i ENB zwiera ten pin z 5V, więc silnik zawsze ma max obroty
Masa ESP32 musi być podłączona do masy L298N
*/

/*
Jeśli sliniki potrzebują dostać 5V to trzeba zasilić minimum 7V, bo spadek napięcia na mostku H to około 2V.

Zakres małego gównianego silnika to 4095 do 2095 przy 12 bitach


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
#include "dc_motor.h"

//================================================
// DEFINITIONS
//================================================

//Speed control for motor A
#define MOTOR_A_SPEED_CONTROL GPIO_NUM_21   //ENA
//Direction control for motor A
#define MOTOR_A_IN1 GPIO_NUM_20
#define MOTOR_A_IN2 GPIO_NUM_19

//Speed control for motor B
#define MOTOR_B_SPEED_CONTROL GPIO_NUM_10   //ENB
//Direction control for motor B
#define MOTOR_B_IN1 GPIO_NUM_11
#define MOTOR_B_IN2 GPIO_NUM_12

//Minimum PWM for specific motor, in this case little yellow shit
#define MIN_PWM 2095

//================================================
// STATIC VARIABLES
//================================================



//================================================
// INITIALIZATION FUNCTIONS
//================================================

/*
Konfiguruje:
1. Wspólny timer PWM.
2. Kanały dla 2 silników
*/
void dc_motor_init(void)
{
    // Konfiguracja wspólnego timera PWM
    ledc_timer_config_t ledc_timer =
    {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .timer_num       = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz         = 5000,
        .clk_cfg         = LEDC_AUTO_CLK
    };

    ledc_timer_config(&ledc_timer);

    // Konfiguracja kanału dla silnika A
        ledc_channel_config_t channel_a =
        {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel    = LEDC_CHANNEL_0,
            .timer_sel  = LEDC_TIMER_0,
            .gpio_num   = MOTOR_A_SPEED_CONTROL,
            .duty       = 0,      // Początkowo sygnał wyłączony
            .hpoint     = 0
        };
        ledc_channel_config(&channel_a);

        // Konfiguracja kanału dla silnika B
        ledc_channel_config_t channel_b =
        {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel    = LEDC_CHANNEL_1,
            .timer_sel  = LEDC_TIMER_0,
            .gpio_num   = MOTOR_B_SPEED_CONTROL,
            .duty       = 0,      // Początkowo sygnał wyłączony
            .hpoint     = 0
        };

    ledc_channel_config(&channel_b);

    gpio_reset_pin(MOTOR_A_IN2);
    gpio_reset_pin(MOTOR_A_IN1);
    gpio_set_direction(MOTOR_A_IN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_A_IN1, GPIO_MODE_OUTPUT);
    gpio_set_level(MOTOR_A_IN2, 0);
    gpio_set_level(MOTOR_A_IN1, 0);

    gpio_reset_pin(MOTOR_B_IN2);
    gpio_reset_pin(MOTOR_B_IN1);
    gpio_set_direction(MOTOR_B_IN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_B_IN1, GPIO_MODE_OUTPUT);
    gpio_set_level(MOTOR_B_IN2, 0);
    gpio_set_level(MOTOR_B_IN1, 0);
    }


//================================================
// MAIN FUNCTIONS
//================================================
    void set_motor_a (uint8_t speed, uint8_t direction ) 
    {
        uint32_t duty;
        if (speed > 100) {
            speed = 100;
        }

       if (speed == 0) {
            gpio_set_level(MOTOR_A_IN2, 0);
            gpio_set_level(MOTOR_A_IN1, 0);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            return;
        }

       duty = MIN_PWM + ((4095 - MIN_PWM) * speed) / 100;

        if(direction == 0){
            gpio_set_level(MOTOR_A_IN2, 1);
            gpio_set_level(MOTOR_A_IN1, 0);
        } else {
            gpio_set_level(MOTOR_A_IN2, 0);
            gpio_set_level(MOTOR_A_IN1, 1);
        }
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }


    void set_motor_b (uint8_t speed, uint8_t direction ) 
    {
        uint32_t duty;
        if (speed > 100) {
            speed = 100;
        }

        if (speed == 0) {
            gpio_set_level(MOTOR_B_IN2, 0);
            gpio_set_level(MOTOR_B_IN1, 0);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
            return;
        }

       duty = MIN_PWM + ((4095 - MIN_PWM) * speed) / 100;

        if(direction == 0){
            gpio_set_level(MOTOR_B_IN2, 1);
            gpio_set_level(MOTOR_B_IN1, 0);
        } else {
            gpio_set_level(MOTOR_B_IN2, 0);
            gpio_set_level(MOTOR_B_IN1, 1);
        }
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    }
