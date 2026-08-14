#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

#define DHT11_OK 0
#define DHT11_ERROR_CHECKSUM -1
#define DHT11_ERROR_TIMEOUT -2

void dht11_init(void);
int dht11_read(uint8_t *humidity,uint8_t *temperature);

#endif