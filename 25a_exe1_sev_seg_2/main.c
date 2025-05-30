#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"


const int segment_pins[7] = {2, 3, 4, 5, 6, 7, 8};
const int BTN_INC = 28;
const int BTN_DEC = 27;
const int BTN_RST = 26;

// Flag
volatile int event = 0;


// Tabela de segmentos para 0-9, E, F
const uint8_t segment_map[12][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}, // 9
    {1, 0, 0, 1, 1, 1, 1}, // E
    {1, 0, 0, 0, 1, 1, 1}  // F
};

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_INC) {
        event = 1;
    } else if (gpio == BTN_DEC) {
        event = 2;
    } else if (gpio == BTN_RST) {
        event = 3;
    }
}

void hardware_init() {
    gpio_init(BTN_INC);
    gpio_set_dir(BTN_INC, GPIO_IN);
    gpio_pull_up(BTN_INC);
    gpio_set_irq_enabled_with_callback(BTN_INC, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(BTN_DEC);
    gpio_set_dir(BTN_DEC, GPIO_IN);
    gpio_pull_up(BTN_DEC);
    gpio_set_irq_enabled(BTN_DEC, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(BTN_RST);
    gpio_set_dir(BTN_RST, GPIO_IN);
    gpio_pull_up(BTN_RST);
    gpio_set_irq_enabled(BTN_RST, GPIO_IRQ_EDGE_FALL, true);

    for (int i = 0; i < 7; i++) {
        gpio_init(segment_pins[i]);
        gpio_set_dir(segment_pins[i], GPIO_OUT);
    }
}

void display(int val) {
    int index;
    if (val >= 0 && val <= 9) index = val;
    else if (val == -2) index = 10; // E
    else index = 11; // F

    for (int i = 0; i < 7; i++) {
        gpio_put(segment_pins[i], segment_map[index][i]);
    }
}

int main() {
    stdio_init_all();
    hardware_init();


    int valor_display = 0;
    display(valor_display);

    while (true) {
        if (event==1) {
            event = 0;
            if (valor_display == -2) {
                valor_display = 1;
            } else if (valor_display >= 0 && valor_display < 9) {
                valor_display++;
            } else if (valor_display == 9) {
                valor_display = -1;
            }
            display(valor_display);
        }

        if (event==2) {
            event = 0;
            if (valor_display == -1) {
                valor_display = 9;
            } else if (valor_display > 0 && valor_display <= 9) {
                valor_display--; 
            } else if (valor_display == 0) {
                valor_display = -2; 
            }
            display(valor_display);
        }

        if (event==3) {
            event = 0;
            valor_display = 0;
            display(valor_display);
        }
    }
}
