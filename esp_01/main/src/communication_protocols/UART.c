//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

/*
UART is primarily a point-to-point communication protocol, meaning it is designed to connect exactly two devices directly,
one acting as the transmitter and the other as the receiver. Unlike bus-based protocols,
UART does not support multiple devices sharing the same communication line without additional hardware.
The protocol is asynchronous, which means it does not require a shared clock signal between devices.
Instead, both devices must agree on a common communication speed, known as the baud rate.

UART communication uses two main pins:
TX (Transmit) sends data from the device
RX (Receive) receives data from another device
TX connects to RX, and RX connects to TX.
In addition, we link the GND of the two devices so that they use the same voltage reference.


To work with the UART protocol, we first need to make sure that the two connected devices can understand each other:
Baud rate: Represents the communication speed, measured in bits per second (bps). Must be the same. 
Data bits length: Defines the number of bits used to represent the actual data in each frame. 
The most common value is 8 bits, but it can also be 5, 6, 7, or 9 bits depending on the application.
Parity bit: Used for basic error detection. The parity can be:
Even parity: when we set even parity the party bit will be set to one if total number of 1s in the data becomes is odd.
Odd parity: when we set odd parity, the party bit will be set to one if total number of 1s in the data becomes is even.
Stop bit(s): Indicates the end of the data frame. It also gives the receiver time to prepare for the next transmission. 
Common configurations use 1 or 2 stop bits.

*/
//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO
#include "driver/uart.h"  

// Standardowe biblioteki C
#include <stdbool.h>

// Własne include
#include "UART.h"

//================================================
// DEFINITIONS
//================================================

#define UART_PORT UART_NUM_1  
#define UART_TX_PIN 17  
#define UART_RX_PIN 18  
#define BUF_SIZE 1024  
  
//================================================
// STATIC VARIABLES
//================================================



//================================================
// INITIALIZATION FUNCTIONS
//================================================

	uart_config_t uart_config = {  
		.baud_rate = 115200,  
		.data_bits = UART_DATA_8_BITS,  
		.parity = UART_PARITY_DISABLE,  
		.stop_bits = UART_STOP_BITS_1,  
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,  
		.source_clk = UART_SCLK_DEFAULT,  
	};  
    

//================================================
// MAIN FUNCTIONS
//================================================

