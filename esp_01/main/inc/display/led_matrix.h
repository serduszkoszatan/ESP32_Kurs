#ifndef LED_MATRIX_H
#define LED_MATRIX_H

void max7219_init();
//Inne funkcje użyte w led_matrix.c
void max7219_send(uint8_t reg, uint8_t data);
void from_0_to_9(uint32_t delay);

extern const uint8_t heart[8];
extern const uint8_t swastika[8];
extern const uint8_t digits[10][8];

#endif