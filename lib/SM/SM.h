#include <Arduino.h>

struct range_frequency {
    float min, step, max;
};

struct Names {
    String frequency, state;
};

struct SM_PROPORTIES {
    float step_angle;
    struct range_frequency range;
};

class SM {

    public:
        SM(SM_PROPORTIES motor, int dir, int stp, int slp, int rst, String nm, int pwmchannel);
        void reset();
        void rotate(float duration);
        void stop();
        void pause(float duration);
        void move();
        float frequency = 0;
        struct range_frequency range;
        String state = "OFF";
        struct Names name;

    private:
        int pin_direction, pin_step, pin_sleep, pin_reset;
        int pwm_channel = 0;
        int resolution = 8;
        int dutyCycle = 125;
        float step_angle;

};