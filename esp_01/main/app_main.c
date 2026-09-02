/*
Każda kolejna zmiana GitHub:
edycja → Stage → Commit → Sync Changes
*/

#include <stdio.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki

#include "esp_heap_caps.h"

void app_main(void){

   printf("Free heap before allocation: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));

   int *data = malloc(80000 * sizeof(int));
   printf("Free heap after allocation: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
   
   if (data == NULL) {
    printf("Allocation failed!\n");
    return;
}
   free(data);
   printf("Free heap after free(): %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));

}
