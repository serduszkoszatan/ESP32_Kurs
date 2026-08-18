#ifndef LCD_16x2_H
#define LCD_16x2_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

void initialize_lcd(void);  
  
void clear_display(void);  
void move_to(uint8_t line, uint8_t column);  
  
void write_character(char character);  
void write_word(char message[]);  
  
#endif