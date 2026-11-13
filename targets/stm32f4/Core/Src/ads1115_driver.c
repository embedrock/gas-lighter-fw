#include "ads1115_driver.h"
#include "stm32f4xx_hal.h"

#define ADS1115_ADDR  (0x48 << 1)
#define ADS1115_REG_CONVERSION  0x00
#define ADS1115_REG_CONFIG      0x01

extern I2C_HandleTypeDef hi2c1;

// Базовые настройки (gain, data rate), без привязки к каналу
static uint16_t base_config = 0;

// Инициализация: gain и data rate задаём один раз
void ads1115_init(uint8_t gain, uint8_t data_rate) {
    // MODE = 1 (single-shot), остальные биты собираем при чтении канала
    base_config = (1 << 8) |            // single-shot
                  (gain << 9) |         // PGA
                  (data_rate << 5);     // DR
}

// Чтение конкретного канала (0-3)
int16_t ads1115_read_channel(uint8_t channel) {
    uint16_t config = base_config;
    
    // OS bit = 1 (старт конвертации)
    config |= (1 << 15);
    
    // MUX: 0x04 + channel (AIN0-GND, AIN1-GND, ...)
    config |= ((0x04 + channel) << 12);
    
    // Запускаем конвертацию
    HAL_I2C_Mem_Write(&hi2c1, ADS1115_ADDR, ADS1115_REG_CONFIG,
                      I2C_MEMADD_SIZE_8BIT, (uint8_t*)&config, 2, 100);
    
    // Ждём завершения. При 128 SPS нужно ~8 мс, берём с запасом
    // Можно заменить на опрос бита OS или прерывание ALERT
    HAL_Delay(10);
    
    // Читаем результат
    uint16_t raw = 0;
    HAL_I2C_Mem_Read(&hi2c1, ADS1115_ADDR, ADS1115_REG_CONVERSION,
                     I2C_MEMADD_SIZE_8BIT, (uint8_t*)&raw, 2, 100);
    
    return (int16_t)raw;
}