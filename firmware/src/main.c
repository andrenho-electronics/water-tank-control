#include "pico/stdlib.h"
#include "pico/time.h"

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

#define TIME_TO_EMERGENCY (10 * 1000)

static uint32_t timer_counter = 0;
static bool     timer_on = false;

static void emergency()
{
    gpio_put(MOTOR, 0);
    bool value = true;
    for (;;) {
        for (int i = 0; i < 4; ++i)
            gpio_put(i, value);
        sleep_ms(500);
        value = !value;
    }
}

static void reset_timer(bool timer_on_)
{
    timer_counter = to_ms_since_boot(get_absolute_time());
    timer_on = timer_on_;
}

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

    typedef enum { TOP, S1, S2, BOTTOM } Sensor;
    bool desired_motor_state = false;

    reset_timer(false);

    while (true) {

        // check for emergency
        if (!gpio_get(MOTOR_OFF))
            emergency();
        if (timer_on && (timer_counter + TIME_TO_EMERGENCY > to_ms_since_boot(get_absolute_time())))
            emergency();

        // read sensors
        bool sensors[] = {
            !gpio_get(SENSOR_TOP),
            !gpio_get(SENSOR_1),
            !gpio_get(SENSOR_2),
            !gpio_get(SENSOR_BOTTOM),
        };

        // turn on LEDs according to the sensors
        gpio_put(LED_LEVEL_TOP, sensors[TOP]);
        gpio_put(LED_LEVEL_1, sensors[S1]);
        gpio_put(LED_LEVEL_2, sensors[S2]);
        gpio_put(LED_LEVEL_BOTTOM, sensors[BOTTOM]);
        
        // manage motor
        if (!sensors[BOTTOM] || !gpio_get(MOTOR_ON))
            desired_motor_state = true;
        if (sensors[TOP])
            desired_motor_state = false;

        gpio_put(MOTOR, desired_motor_state);

        // emergency timer
        if (desired_motor_state && !timer_on)
            reset_timer(true);
        if (!desired_motor_state)
            timer_on = false;
    }
}
