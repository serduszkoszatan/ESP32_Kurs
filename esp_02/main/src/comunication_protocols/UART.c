//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO
#include "driver/uart.h"

// Standardowe biblioteki C

// Własne include
#include "UART.h"

//================================================
// DEFINITIONS
//================================================

#define UART_PORT UART_NUM_1  
#define UART_TX_PIN 7  
#define UART_RX_PIN 6
#define BUF_SIZE 1024  

#define LED_PIN GPIO_NUM_1

//================================================
// STATIC VARIABLES
//================================================

static uint8_t data[BUF_SIZE];

//================================================
// INITIALIZATION FUNCTIONS
//================================================

void UART_init (void) {

    uart_config_t uart_config = {  
		.baud_rate = 115200,  
		.data_bits = UART_DATA_8_BITS,  
		.parity = UART_PARITY_DISABLE,  
		.stop_bits = UART_STOP_BITS_1,  
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,  
		.source_clk = UART_SCLK_DEFAULT,  
	};  

    uart_driver_install(UART_PORT, BUF_SIZE, 0, 0, NULL, 0);  
	uart_param_config(UART_PORT, &uart_config);  
	uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);  
}

void UART_read(void)
{
    printf("UART_read\n");

    int len = uart_read_bytes(
        UART_PORT,
        data,
        BUF_SIZE,
        pdMS_TO_TICKS(100)
    );

    if (len >= 2 && data[0] == 'O' && data[1] == 'N')
    {
        gpio_set_level(GPIO_NUM_1, 1);
        printf("ON\n");
    }
    else if (len >= 3 &&
             data[0] == 'O' &&
             data[1] == 'F' &&
             data[2] == 'F')
    {
        gpio_set_level(GPIO_NUM_1, 0);
        printf("OFF\n");
    }
}

//================================================
// MAIN FUNCTIONS
//================================================

