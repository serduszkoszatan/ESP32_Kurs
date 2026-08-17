//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

/*
28BYJ-48 5V stepper motor with ULN2003 driver.

IN1 -> GPIO 15
IN2 -> GPIO 16
IN3 -> GPIO 17
IN4 -> GPIO 18

The motor is controlled using a half-step sequence.
One full revolution is approximately 4096 half-steps.
*/

//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO

// Standardowe biblioteki C

// Własne include
#include "ULN2003.h"

//================================================
// DEFINITIONS
//================================================

#define STEPPER_IN1 GPIO_NUM_15
#define STEPPER_IN2 GPIO_NUM_16
#define STEPPER_IN3 GPIO_NUM_17
#define STEPPER_IN4 GPIO_NUM_18

#define STEPS_PER_REVOLUTION 4096
#define STEP_DELAY_MS 2

//================================================
// STATIC VARIABLES
//================================================

static int32_t current_step = 0;
static uint8_t sequence_index = 0;

static const uint8_t step_sequence[8][4] =
{
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

//================================================
// INITIALIZATION FUNCTIONS
//================================================

void uln2003_init(void)
{
    gpio_reset_pin(STEPPER_IN1);
    gpio_reset_pin(STEPPER_IN2);
    gpio_reset_pin(STEPPER_IN3);
    gpio_reset_pin(STEPPER_IN4);

    gpio_set_direction(STEPPER_IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(STEPPER_IN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(STEPPER_IN3, GPIO_MODE_OUTPUT);
    gpio_set_direction(STEPPER_IN4, GPIO_MODE_OUTPUT);

    gpio_set_level(STEPPER_IN1, 0);
    gpio_set_level(STEPPER_IN2, 0);
    gpio_set_level(STEPPER_IN3, 0);
    gpio_set_level(STEPPER_IN4, 0);
}


//================================================
// MAIN FUNCTIONS
//================================================

static void uln2003_apply_step(void)
{
    gpio_set_level(STEPPER_IN1, step_sequence[sequence_index][0]);
    gpio_set_level(STEPPER_IN2, step_sequence[sequence_index][1]);
    gpio_set_level(STEPPER_IN3, step_sequence[sequence_index][2]);
    gpio_set_level(STEPPER_IN4, step_sequence[sequence_index][3]);
}

void uln2003_step(int32_t steps)
{
    int32_t direction;
    int32_t steps_to_move;

    if(steps > 0){
        direction = 1;
        steps_to_move = steps;
    }else{
        direction = -1;
        steps_to_move = -steps;
    }
    for(int32_t i = 0; i < steps_to_move; i++){
        if(direction > 0){
            sequence_index++;
            if(sequence_index >= 8){
                sequence_index = 0;
            }
            current_step++;
        }else{
            if(sequence_index == 0){
                sequence_index = 7;
            }else{
                sequence_index--;
            }
            current_step--;
        }
        if(current_step >= STEPS_PER_REVOLUTION){
            current_step -= STEPS_PER_REVOLUTION;
        }
        if(current_step < 0){
            current_step += STEPS_PER_REVOLUTION;
        }
        uln2003_apply_step();
        vTaskDelay(pdMS_TO_TICKS(STEP_DELAY_MS));
    }
}

void uln2003_set_angle(float angle)
{
    int32_t target_step;
    int32_t steps;
    while(angle < 0){
        angle += 360.0f;
    }
    while(angle >= 360.0f){
        angle -= 360.0f;
    }
    target_step = (int32_t)((angle * STEPS_PER_REVOLUTION) / 360.0f);
    steps = target_step - current_step;
    if(steps > STEPS_PER_REVOLUTION / 2){
        steps -= STEPS_PER_REVOLUTION;
    }
    if(steps < -(STEPS_PER_REVOLUTION / 2)){
        steps += STEPS_PER_REVOLUTION;
    }
    uln2003_step(steps);
}

void uln2003_release(void)
{
    gpio_set_level(STEPPER_IN1, 0);
    gpio_set_level(STEPPER_IN2, 0);
    gpio_set_level(STEPPER_IN3, 0);
    gpio_set_level(STEPPER_IN4, 0);
}