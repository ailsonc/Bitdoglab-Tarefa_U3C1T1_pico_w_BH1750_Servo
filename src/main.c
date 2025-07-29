#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "display.h"
#include "bh1750.h"

#define I2C_PORT i2c0
#define SERVO_PIN 2

// Ângulos para o servo motor
#define ANGLE_LIGHT_DETECTED 90.0f  // Posição quando há luz
#define ANGLE_NO_LIGHT       0.0f   // Posição quando não há luz

uint16_t angle_to_duty(float angle) {
    float pulse_ms = 0.5f + (angle / 180.0f) * 2.0f;
    return (uint16_t)((pulse_ms / 20.0f) * 20000.0f);
}

void servo_init(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 20000);
    pwm_init(slice, &config, true);
}

void servo_set_angle(uint pin, float angle) {
    pwm_set_gpio_level(pin, angle_to_duty(angle));
}

int main() {
    stdio_init_all();
    i2c_inst_t *i2c = bh1750_init(I2C_PORT);
    display_init(); 
    servo_init(SERVO_PIN);

    float lux;
    char buffer[32];
    bool servo_is_active = false; // Estado para controlar a posição do servo

    // Garante que o servo comece na posição "sem luz"
    servo_set_angle(SERVO_PIN, ANGLE_NO_LIGHT);
    
    while (true) {
        bh1750_read_lux(i2c, &lux);
        sprintf(buffer, "Lum: %.2f lux", lux);
        display_status_msg(buffer);

        // Se a luz estiver abaixo do limiar E o servo não estiver na posição "ativo"
        if (lux < 10.0f && !servo_is_active) {
            servo_set_angle(SERVO_PIN, ANGLE_LIGHT_DETECTED); // Move o servo para a posição "ativo"
            servo_is_active = true; // Marca o servo como ativo
            printf("Luz baixa detectada! Servo movido para %.1f graus.\n", ANGLE_LIGHT_DETECTED);
        } 
        // Se a luz estiver acima ou igual ao limiar E o servo estiver na posição "ativo"
        else if (lux >= 10.0f && servo_is_active) {
            servo_set_angle(SERVO_PIN, ANGLE_NO_LIGHT); // Move o servo de volta para a posição "inativo"
            servo_is_active = false; // Marca o servo como inativo
            printf("Luz suficiente detectada! Servo movido para %.1f graus.\n", ANGLE_NO_LIGHT);
        }

        sleep_ms(500);
    }

    return 0; 
}