#include "pico/stdlib.h"

#include <stdio.h>

#define LED_LEVEL_TOP     0
#define LED_LEVEL_1       1
#define LED_LEVEL_2       2
#define LED_LEVEL_BOTTOM  3
#define MOTOR_ON         14
#define MOTOR_OFF        15
#define MOTOR            28
#define SENSOR_TOP       21
#define SENSOR_1         20
#define SENSOR_2         19
#define SENSOR_BOTTOM    18

int main()
{
    // turn on Pico LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    
    // setup pins
    gpio_init(LED_LEVEL_TOP);
    gpio_init(LED_LEVEL_1);
    gpio_init(LED_LEVEL_2);
    gpio_init(LED_LEVEL_BOTTOM);
    gpio_init(MOTOR_ON);
    gpio_init(MOTOR_OFF);
    gpio_init(MOTOR);
    gpio_init(SENSOR_TOP);
    gpio_init(SENSOR_1);
    gpio_init(SENSOR_2);
    gpio_init(SENSOR_BOTTOM);

    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_set_dir(LED_LEVEL_TOP, GPIO_OUT);
    gpio_set_dir(LED_LEVEL_1, GPIO_OUT);
    gpio_set_dir(LED_LEVEL_2, GPIO_OUT);
    gpio_set_dir(LED_LEVEL_BOTTOM, GPIO_OUT);
    gpio_set_dir(MOTOR, GPIO_OUT);

    gpio_pull_up(MOTOR_ON);
    gpio_pull_up(MOTOR_OFF);
    gpio_pull_up(SENSOR_TOP);
    gpio_pull_up(SENSOR_1);
    gpio_pull_up(SENSOR_2);
    gpio_pull_up(SENSOR_BOTTOM);

    while (true) {
        // read sensors
        bool sensors[] = {
            !gpio_get(SENSOR_TOP),
            !gpio_get(SENSOR_1),
            !gpio_get(SENSOR_2),
            !gpio_get(SENSOR_BOTTOM),
        };

        // turn on LEDs according to the sensors
        gpio_put(LED_LEVEL_TOP, sensors[0]);
        gpio_put(LED_LEVEL_1, sensors[1]);
        gpio_put(LED_LEVEL_2, sensors[2]);
        gpio_put(LED_LEVEL_BOTTOM, sensors[3]);
    }
}
