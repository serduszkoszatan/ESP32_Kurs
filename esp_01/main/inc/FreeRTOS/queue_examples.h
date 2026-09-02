#ifndef QUEUE_EXAMPLES_H
#define QUEUE_EXAMPLES_H

//Inne funkcje użyte w template_file.c
void blink_task_1(void *pvParameters);
void blink_task_2(void *pvParameters);

void producer_task(void *pvParameters);
void consumer_task(void *pvParameters);

#endif