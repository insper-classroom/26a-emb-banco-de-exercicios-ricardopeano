
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"

const int BTN_PIN = 28;

const int LED_PIN_B = 9;
const int LED_PIN_Y = 5;

volatile int flag_btn = 0;

volatile int alarm_flag = 0;

volatile int timer_flag_b = 0;
volatile int timer_flag_y = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {  // fall edge
        if (gpio == BTN_PIN) {
            flag_btn = 1;
        }
    }
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarm_flag = 1;
    return 0;
}

bool timer_callback_b(struct repeating_timer_t *rt) {
    timer_flag_b = 1;
    return true; 
}

bool timer_callback_y(struct repeating_timer_t *rt) {
    timer_flag_y = 1;
    return true; 
}

void setup() {
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
}

int main() {
    stdio_init_all();
    setup();

    while (true) {
    

    }
}
