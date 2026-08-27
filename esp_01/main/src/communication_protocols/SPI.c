//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

//Wkleić do I2C.c
/*The I²C protocol solves the problem of connecting multiple devices using only two wires, 
but it has a small limitation: it uses a single data line for both sending and receiving, it operates in half-duplex mode
 it cannot send and receive data at the same time, sharing a bus with many devices requires addressing which adds overhead.
To achieve faster and simpler communication, another protocol is commonly used: SPI.
*/

/*
SPI, which stands for Serial Peripheral Interface, is a synchronous serial communication protocol 
that allows devices to communicate at very high speeds in full-duplex mode. 
In an SPI network, one device acts as the master and controls the communication,
while one or more slave devices respond to the master. Unlike I2C, SPI does not use addresses.
Instead, the master uses a dedicated Chip Select (CS) line for each slave to choose which device to communicate with.
-The master device controls the communication, generates the clock signal on the SCLK line, and manages the CS lines.
-The slave devices listen to the MOSI line and send data back on the MISO line when their specific CS line is activated.

Instead of just two wires, SPI typically uses four communication lines:
    -MOSI (Master Out Slave In) used to send data from the master to the slave
    -MISO (Master In Slave Out) used to send data from the slave to the master
    -SCLK (Serial Clock) used to synchronize the communication using a clock signal
    -CS (Chip Select) / SS (Slave Select) used by the master to select a specific slave device

SPI Working Principle:
    To work with the SPI protocol, all connected devices must share the common bus lines (MOSI, MISO, SCLK), 
and the master must have a separate CS line for every slave.
Communication is always initiated and controlled by a master device.
    When the bus is idle, the SCLK line rests (HIGH or LOW depending on the configured mode), 
and the CS lines are kept HIGH to keep the slave devices inactive.
    To begin communication, the master pull the CS line of the target slave LOW. 
This signals the specific device that a transmission is starting.
    Next, the master starts generating the clock pulses on the SCLK line. 
For every clock cycle, one bit of data is pushed out on the MOSI line by the master,
and one bit is pushed out on the MISO line by the slave. 
Because of the shift-register architecture, data is always exchanged.
    When the communication is finished, the master stops the clock signal and pulls the CS line back HIGH. 
This releases the slave and returns the bus to the idle state.


Using SPI protocol Esp32:
The ESP32-S3 includes four SPI controllers:
-SPI0 and SPI1 are mainly reserved for internal communication with external flash memory and PSRAM.
-SPI2 and SPI3 are available for general-purpose applications.

Each SPI controller can operate independently in one of two modes:
-Master mode: the ESP32-S3 controls communication, generates the serial clock (SCLK), and manages the chip-select (CS) signal.
-Slave mode: the ESP32-S3 responds to transactions initiated by an external SPI master.
SPI2 and SPI3 can be routed to almost any available GPIO pins through the ESP32-S3 GPIO matrix,
providing flexible pin assignment for SPI signals such as MOSI, MISO, SCLK, and CS.
*/

//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO
#include "driver/spi_master.h"

// Standardowe biblioteki C
#include <string.h>

// Własne include
#include "SPI.h"

//================================================
// DEFINITIONS
//================================================

#define SPI_MASTER_MOSI GPIO_NUM_11
#define SPI_MASTER_MISO GPIO_NUM_12
#define SPI_MASTER_SCLK GPIO_NUM_13
#define SPI_MASTER_CS   GPIO_NUM_10
#define SPI_HOST        SPI2_HOST


//================================================
// STATIC VARIABLES
//================================================

static spi_device_handle_t spi_handle;

//================================================
// INITIALIZATION FUNCTIONS
//================================================

void spi_init(void)
{
//In this structure, we assign the MOSI, MISO, and SCLK pins, and define the maximum transfer size.
    spi_bus_config_t bus_configuration = {
        .mosi_io_num = SPI_MASTER_MOSI,
        .miso_io_num = SPI_MASTER_MISO,
        .sclk_io_num = SPI_MASTER_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
// install the SPI master bus using the spi_bus_initialize() function.
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_HOST, &bus_configuration, SPI_DMA_CH_AUTO));


/*
we configure the slave device parameters using the spi_device_interface_config_t structure. 
Here, we specify the clock speed (1 MHz) clock_speed_hz, the SPI mode mode, and assign the CS pin, spics_io_num. 
Finally we set the transaction queue size. This sets how many transactions can be 'in the air' at the same time.
*/    
    spi_device_interface_config_t device_configuration = {
        .clock_speed_hz = 1000000,
        .mode = 0,
        .spics_io_num = SPI_MASTER_CS,
        .queue_size = 1,
    };
//Then, we add the slave device to the SPI bus using spi_bus_add_device().
    ESP_ERROR_CHECK(spi_bus_add_device(SPI_HOST, &device_configuration, &spi_handle));
}

//================================================
// MAIN FUNCTIONS
//================================================

void spi_write (const char* data) {
    //tworzymy strukturę 
    spi_transaction_t transaction = {0};
    //memset(adres, wartość, liczba_bajtów);
    //Ustawia pod adresem transakcji, wszystkie bajty na 0, w strukturze typu spi_transaction_t   
    //memset(&transaction, 0, sizeof(transaction)); zamiast tego użyte "= {0};" po stworzeniu struktury
    //transaction.length podaje długość transmisji w bitach.
    //strlen(data) zwraca liczbę bajtów.
    transaction.length = 4*8;   //Wysyłamy zawsze 4 bajty, ale można 4 zastąpić strlen(data), wtedy jest uniwersalne
    transaction.tx_buffer = data;
    ESP_ERROR_CHECK(spi_device_transmit(spi_handle, &transaction));
}
