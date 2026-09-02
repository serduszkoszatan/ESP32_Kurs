#ifndef BINARY_SEMAPHORES_H
#define BINARY_SEMAPHORES_H

//Inne funkcje użyte w binary_sempahores.c
static void IRAM_ATTR gpio_isr_handler(void* arg);
void led_toggle_task(void *pvParameters);
#endif