//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

/*
I2C, which stands for Inter-Integrated Circuit, is a serial communication protocol that allows multiple devices to communicate using only two wires.
The two communication lines used in the I2C protocol are:
SDA (Serial Data Line) used to transfer data between devices
SCL (Serial Clock Line) used to synchronize the communication using a clock signal

In an I²C network, one device acts as the master and controls the communication, while one or more slave devices respond to the master.
Each slave device has a unique address, allowing multiple devices to operate on the same bus without requiring separate communication lines.

When connecting devices on an I²C bus, both the SDA and SCL lines require pull-up resistors connected to the supply voltage (VCC). 
This is because I²C devices use an open-drain/open-collector configuration, meaning devices can pull the lines low but cannot drive them high directly.
The pull-up resistors ensure the lines return to a high state when no device is actively pulling them low.


I²C communication follows these main principles:
Master-slave communication: The master device controls the entire communication process. 
    It generates the clock signal on the SCL line and decides when communication starts and stops.
Addressing: Each slave device connected to the bus has a unique address, usually 7-bit or 10-bit long. 
    The master sends this address to select the target slave device before exchanging data.
Synchronous communication: Data transfer is synchronized using the SCL clock line. 
    Unlike UART, I²C does not require baud rate matching because the master provides the clock timing directly.
ACK/NACK response: After every transmitted byte, the receiving device sends an acknowledgment bit:
    ACK (Acknowledge): indicates that the data was received successfully.
    NACK (Not Acknowledge): indicates that the receiver did not accept the data or communication should stop.

The ESP32-S3 contains two hardware I²C controllers responsible for managing communication on the I²C bus.
The controllers handles generating the clock signal, transmitting and receiving data, detecting START and STOP conditions,
and managing ACK/NACK responses automatically.
Having two independent I²C controllers allows the ESP32-S3 to communicate with multiple I²C buses at the same time,
any GPIO pins can be configured as the SDA and SCL pins.


I2C slave addressing:
When using another microcontroller as an I2C slave, we can choose and define its slave address ourselves.

For external devices such as sensors, displays, or EEPROMs, the I2C address is defined by the manufacturer and should be
checked in the device datasheet.

Some devices allow selecting between several addresses using address pins or configuration settings.

*/
//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"     
#include "driver/gpio.h"        
#include "driver/i2c_master.h"

// Standardowe biblioteki C
#include <stdbool.h>
#include <string.h>

// Własne include
#include "I2C.h"

//================================================
// DEFINITIONS
//================================================

#define I2C_MASTER_SCL  GPIO_NUM_8
#define I2C_MASTER_SDA  GPIO_NUM_9
#define I2C_PORT I2C_NUM_0
#define SLAVE_ADRR 0x28

//================================================
// STATIC VARIABLES
//================================================

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t device_handle;

//================================================
// INITIALIZATION FUNCTIONS
//================================================

void I2C_init (void)
{

//In this structure, we select the I²C controller, assign the SDA and SCL pins, 
//and enable the internal pull-up resistors, and also set the master to ignore any pulse on the I2C line that lasts less than 7 clock cycles.
   
 i2c_master_bus_config_t i2c_master_configuration = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port   = I2C_PORT,
    .scl_io_num = I2C_MASTER_SCL,
    .sda_io_num = I2C_MASTER_SDA,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};
//we install the I²C master bus using the i2c_new_master_bus() function.
i2c_new_master_bus(&i2c_master_configuration, &bus_handle);

//configure the slave device using the i2c_device_config_t structure. 
//Here, we specify the slave address, we set the length of address and set the I²C clock frequency to 100 kHz
i2c_device_config_t i2c_device_configuration = {
    .dev_addr_length    = I2C_ADDR_BIT_LEN_7,
    .device_address     = SLAVE_ADRR,
    .scl_speed_hz       = 100000, 
};
//we add the slave device to the I²C master bus using the i2c_master_bus_add_device() function.
i2c_master_bus_add_device(bus_handle, &i2c_device_configuration, &device_handle);
}

//================================================
// MAIN FUNCTIONS
//================================================

void i2c_write (const char *data) 
{
    i2c_master_transmit(device_handle, (const uint8_t*)data, strlen(data), -1);
}
