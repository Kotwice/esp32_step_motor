#include <LCS.h>

LCS::LCS(int PIN_RELAY, int PIN_PWM, int PWM_CHANNEL) {

    pin_relay = PIN_RELAY;
    pin_pwm = PIN_PWM;
    pwm_channel = PWM_CHANNEL;

    pinMode(pin_relay, OUTPUT);

    digitalWrite(pin_relay, LOW);

    ledcAttachPin(pin_pwm, pwm_channel);

    ledcSetup(pwm_channel, frequency, resolution);

    rotate(duty_cycle);

}

void LCS::rotate (float DUTY_CYCLE) {

    duty_cycle = DUTY_CYCLE;

    ledcWrite(pwm_channel, int(duty_cycle * (pow(2, resolution) - 1)));

}

void LCS::power (String STATE) {

    state = STATE;

    if (state == "on") {
        digitalWrite(pin_relay, HIGH);
    }
    else {
        digitalWrite(pin_relay, LOW);
    }

}