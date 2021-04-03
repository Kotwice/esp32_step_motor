#include <TC.h>

TC::TC(int PIN_SCK, int PIN_MISO, int PIN_CS, String NAME) {

    pin_sck = PIN_SCK;
    pin_miso = PIN_MISO;
    pin_cs = PIN_CS;

    parameters.temperature = "tc_temperature" + NAME;

    pinMode(pin_sck, OUTPUT);
    pinMode(pin_cs, OUTPUT);
    pinMode(pin_miso, INPUT);

    digitalWrite(pin_cs, HIGH);

}

void TC::probe() {

    uint16_t temporary;

    digitalWrite(pin_cs, LOW);
    delayMicroseconds(10);

    temporary = spi_transfer();
    temporary <<= 8;
    temporary |= spi_transfer();

    digitalWrite(pin_cs, HIGH);

    if (temporary & 0x4) {

        mesument = NAN;

    }

    temporary >>= 3;

    mesument = temporary * 0.25;
  
}

byte TC::spi_transfer() {

    byte temporary = 0;

    for (int i = 7; i >= 0; i--) {

        digitalWrite(pin_sck, LOW);
        delayMicroseconds(10);

        if (digitalRead(pin_miso)) {

            temporary |= (1 << i);

        }

        digitalWrite(pin_sck, HIGH);
        delayMicroseconds(10);

    }

    return temporary;

}