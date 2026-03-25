
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

volatile int timer_flag_b = 1;
volatile int timer_flag_y = 1;

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

bool timer_callback_b(repeating_timer_t *rt) {
    timer_flag_b = 1;
    return true; 
}

bool timer_callback_y(repeating_timer_t *rt) {
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

    alarm_id_t alarm_id = 0;

    repeating_timer_t start_timer_b;
    repeating_timer_t start_timer_y;

    int led_state_b = 0;
    int led_state_y = 0;

    int alarme_ativado = 0;

    while (true) {

        if (flag_btn) {
            flag_btn = 0;
            alarm_id = add_alarm_in_ms(5000, alarm_callback, NULL, false);
            add_repeating_timer_ms(500, timer_callback_y, NULL, &start_timer_y);
            add_repeating_timer_ms(150, timer_callback_b, NULL, &start_timer_b);
            alarme_ativado = 1;
        }

        if (alarme_ativado && timer_flag_b) {
            timer_flag_b = 0;
            led_state_b = !led_state_b;
            gpio_put(LED_PIN_B, led_state_b);
        }

        if (alarme_ativado && timer_flag_y) {
            timer_flag_y = 0;
            led_state_y = !led_state_y;
            gpio_put(LED_PIN_Y, led_state_y);
        }

        if (alarm_flag) {
            alarm_flag = 0;
            cancel_alarm(alarm_id);
            cancel_repeating_timer(&start_timer_b);
            cancel_repeating_timer(&start_timer_y);
            gpio_put(LED_PIN_B, 0);
            gpio_put(LED_PIN_Y, 0);
            alarme_ativado = 0;
        }
    }
}
