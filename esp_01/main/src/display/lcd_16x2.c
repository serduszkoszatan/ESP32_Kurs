//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

/*
To communicate with the LCD directly, we have two possible modes:
8-bit mode and 4-bit mode.
In 8-bit mode, we uses all eight data pins (D0–D7) of the LCD to send one full byte of data at a time. 
This method is faster because the entire character is transmitted in a single operation. 
However, this mode requires many GPIO pins:
8 pins for data, 2 control pins (RS, Enable)

In 4-bit mode, the LCD receives data in two steps instead of one. First, we sends the higher 4 bits,
then we sends the lower 4 bits of the same byte. The LCD internally combines these two parts to reconstruct the full 8-bit value.
This approach reduces the number of required pins:
4 data pins (D4–D7), 2 control pins (RS and Enable)
*/

//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"  //Podstawowe biblioteki 
#include "freertos/task.h"      //Podstawowe biblioteki
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO

// Standardowe biblioteki C

// Własne include
#include "lcd_16x2.h"

//================================================
// DEFINITIONS
//================================================

#define ENABLE_PIN  GPIO_NUM_1    
#define RS_PIN      GPIO_NUM_6            //Register Select
#define D4_PIN      GPIO_NUM_15
#define D5_PIN      GPIO_NUM_16
#define D6_PIN      GPIO_NUM_17
#define D7_PIN      GPIO_NUM_18

//================================================
// STATIC VARIABLES
//================================================



//================================================
// INITIALIZATION FUNCTIONS
//================================================

// Generuje impuls na pinie ENABLE, aby zatwierdzić dane wysłane do LCD.
// ENABLE zostaje ustawione w stan wysoki na 1 us, a następnie wraca do stanu niskiego.
// Po zakończeniu impulsu odczekujemy 20 us, aby LCD miał czas na przetworzenie danych.
void lcd_pulse_enable (void)
{
    gpio_set_level(ENABLE_PIN, 1);
    ets_delay_us(1);
    gpio_set_level(ENABLE_PIN, 0);
    ets_delay_us(50);
}

// Wysyła 4 bity danych do LCD w trybie 4-bitowym.
// Każdy bit zmienia odpowiedni pin danych D4-D7.
// Po ustawieniu wszystkich pinów generowany jest impuls ENABLE,
// który informuje LCD o konieczności odczytania przesłanych danych.
void lcd_send_4_bits_mode(uint8_t bits)
{
    // Pobieramy bit 0 (LSB) i ustawiamy nim pin D4.
    gpio_set_level(D4_PIN, bits & 0b0001);
    // Pobieramy bit 1 i ustawiamy nim pin D5.
    gpio_set_level(D5_PIN, bits & 0b0010);
    // Pobieramy bit 2 i ustawiamy nim pin D6.
    gpio_set_level(D6_PIN, bits & 0b0100);
    // Pobieramy bit 3 i ustawiamy nim pin D7.
    gpio_set_level(D7_PIN, bits & 0b1000);
    // Generujemy impuls ENABLE, aby LCD odczytał ustawione bity.
    lcd_pulse_enable();
}

// Wysyła 8-bitową komendę do LCD w trybie 4-bitowym.
// Najpierw wysyłane są 4 starsze bity (starsza połówka bajtu),
// a następnie 4 młodsze bity (młodsza połówka bajtu).
void send_command_4_bits_mode(uint8_t command){
    // Przesuwamy 4 starsze bity na pozycję 4 młodszych bitów
    // i wysyłamy je do LCD.
	lcd_send_4_bits_mode(command >> 4);
    // Wysyłamy 4 młodsze bity komendy.
	lcd_send_4_bits_mode(command );
}


void lcd_gpio_init_4_bits_mode(void){
    //Setup GPIOs
    gpio_set_direction(D4_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(D5_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(D6_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(D7_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(RS_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ENABLE_PIN, GPIO_MODE_OUTPUT);

    // Initial State
    gpio_set_level(RS_PIN, 0);
    gpio_set_level(ENABLE_PIN, 0);
}


void initialize_lcd(void)
{
    // Inicjalizacja pinów GPIO używanych przez LCD.
    lcd_gpio_init_4_bits_mode();
    // Czekamy 50 ms, aby LCD zakończył uruchamianie po włączeniu zasilania.
    vTaskDelay(pdMS_TO_TICKS(50));
    // Ustawiamy D4 i D5 w stan wysoki (0011),
    // przygotowując LCD do przejścia w tryb 4-bitowy.
    lcd_send_4_bits_mode(0x03);
    // Czekamy 5 ms przed wysłaniem kolejnego impulsu ENABLE.
    vTaskDelay(pdMS_TO_TICKS(5));
    // Generujemy drugi impuls ENABLE.
    lcd_pulse_enable();
    // Czekamy 100 us przed kolejnym impulsem.
    ets_delay_us(100);
    // Generujemy trzeci impuls ENABLE.
    lcd_pulse_enable();
    // Czekamy 10 us przed przejściem do kolejnego etapu inicjalizacji.
    ets_delay_us(10);
    // Ustawiamy LCD w tryb 4-bitowy.
    lcd_send_4_bits_mode(0x02);
    // Function Set:
    // Ustawiamy długość danych na 4 bity oraz wyświetlacz 2-liniowy.
    // DL = 0 -> tryb 4-bitowy
    // N  = 1 -> wyświetlacz 2-liniowy
    send_command_4_bits_mode(0x28);
    // Włączamy wyświetlacz oraz wyłączamy kursor i jego miganie.
    send_command_4_bits_mode(0x0C);
    // Czyścimy wyświetlacz i ustawiamy kursor na pozycji początkowej (0,0).
    send_command_4_bits_mode(0x01);
    // Czekamy 2 ms na wykonanie komendy czyszczenia wyświetlacza.
    vTaskDelay(pdMS_TO_TICKS(2));
}

//================================================
// MAIN FUNCTIONS
//================================================

// Czyści cały wyświetlacz LCD i ustawia kursor na pozycji początkowej (0,0).
// Po wysłaniu komendy czekamy 2 ms, aby LCD miał czas na jej wykonanie.
void clear_display(void){
	send_command_4_bits_mode(0x01);
    vTaskDelay(pdMS_TO_TICKS(2));
}

// Ustawia kursor LCD w określonym wierszu i kolumnie.
// Numerowanie wierszy i kolumn rozpoczyna się od 1.
void move_to(uint8_t line, uint8_t column)
{
    if (line == 1)
    {
        // Pierwszy wiersz rozpoczyna się od adresu 0x00.
        // Odejmujemy 1 od kolumny, ponieważ pierwsza kolumna ma adres 0.
        send_command_4_bits_mode(0x80 | (column - 1));
    }
    else
    {
        // Drugi wiersz rozpoczyna się od adresu 0x40.
        // Odejmujemy 1 od kolumny, ponieważ pierwsza kolumna ma adres 0.
        send_command_4_bits_mode(0xC0 | (column - 1));
    }
}

void write_character(char character){
  gpio_set_level(RS_PIN, 1);
  send_command_4_bits_mode(character);
  gpio_set_level(RS_PIN, 0);
}

void write_word(char message[]){
  for (int i=0;message[i]!='\0';i++){
    write_character(message[i]);
  }
}