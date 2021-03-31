#include <Arduino.h>
#include <vector>

class LCS {

    public:

        LCS (int RELAY, int PIN_PWM, int PWM_CHANNEL);

        void rotate(float POWER);

        void power(String STATE);

        float duty_cycle = 0;

        String state = "off";

        struct {

            String duty_cycle = "cl_duty_cycle";
            String state = "lcs_state";

        } parameters;
        
        struct {

            float min = 0;
            float step = 0.1;
            float max = 1;

        } range;

    private:
        int pin_relay;
        int pin_pwm;
        int pwm_channel;
        int resolution = 8;
        float frequency = 1e3;

};