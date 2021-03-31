#include <HT.h>

HT::HT (int PIN_PWM, int PWM_CHANNEL, String NAME) {

    pin_pwm = PIN_PWM;
    pwm_channel = PWM_CHANNEL;

    parameters.duty_cycle = "ht_duty_cycle_" + NAME;

    ledcAttachPin(pin_pwm, pwm_channel);
    ledcSetup(pwm_channel, frequency, resolution);

}

void HT::heat (float DUTY_CYCLE) {

    duty_cycle = DUTY_CYCLE;

    ledcWrite(pwm_channel, int(duty_cycle * (pow(2, resolution) - 1)));

}