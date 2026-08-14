//Dodać ten plik do CMakeList.txt wraz ze ścieżką z main jako folderem roboczym

//================================================
// READ ME
//================================================

/*
Unlike older microcontrollers that restrict PWM to specific hardware pins,
the ESP32-S3 allows us to route PWM signals to all pins that can act as outputs using the LED Control (LEDC) peripheral.

To work with PWM in ESP-IDF, we use the LEDC (LED Controller) peripheral.
This hardware module generates PWM signals efficiently without heavy CPU usage.
The configuration is done in two main steps: first, we set up a timer that defines the PWM signal’s frequency and resolution,
and then we configure a channel that connects this timer to a specific GPIO pin.
Once configured, we can control the signal by adjusting the duty cycle, which determines how long the signal stays HIGH
during each PWM period.
*/

//================================================
// INCLUDES
//================================================

// Standardowe biblioteki ESP-IDF
#include "freertos/FreeRTOS.h"     // FreeRTOS
#include "freertos/task.h"
#include "driver/ledc.h"           // Obsługa PWM (LEDC)
#include "driver/gpio.h"           // Definicje GPIO

// Własne include
#include "PWM.h"

//================================================
// DEFINITIONS
//================================================

// Piny wykorzystywane przez kanały PWM
#define PWM_PIN_1 GPIO_NUM_2
#define PWM_PIN_2 GPIO_NUM_3

// Maksymalna wartość duty dla rozdzielczości 12-bit (2^12 - 1)
#define PWM_MAX_DUTY 4095

// Liczba skonfigurowanych urządzeń PWM
#define PWM_COUNT (sizeof(pwm_devices) / sizeof(pwm_devices[0]))

//================================================
// STATIC VARIABLES
//================================================

/*
Struktura przechowująca informacje o jednym wyjściu PWM.
Łączy numer pinu GPIO z odpowiadającym mu kanałem LEDC.
*/
typedef struct
{
    gpio_num_t gpio;
    ledc_channel_t channel;
} pwm_device_t;

/*
Tabela wszystkich wyjść PWM używanych w projekcie.

Aby dodać nowe wyjście PWM, wystarczy dopisać kolejny wpis.
*/
static const pwm_device_t pwm_devices[] =
{
    {PWM_PIN_1, LEDC_CHANNEL_0},
    {PWM_PIN_2, LEDC_CHANNEL_1},
};

//================================================
// INITIALIZATION FUNCTIONS
//================================================

/*
Inicjalizacja modułu PWM.
Konfiguruje:
1. Wspólny timer PWM.
2. Wszystkie kanały PWM znajdujące się w tablicy pwm_devices.
*/
void pwm_init(void)
{
    // Konfiguracja wspólnego timera PWM
    ledc_timer_config_t ledc_timer =
    {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .timer_num       = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz         = 5000,
        .clk_cfg         = LEDC_AUTO_CLK
    };

    ledc_timer_config(&ledc_timer);

    // Konfiguracja wszystkich kanałów PWM
    for (int i = 0; i < PWM_COUNT; i++)
    {
        ledc_channel_config_t channel =
        {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel    = pwm_devices[i].channel,
            .timer_sel  = LEDC_TIMER_0,
            .gpio_num   = pwm_devices[i].gpio,
            .duty       = 0,      // Początkowo sygnał wyłączony
            .hpoint     = 0
        };

        ledc_channel_config(&channel);
    }
}

//================================================
// MAIN FUNCTIONS
//================================================

/*
Ustawia współczynnik wypełnienia (Duty Cycle) dla wybranego pinu GPIO.

Parametry:
pin  - numer pinu GPIO skonfigurowanego jako PWM.
duty - wartość od 0 do PWM_MAX_DUTY.

Funkcja wyszukuje odpowiedni kanał LEDC przypisany do danego pinu
i aktualizuje jego wypełnienie.
*/
void pwm_set(gpio_num_t pin, uint32_t duty)
{
    // Zabezpieczenie przed przekroczeniem maksymalnej wartości
    if (duty > PWM_MAX_DUTY)
    {
        duty = PWM_MAX_DUTY;
    }

    // Wyszukanie kanału odpowiadającego danemu pinowi
    for (int i = 0; i < PWM_COUNT; i++)
    {
        if (pwm_devices[i].gpio == pin)
        {
            ledc_set_duty(
                LEDC_LOW_SPEED_MODE,
                pwm_devices[i].channel,
                duty);

            ledc_update_duty(
                LEDC_LOW_SPEED_MODE,
                pwm_devices[i].channel);

            return;
        }
    }
}
