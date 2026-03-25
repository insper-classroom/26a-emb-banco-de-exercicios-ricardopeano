/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

const int BTN_PIN = 28;

const int LED_PIN = 5;

volatile int flag_fall = 0;
volatile int flag_rise = 0;
volatile int alarm_flag = 0;
volatile int timer_flag = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {  // fall edge
        flag_fall = 1;
    } else if (events == 0x8) {  // rise edge
        flag_rise = 1;
    }
}


int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarm_flag = 1;
    return 0;
}

bool timer_callback(struct repeating_timer_t *rt) {
    timer_flag = 1;
    return true; 
}

void setup() {
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN,
                                       GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
                                       true,
                                       &btn_callback);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

int main() {
    stdio_init_all();
    setup();

    absolute_time_t tempo_i;
    absolute_time_t tempo_f;
    
    repeating_timer_t start_timer;

    int led_state = 0;

    while (true) {

        if (flag_fall) {
            flag_fall = 0;
            gpio_put(LED_PIN, 0);
            tempo_i = get_absolute_time();
        }

        if (flag_rise) {
            flag_rise = 0;
            tempo_f = get_absolute_time();
            int64_t tempo_decorrido = absolute_time_diff_us(tempo_i, tempo_f);
            add_repeating_timer_ms(tempo_decorrido / 1000, timer_callback, NULL, &start_timer);
        }

        if (timer_flag) {
            timer_flag = 0;
            led_state = !led_state;
            gpio_put(LED_PIN, led_state);
        }

    }

    return 0;
}