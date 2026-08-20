#ifndef ANALOG_READ_H
#define ANALOG_READ_H

#include "esp_adc/adc_oneshot.h"

#define READ_PIN_1 ADC_CHANNEL_0

void analog_read_init(void);
int analog_read(adc_channel_t channel);

//Inne funkcje użyte w template_file.c

#endif