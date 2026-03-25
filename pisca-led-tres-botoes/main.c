#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

const int BTN_PIN_G = 18;
const int BTN_PIN_Y = 28;
const int BTN_PIN_B = 22;

const int LED_PIN_G = 14;
const int LED_PIN_Y = 6;
const int LED_PIN_B = 10;

volatile int flag_btn_b = 0;
volatile int flag_btn_y = 0;

volatile int flag_fall_btn_g = 0;
volatile int flag_rise_btn_g = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {  // fall edge
        if (gpio == BTN_PIN_B) {
            flag_btn_b = !flag_btn_b;
        } else if (gpio == BTN_PIN_Y) {
            flag_btn_y = !flag_btn_y;
        } else if (gpio == BTN_PIN_G) {
            flag_fall_btn_g = 1;
        }
    } else if (events == 0x8) {  // rise edge
        if (gpio == BTN_PIN_G) {
            flag_rise_btn_g = 1;
        }
    }
}

void setup() {
    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);
    gpio_set_irq_enabled_with_callback(BTN_PIN_B, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G,
                                       GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
                                       true,
                                       &btn_callback);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
}

int main() {
  stdio_init_all();
  setup();

  int led_state = 0;
 
  while (true) {

    led_state = !led_state;

    if (flag_btn_b) {
        gpio_put(LED_PIN_B, led_state);
    }

    if (flag_btn_y) {
        gpio_put(LED_PIN_Y, led_state);
    }

    if (flag_fall_btn_g && !flag_rise_btn_g) {
        gpio_put(LED_PIN_G, led_state);
    }

    if (flag_rise_btn_g) {
        flag_fall_btn_g = 0;
        flag_rise_btn_g = 0;
        gpio_put(LED_PIN_G, 0);
    }

    sleep_ms(200); 

  }
}