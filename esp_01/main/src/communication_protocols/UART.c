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

// Własne include
#include "UART.h"

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO
#include "driver/uart.h"  

// Standardowe biblioteki C
#include <stdbool.h>
#include <string.h>

//================================================
// DEFINITIONS
//================================================

#define UART_PORT UART_NUM_1  
#define UART_TX_PIN GPIO_NUM_17  
#define UART_RX_PIN GPIO_NUM_18
#define BUF_SIZE 1024 
  
//================================================
// STATIC VARIABLES
//================================================



//================================================
// INITIALIZATION FUNCTIONS
//================================================
 
//Przypisanie przycisku do PINU 1, tylko na potrzeby kursu, inaczej do wyjebania



void UART_init (void) {
//configuration the UART communication settings
	uart_config_t uart_config = {  
		.baud_rate = 115200,  
		.data_bits = UART_DATA_8_BITS,  
		.parity = UART_PARITY_DISABLE,  
		.stop_bits = UART_STOP_BITS_1,  
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,  
		.source_clk = UART_SCLK_DEFAULT,  
	};  

// UART_PORT - wybór używanego UART-a, zdefiniowany w definicjach wyżej
// BUF_SIZE - rozmiar bufora odbiorczego w bajtach, zdefiniowany w definicjach wyżej
// 0 - brak bufora nadawczego TX.
// 0 - nie używamy kolejki zdarzeń UART.
// NULL - brak uchwytu do kolejki zdarzeń.
// 0 - domyślne ustawienia przerwań.
	uart_driver_install(UART_PORT, BUF_SIZE, 0, 0, NULL, 0);
// Wprowadzamy wcześniej przygotowaną konfigurację UART. Przekazujemy port oraz adres struktury uart_config.
	uart_param_config(UART_PORT, &uart_config);	

	uart_set_pin(
	UART_PORT,
	UART_TX_PIN, 
	UART_RX_PIN,  
	UART_PIN_NO_CHANGE, 
	UART_PIN_NO_CHANGE
	);  

}
//================================================
// MAIN FUNCTIONS
//================================================

void UART_write(const char *data)
{
    uart_write_bytes(UART_PORT, data, strlen(data));
}
