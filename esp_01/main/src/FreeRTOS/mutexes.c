//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================
/*
Resource Management: Mutexes
Queues are great for moving data, but what if two tasks need to access the exact same physical hardware resource?
Imagine two tasks both trying to use the I²C bus or the UART terminal at the exact same millisecond. 
The data sent to the terminal will become garbled, mixing the outputs of both tasks together. 
This is known as a Race Condition.

To prevent this, FreeRTOS provides a Mutex (Mutual Exclusion). A Mutex acts like a physical key to a locked room.

Before a task can use a shared resource, it must Take the Mutex.
If another task tries to use the resource, it sees the Mutex is gone
and immediately enters the Blocked state, waiting at the door.
When the first task finishes, it "Gives" the Mutex back.
The waiting task then grabs the Mutex, locks the door, and proceeds.

Programming a Mutex
To work with mutexes, we first need to create a global variable of type SemaphoreHandle_t. 
After that, we can manipulate the mutex using two main functions:

xSemaphoreCreateMutex() Creates a mutex object and returns a handle to it.
xSemaphoreTake() Attempts to lock the mutex before accessing a shared resource. 
    It takes two arguments: the mutex handle and a timeout value that specifies how long the task should wait
    if the mutex is already locked.
xSemaphoreGive() Releases the mutex after the shared resource is no longer being used, allowing other tasks to access it.


*/

//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO
#include "freertos/semphr.h"  //Biblioteka do obsługi mutex
#include <stdio.h>  

// Standardowe biblioteki C

// Własne include
#include "mutexes.h"

//================================================
// DEFINITIONS
//================================================



//================================================
// STATIC VARIABLES
//================================================

SemaphoreHandle_t terminal_mutex;       //

//================================================
// INITIALIZATION FUNCTIONS
//================================================



//================================================
// MAIN FUNCTIONS
//================================================

