#ifndef PWM_H
#define PWM_H

#include "driver/gpio.h"
#include <stdint.h>

#define PWM_MAX_DUTY 4095

void pwm_init(void);

void pwm_set(gpio_num_t pin, uint32_t duty);

#endif