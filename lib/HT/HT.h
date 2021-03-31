#include <Arduino.h>

class HT {

    public:
        HT(int PIN_PWM, int PWM_CHANNEL, String NAME);

        float duty_cycle = 0;

        String state = "off";

        void heat (float DUTY_CYCLE);

        struct {
            String duty_cycle;
        } parameters;

    private:
        int pin_pwm, pwm_channel;
        int resolution = 8;
        float frequency = 1e3;

};