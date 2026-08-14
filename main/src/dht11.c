#include "dht11.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_timer.h"
#include "esp_rom_sys.h"

#include <stdio.h>

#define DHT11_PIN GPIO_NUM_4

static void dht11_set_output(void)
{
    gpio_set_direction(DHT11_PIN,GPIO_MODE_OUTPUT);
}

static void dht11_set_input(void)
{
    gpio_set_direction(DHT11_PIN,GPIO_MODE_INPUT);
}

void dht11_init(void)
{
    gpio_config_t config =
    {
        .pin_bit_mask = (1ULL << DHT11_PIN),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&config);
}

static int wait_pin_level(int level)
{
    int timeout = 10000;

    while(gpio_get_level(DHT11_PIN) != level){
        timeout--;

        if(timeout <= 0){
            return -1;
        }

        esp_rom_delay_us(1);
    }

    return 0;
}

static int read_bit(void)
{
    int64_t start;

    if(wait_pin_level(1) != 0){
        return -1;
    }

    start = esp_timer_get_time();

    if(wait_pin_level(0) != 0){
        return -1;
    }

    if((esp_timer_get_time() - start) > 40){
        return 1;
    }

    return 0;
}

static int read_byte(void)
{
    uint8_t data = 0;

    for(int i = 0; i < 8; i++){
        int bit = read_bit();

        if(bit < 0){
            return -1;
        }

        data <<= 1;
        data |= bit;
    }

    return data;
}

int dht11_read(uint8_t *humidity,uint8_t *temperature)
{
    uint8_t data[5];

    dht11_set_output();

    gpio_set_level(DHT11_PIN,0);

    vTaskDelay(pdMS_TO_TICKS(18));

    gpio_set_level(DHT11_PIN,1);

    esp_rom_delay_us(40);

    dht11_set_input();

    if(wait_pin_level(0) != 0){
        return DHT11_ERROR_TIMEOUT;
    }

    if(wait_pin_level(1) != 0){
        return DHT11_ERROR_TIMEOUT;
    }

    if(wait_pin_level(0) != 0){
        return DHT11_ERROR_TIMEOUT;
    }

    for(int i = 0; i < 5; i++){
        data[i] = read_byte();

        if(data[i] == 255){
            return DHT11_ERROR_TIMEOUT;
        }
    }

    if(data[4] != (data[0] + data[1] + data[2] + data[3])){
        return DHT11_ERROR_CHECKSUM;
    }

    *humidity = data[0];

    *temperature = data[2];

    return DHT11_OK;
}