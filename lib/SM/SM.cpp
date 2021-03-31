#include <SM.h>

SM::SM(int PIN_STEP, int PIN_SLEEP, int PWM_CHANNEL, String NAME) {

    pin_step = PIN_STEP;
    pin_sleep = PIN_SLEEP;
    pwm_channel = PWM_CHANNEL;

    parameters.frequency = "sm_frequency_" + NAME;
    parameters.state = "sm_state_" + NAME;

    pinMode(pin_sleep, OUTPUT);
    digitalWrite(pin_sleep, LOW);

    ledcAttachPin(pin_step, pwm_channel);

}

void SM::rotate(float FREQUENCY) {

    frequency = FREQUENCY;

    ledcSetup(pwm_channel, frequency * 360 / step_angle, resolution);
    ledcWrite(pwm_channel, dutyCycle);

}

void SM::power (String STATE) {

    state = STATE;

    if (state == "on") {
        digitalWrite(pin_sleep, HIGH);
    }
    else {
        digitalWrite(pin_sleep, LOW);
    }

}