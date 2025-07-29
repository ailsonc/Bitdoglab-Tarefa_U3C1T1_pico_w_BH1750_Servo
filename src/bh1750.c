#include "bh1750.h"

i2c_inst_t* bh1750_init(i2c_inst_t *i2c_instance) {
    printf("[BH1750] Configurando BH1750 no I2C...\n");

    i2c_init(i2c_instance, 400 * 1000);
    gpio_set_function(I2C0_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL_PIN, GPIO_FUNC_I2C);
    
    gpio_pull_up(I2C0_SDA_PIN);
    gpio_pull_up(I2C0_SCL_PIN);
    
    bh1750_set_mode(i2c_instance, BH1750_POWER_ON);
    sleep_ms(10); 
    
    bh1750_set_mode(i2c_instance, BH1750_CONT_HIGH_RES_MODE);
    sleep_ms(180);

    printf("[BH1750] BH1750 configurado e pronto para leitura.\n");

    return i2c_instance;
}


void bh1750_set_mode(i2c_inst_t *i2c, uint8_t mode) {
    i2c_write_blocking(i2c, BH1750_ADDR, &mode, 1, false);
}

void bh1750_read_lux(i2c_inst_t *i2c, float *lux) {
    uint8_t buffer[2];

    i2c_read_blocking(i2c, BH1750_ADDR, buffer, 2, false);
    uint16_t raw = (buffer[0] << 8) | buffer[1];
    
    *lux = raw / 1.2f;
}