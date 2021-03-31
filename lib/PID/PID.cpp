#include <PID.h>

PID::PID (float DESTINATION, float DURATION, float I, float P, float D, String NAME) {

    destination = DESTINATION;
    duration = DURATION;

    coefficients.i = I;
    coefficients.p = P;
    coefficients.d = D;

    parameters.i = "pid_i_" + NAME;
    parameters.p = "pid_p_" + NAME;
    parameters.d = "pid_d_" + NAME;
    parameters.destination = "pid_destination_" + NAME;
    parameters.state = "pid_state_" + NAME;

}

void PID::calculate (float MESUMENT) {

    mesuments.current = MESUMENT;

    errors.current = (destination - mesuments.current) / destination;
    errors.previously = (destination - mesuments.previously) / destination;

    integral = integral + errors.current * duration;
    proportional = errors.current;
    differential = (errors.current - errors.previously) / duration;

    control = coefficients.i * integral + coefficients.p * proportional + coefficients.d * differential;

    if (control < 0) {
        control = 0;
    }

    if (control > 1) {
        control = 1;
    }

    if (state != "on") {
        control = 0;
    }

    mesuments.previously = mesuments.current;

}