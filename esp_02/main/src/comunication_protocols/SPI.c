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
#include "driver/spi_slave.h"

// Standardowe biblioteki C
#include <string.h>

// Własne include
#include "SPI.h"

//================================================
// DEFINITIONS
//================================================

#define SPI_SLAVE_MOSI 11
#define SPI_SLAVE_MISO 12
#define SPI_SLAVE_SCLK 13
#define SPI_SLAVE_CS 10
#define SPI_HOST SPI2_HOST

//================================================
// STATIC VARIABLES
//================================================

    static char receive_buffer[4] = {0};
    static spi_slave_transaction_t transaction = {0};

//================================================
// INITIALIZATION FUNCTIONS
//================================================

void spi_init(void) 
{
    //we configure the SPI bus and the SPI slave interface.
        spi_bus_config_t bus_configuration = {
        .mosi_io_num = SPI_SLAVE_MOSI,
        .miso_io_num = SPI_SLAVE_MISO,
        .sclk_io_num = SPI_SLAVE_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    spi_slave_interface_config_t slave_configuration = {
        .mode = 0,
        .spics_io_num = SPI_SLAVE_CS,
        .queue_size = 3,
        .flags = 0,
    };

    //ustawienie buforu do odczytu
    transaction.length = 8 * 4; // 4 bytes
    transaction.rx_buffer = receive_buffer;

    //After configuring the settings, we initialize the SPI slave device using the spi_slave_initialize() function.
        ESP_ERROR_CHECK(spi_slave_initialize(SPI_HOST, &bus_configuration, &slave_configuration, SPI_DMA_CH_AUTO));
}

//================================================
// MAIN FUNCTIONS
//================================================

/*
Finally, we create a buffer to store the data we will receive from the SPI master.
We also create a transaction structure, set the data length, and provide a pointer to the buffer where the received data will be stored.
Inside the infinite loop, we prepare the transaction and pass it to the SPI driver using spi_slave_transmit().
*/

void spi_read (void)
{
            ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST, &transaction, portMAX_DELAY));
}

char* spi_get_buffer(void)
{
    return receive_buffer;
}