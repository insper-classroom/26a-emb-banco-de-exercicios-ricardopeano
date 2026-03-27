#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int BTN_PIN_G = 28;
const int BTN_PIN_Y = 26;
const int BTN_PIN_R = 20;

const int LED_PIN_G = 5;
const int LED_PIN_Y = 9;
const int LED_PIN_R = 13;

volatile int flag_btn_g = 0;
volatile int flag_btn_y = 0;
volatile int flag_btn_r = 0;

volatile int alarm_flag = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {  // fall edge
        if (gpio == BTN_PIN_Y) {
            flag_btn_y = 1;
        } else if (gpio == BTN_PIN_G) {
            flag_btn_g = 1;
        } else if (gpio == BTN_PIN_R) {
            flag_btn_r = 1;
        } 
    } 
}    

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarm_flag = 1;  
    return 0;     
}

void setup() {
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
}

int main() {
    stdio_init_all();
    setup();

    int seq[10] = {LED_PIN_Y, LED_PIN_G, LED_PIN_R, LED_PIN_Y, LED_PIN_G, LED_PIN_Y, LED_PIN_R, LED_PIN_Y, LED_PIN_G, LED_PIN_Y};
    int level = 1;
    int contador = 0;

    int state = 0; // 0: esperando verde, 1: exibindo sequência, 2: aguardando input, 3: game over, 4: setar alarme

    alarm_id_t alarm_id = 0;

    while (1) {

        if (flag_btn_g && state == 0) {
            flag_btn_g = 0;
            state = 1;
            }
        
        if (state==1) {
            for (int i = 0; i<level; i++) {
                gpio_put(seq[i], 1);
                sleep_ms(300);
                gpio_put(seq[i], 0);
            }
            state = 4;
        }

        if (state == 4) {
            cancel_alarm(alarm_id);
            alarm_id = add_alarm_in_ms(3000, alarm_callback, NULL, true);
            if (contador==level) {
                cancel_alarm(alarm_id);
                level++;
                state = 1;
                contador = 0;
            } 
            if (state != 1) {
                state = 2;
            } 
        }

        if (state == 2) {
            if (alarm_flag) {
                state = 3;
            } 
            if (flag_btn_g) {
                flag_btn_g = 0;
                if (seq[contador] != LED_PIN_G) {
                    state = 3;
                }
                contador++;
                state = 4;
            } else if (flag_btn_r) {
                flag_btn_r = 0;
                if (seq[contador] != LED_PIN_R) {
                    state = 3;
                }
                contador++;
                state = 4;
            } else if (flag_btn_y) {
                flag_btn_y = 0;
                if (seq[contador] != LED_PIN_Y) {
                    state = 3;
                }
                contador++;
                if (state != 3) {
                    state = 4;
                }
            }
        }

        if (state == 3) {
            printf("Points: %d", level);
            gpio_put(LED_PIN_G, 0);
            gpio_put(LED_PIN_Y, 0);
            gpio_put(LED_PIN_R, 1);
            sleep_ms(300);
            gpio_put(LED_PIN_R, 0);
            cancel_alarm(alarm_id);
            contador = 0;
            level = 1;
            state = 0;
        }
        
    }
}