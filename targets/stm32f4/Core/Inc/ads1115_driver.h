#pragma once

#include "main.h"

void ads1115_init(uint8_t gain, uint8_t data_rate);
int16_t ads1115_read_channel(uint8_t channel);