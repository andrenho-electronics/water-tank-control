#include "pico/stdlib.h"
#include "pico/time.h"

#include <stdio.h>

#define TIME_TO_EMERGENCY (10 * 60 * 1000) // 10 min

// pins

#define LED_LEVEL_TOP     0
#define LED_LEVEL_1       1
#define LED_LEVEL_2       2
#define LED_LEVEL_BOTTOM  3
#define MOTOR_ON         14
#define EMERGENCY        15
#define MOTOR            28
#define SENSOR_TOP       21
#define SENSOR_1         20
#define SENSOR_2         19
#define SENSOR_BOTTOM    18

// types

typedef enum { MotorOff, MotorOn, Emergency } State;
typedef enum { TOP, S1, S2, BOTTOM } Sensor;

// globals

static absolute_time_t timer;
static State           state = MotorOff;

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

static void init_pins()
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
    gpio_init(EMERGENCY);
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
    gpio_pull_up(EMERGENCY);
    gpio_pull_up(SENSOR_TOP);
    gpio_pull_up(SENSOR_1);
    gpio_pull_up(SENSOR_2);
    gpio_pull_up(SENSOR_BOTTOM);

}

int main()
{
    init_pins();

    timer = get_absolute_time();

    while (true) {

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
        
        // manage states
        State new_state = state;

        if (!gpio_get(EMERGENCY))
            new_state = Emergency;

        switch (state) {
            case MotorOff:
                gpio_put(MOTOR, 0);
                if ((!sensors[BOTTOM] && !sensors[TOP]) || !gpio_get(MOTOR_ON))
                    new_state = MotorOn;
                break;
            case MotorOn:
                gpio_put(MOTOR, 1);
                if (sensors[TOP])
                    new_state = MotorOff;
                if (get_absolute_time() > delayed_by_ms(timer, TIME_TO_EMERGENCY))
                    new_state = Emergency;
                break;
            case Emergency:
                emergency();
        }

        if (state == MotorOff && new_state == MotorOn)
            timer = get_absolute_time();

        state = new_state;
    }
}
