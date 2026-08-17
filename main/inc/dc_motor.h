#ifndef DC_MOTOR_H
#define DC_MOTOR_H


//Inne funkcje użyte w dc_motor.c
void dc_motor_init(void);
void set_motor_a (uint8_t speed, uint8_t direction );
void set_motor_b (uint8_t speed, uint8_t direction );

#endif