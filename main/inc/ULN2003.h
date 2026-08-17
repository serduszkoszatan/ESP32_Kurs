#ifndef ULN2003_H
#define ULN2003_H

#include <stdint.h>

void uln2003_init(void);
//Inne funkcje użyte w template_file.c

void uln2003_step(int32_t steps);
void uln2003_set_angle(float angle);
void uln2003_release(void);
#endif
