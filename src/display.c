#include "display.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include <string.h>
#include <stdio.h>

static struct render_area frame_area = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1
};

// Inicialização do I2C e do display SSD1306
void display_init(void) {
    i2c_init(i2c1, SSD1306_I2C_CLOCK * 1000);
    gpio_set_function(SSD1306_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_I2C_SDA);
    gpio_pull_up(SSD1306_I2C_SCL);

    ssd1306_init();

    calculate_render_area_buffer_length(&frame_area);
}

void display_status_msg(char status_msg[]) {
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    char status_text[128];
    snprintf(status_text, sizeof(status_text), "%s", status_msg);
    ssd1306_draw_string(ssd, 5, 0, status_text);
    render_on_display(ssd, &frame_area);
}
