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
#include "freertos/queue.h"
#include "driver/gpio.h"  //Biblioteka do obsługi GPIO
#include "esp_adc/adc_oneshot.h"


// Standardowe biblioteki C

// Własne include
#include "free_rtos.h"

//================================================
// DEFINITIONS
//================================================

#define LED_1 GPIO_NUM_5
#define LED_2 GPIO_NUM_6


//================================================
// STATIC VARIABLES
//================================================

QueueHandle_t data_queue;

//================================================
// INITIALIZATION FUNCTIONS
//================================================



//================================================
// MAIN FUNCTIONS
//================================================

// Task 1: Blinks LED 1 every 500ms
void blink_task_1(void *pvParameters) {
    gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
    while (1) {
        gpio_set_level(LED_1, 1);
        vTaskDelay(pdMS_TO_TICKS(500)); // Block for 500ms
        gpio_set_level(LED_1, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Task 2: Blinks LED 2 every 1000ms
void blink_task_2(void *pvParameters) {
    gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);
    while (1) {
        gpio_set_level(LED_2, 1);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Block for 500ms
        gpio_set_level(LED_2, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void producer_task(void *pvParameters)
{
    // Uchwyt do jednostki ADC, który będzie używany przez funkcje ESP-IDF do obsługi skonfigurowanego przetwornika ADC.
    adc_oneshot_unit_handle_t adc1_handle;

    // Konfiguracja jednostki ADC. Wybieramy ADC_UNIT_1 jako jednostkę, z której będziemy korzystać.
    adc_oneshot_unit_init_cfg_t init_config1 = {.unit_id = ADC_UNIT_1};

    // Inicjalizacja jednostki ADC na podstawie przygotowanej konfiguracji.
    // Funkcja zapisuje utworzony uchwyt ADC pod adresem adc1_handle.
    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    // Konfiguracja kanału ADC:
    // - ADC_BITWIDTH_DEFAULT - domyślna rozdzielczość pomiaru,
    // - ADC_ATTEN_DB_12 - tłumienie wejścia ADC 12 dB, pozwalające na pomiar wyższego napięcia na wejściu.
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12
    };

    // Konfiguracja kanału ADC_CHANNEL_4 zgodnie z przygotowaną konfiguracją.
    // Od tego momentu kanał 4 jest gotowy do wykonywania pomiarów.
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_4, &config);

    // Zmienna przechowująca aktualną wartość odczytaną z ADC.
    // W tym przypadku jest to wartość z fotorezystora LDR.
    int ldrValue = 0;

    while(1){
        // Wykonanie pojedynczego pomiaru na kanale ADC_CHANNEL_4. Wynik pomiaru zostanie zapisany w zmiennej ldrValue.
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_4, &ldrValue);

        // Umieszczenie odczytanej wartości w kolejce FreeRTOS.
        // &ldrValue przekazuje adres zmiennej, z której funkcja skopiuje wartość do kolejki.
        // Jeżeli kolejka jest pełna, funkcja będzie czekać maksymalnie 100 ms na zwolnienie miejsca.
        xQueueSend(data_queue, &ldrValue, pdMS_TO_TICKS(100));

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void consumer_task(void *pvParameters)
{
    // Zmienna, do której będzie zapisywana wartość odebrana z kolejki.
    int received_data = 0;

    while(1){
        // Oczekiwanie na dane w kolejce.
        // Odebrana wartość zostanie skopiowana do received_data.
        // portMAX_DELAY oznacza, że task będzie czekał bezterminowo,
        // dopóki w kolejce nie pojawią się dane.
        xQueueReceive(data_queue, &received_data, portMAX_DELAY);

        // Sprawdzenie odebranej wartości.
        // Jeżeli wartość ADC jest większa niż 1600,
        // ustawiamy GPIO 7 w stan wysoki.
        if(received_data > 1600){
            gpio_set_level(7, 1);
        }else{
            // Jeżeli wartość jest mniejsza lub równa 1600,
            // ustawiamy GPIO 7 w stan niski.
            gpio_set_level(7, 0);
        }
    }
}

/*
Finally, inside app_main(), we initialize GPIO 7 as an output pin, create the queue, and then launch both tasks.

void app_main(void)
{
    // Przywrócenie GPIO 7 do stanu początkowego przed jego konfiguracją.
    gpio_reset_pin(7);

    gpio_set_direction(7, GPIO_MODE_OUTPUT);

    // Utworzenie kolejki mogącej przechowywać 5 elementów typu int.
    // Producer będzie umieszczał w niej wyniki pomiarów ADC,
    // a consumer będzie je z niej odbierał.
    data_queue = xQueueCreate(5, sizeof(int));

    // Sprawdzenie, czy utworzenie kolejki zakończyło się powodzeniem.
    // NULL oznacza, że kolejka nie została utworzona.
    if(data_queue != NULL){
        // Task będzie wykonywał funkcję producer_task() na rdzeniu CPU 1.
        xTaskCreatePinnedToCore(producer_task, "Producer", 2048, NULL, 1, NULL, 1);

        // Utworzenie taska konsumenta.
        // Task będzie wykonywał funkcję consumer_task() na rdzeniu CPU 1.
        xTaskCreatePinnedToCore(consumer_task, "Consumer", 2048, NULL, 1, NULL, 1);
    }
}
*/

