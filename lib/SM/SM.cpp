#include <SM.h>

SM::SM(SM_PROPORTIES motor, int dir, int stp, int slp, int rst) {

    step_angle = motor.step_angle;
    range = motor.range;

    pin_direction = dir;
    pin_step = stp;
    pin_sleep = slp;
    pin_reset = rst;

    pinMode(pin_direction, OUTPUT);
    pinMode(pin_sleep, OUTPUT);
    pinMode(pin_reset, OUTPUT);

    digitalWrite(pin_direction, HIGH);
    digitalWrite(pin_sleep, LOW);
    digitalWrite(pin_reset, HIGH);

    ledcAttachPin(pin_step, pwm_channel);

}

void SM::pause(float duration) {
    float origin = millis();
    while (millis() - origin < duration * 1e3) {    }
}

void SM::move() {

    digitalWrite(pin_sleep, HIGH);

    ledcSetup(pwm_channel, frequency * 360 / step_angle, resolution);
    ledcWrite(pwm_channel, dutyCycle);

}

void SM::rotate(float duration = 0) {

    if (duration == 0) {
        move();
    }
    else {
        move();
        pause(duration);
        stop();
    }

}

void SM::stop() {

    digitalWrite(pin_sleep, LOW);
    ledcWrite(pwm_channel, 0);

}

void SM::reset() {

    digitalWrite(pin_reset, LOW);
    digitalWrite(pin_reset, HIGH);
    ledcWrite(pwm_channel, 0);

}