#include <Arduino.h>

class SM {

    public:
        SM(int PIN_STEP, int PIN_SLEEP, int PWM_CHANNEL, String NAME);

        void power(String STATE);
        void rotate(float FREQUENCY);

        float frequency = 0;
        String state = "off";
        struct {

            float min = 0.5;
            float step = 0.1; 
            float max = 3;

        } range;

        struct {
            String frequency, state;
        } parameters;

    private:
        int pin_direction, pin_step, pin_sleep, pin_reset;
        int pwm_channel = 0;
        int resolution = 8;
        int dutyCycle = pow(2, resolution) - 1;
        float step_angle = 1.8;

};