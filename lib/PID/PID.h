#include <Arduino.h>

class PID {

    public:

        PID (float DESTINATION, float DURATION, float I, float P, float D, String NAME);

        String state = "off";

        struct {
            String i, p, d, destination, state;
        } parameters;

        struct {
            float p, i, d;
        } coefficients;

        struct {
            float previously = 30, current;
        } mesuments, errors;

        float control, destination, duration;

        void calculate (float MESUMENT);

    private:

        float integral = 0, proportional, differential;

};