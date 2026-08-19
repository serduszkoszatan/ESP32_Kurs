//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

/*
To simplify working with an LED matrix, we use a driver chips such as the MAX7219.
The MAX7219 expects data in a 16-bit format, which is divided into two bytes.
The first byte represents the register address. The second byte contains the actual data that will be written to that register.    

We communicate with this driver using SPI communication, where the data flow is controlled using three pins:
Data pin (DIN): This is where the actual information flows in.
    We send one bit at a time. The first 8 bits represent the register address, and the next 8 bits represent the value or pattern.
Clock pin (CLK): This provides the timing.
    On each rising edge of the clock signal, 
    the MAX7219 samples and shifts in one bit from the DIN line into its internal shift register.
CS / LOAD pin: 
    This acts as the latch or chip select signal (active low).
    While CS is held low, the chip listens and shifts in bits with every CLK pulse.
    After exactly 16 clock pulses (16 bits sent), we pull CS high. 
    This rising edge tells the MAX7219, “OK, the full command is ready now decode it and apply it to the display or registers.”

In short, to communicate with the MAX7219, we first pull CS (Chip Select) low to indicate that data transmission is starting.
Then, for 16 clock cycles, we set the desired bit on the DIN (Data) pin and pulse the CLK (Clock) pin high and then low.

*/
//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO
#include "rom/ets_sys.h"

// Standardowe biblioteki C

// Własne include
#include "led_matrix.h"

//================================================
// DEFINITIONS
//================================================

#define CLK_PIN GPIO_NUM_19
#define CS_PIN GPIO_NUM_20
#define DIN_PIN GPIO_NUM_21

//================================================
// STATIC VARIABLES
//================================================

const uint8_t  heart[8] = {
    0b00000000,
    0b01100110,
    0b11111111,
    0b11111111,
    0b11111111,
    0b01111110,
    0b00111100,
    0b00011000
  };

const uint8_t  swastika[8] = {
    0b00000000,
    0b01001111,
    0b01001000,
    0b01001000,
    0b01111111,
    0b00001001,
    0b00001001,
    0b01111001
  };

const uint8_t digits[10][8] = {
    {
        0b00000000,
        0b00111100,
        0b01100110,
        0b01101110,
        0b01110110,
        0b01100110,
        0b01100110,
        0b00111100
    },
    {
        0b00000000,
        0b00011000,
        0b00111000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b01111110
    },
    {
        0b00000000,
        0b00111100,
        0b01100110,
        0b00000110,
        0b00001100,
        0b00110000,
        0b01100000,
        0b01111110
    },
    {
        0b00000000,
        0b00111100,
        0b01100110,
        0b00000110,
        0b00011100,
        0b00000110,
        0b01100110,
        0b00111100
    },
    {
        0b00000000,
        0b00001100,
        0b00011100,
        0b00101100,
        0b01001100,
        0b01111110,
        0b00001100,
        0b00001100
    },
    {
        0b00000000,
        0b01111110,
        0b01100000,
        0b01100000,
        0b00111100,
        0b00000110,
        0b01100110,
        0b00111100
    },
    {
        0b00000000,
        0b00111100,
        0b01100110,
        0b01100000,
        0b01111100,
        0b01100110,
        0b01100110,
        0b00111100
    },
    {
        0b00000000,
        0b01111110,
        0b00000110,
        0b00001100,
        0b00011000,
        0b00110000,
        0b00110000,
        0b00110000
    },
    {
        0b00000000,
        0b00111100,
        0b01100110,
        0b01100110,
        0b00111100,
        0b01100110,
        0b01100110,
        0b00111100
    },
    {
        0b00000000,
        0b00111100,
        0b01100110,
        0b01100110,
        0b00111110,
        0b00000110,
        0b01100110,
        0b00111100
    }
};

//================================================
// INITIALIZATION FUNCTIONS
//================================================


//================================================
// MAIN FUNCTIONS
//================================================

/*
Petla for sprawdza bit po bicie wyslanego bajtu:
Mamy:               data = 10100101
Pierwsza maska:     bits[0] = 10000000
Robimy:
  10100101
& 10000000
czyli nie jest zerem. A więc pierwszy bit: 1.
*/

void send_byte(uint8_t data){

   const uint8_t bits[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

    for (int i = 0; i < 8; i++) {
        gpio_set_level(CLK_PIN, 0);
        if ((data & bits[i]) != 0)
            gpio_set_level(DIN_PIN, 1);
        else
            gpio_set_level(DIN_PIN, 0);

        ets_delay_us(2);            // Czeka 2 µs, żeby ustawiony wcześniej bit na DIN był stabilny.
        gpio_set_level(CLK_PIN, 1); // To jest sygnał dla odbiornika: „odczytaj teraz bit znajdujący się na DIN”.
        ets_delay_us(2);            // Utrzymuje CLK w stanie HIGH, żeby odbiornik miał czas poprawnie zarejestrować bit.
        gpio_set_level(CLK_PIN, 0);
    }
}

void max7219_send(uint8_t reg, uint8_t data){
    //ustawiamy CS na niski, zaczynamy komnunikację
    gpio_set_level(CS_PIN, 0);

    send_byte(reg);
    send_byte(data);

    //ustawiamy CS na wysoki, kończymy komnunikację
    gpio_set_level(CS_PIN, 1);

}

/*

STEPS IN max7219_init FUNCTION:

Each call to max7219_send(reg, data) writes a value to a specific register inside the driver. 
The initialization function starts by configuring the display test register (0x0F) with 0x00,
which disables test mode. This is important because test mode forces all LEDs on.

Next, the shutdown register (0x0C) is set to 0x01. Despite its name, this actually
brings the device out of shutdown mode and turns the display on. 
Without this step, the MAX7219 would remain inactive.

Then, the scan limit register (0x0B) is set to 0x07. 
This tells the driver to use all 8 digits (from 0 to 7). 
If a smaller value were used, only part of the display would be active.

After that, the intensity register (0x0A) is set to 0x08, which controls the brightness of the LEDs. 
The value can typically range from 0x00 (dimmest) to 0x0F (brightest), so this sets a medium brightness level.
*/

void max7219_init(){
    
    //ustawiamy piny
    gpio_set_direction(CLK_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(CS_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(DIN_PIN, GPIO_MODE_OUTPUT);

    gpio_set_level(CLK_PIN, 0);
    gpio_set_level(CS_PIN, 1);
    gpio_set_level(DIN_PIN, 0);

    max7219_send(0x0F, 0x00);
    max7219_send(0x0C, 0x01);
    max7219_send(0x0B, 0x07);
    max7219_send(0x0A, 0x08);
    max7219_send(0x09, 0x00);
  
    for (int i = 1; i <= 8; i++){
        max7219_send(i, 0x00);
    }
}

//================================================
// DISPLAYING IN APP_MAIN
//================================================

void from_0_to_9(uint32_t delay)
{
    for(int j = 0; j <10; j++) {
        for (int i = 0; i < 8; i++) {
            max7219_send(i + 1, digits[j][i]);
        }
        vTaskDelay(pdMS_TO_TICKS(delay));
    }
}