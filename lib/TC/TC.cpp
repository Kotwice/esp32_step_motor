#include <TC.h>

TC::TC(int8_t sck, int8_t miso, int8_t cs) {

    SCK = sck;
    CS = cs;
    MISO = miso;

    pinMode(SCK, OUTPUT);
    pinMode(CS, OUTPUT);
    pinMode(MISO, INPUT);

    digitalWrite(CS, HIGH);

    //timer = timerBegin(0, 80, true);
    //timerAttachInterrupt(timer, &T, true);
    //timerAlarmWrite(timer, duration * 1e6, true);

    //timerAlarmEnable(timer);

}

void TC::probe() {

    uint16_t temporary;

    digitalWrite(CS, LOW);
    //pause(10);
    delayMicroseconds(10);

    temporary = spi_transfer();
    temporary <<= 8;
    temporary |= spi_transfer();

    digitalWrite(CS, HIGH);

    if (temporary & 0x4) {

        value = NAN;

    }

    temporary >>= 3;

    value = temporary * 0.25;
  
}

void TC::pause(float duration) {
    float origin = millis();
    while (millis() - origin < duration) {    }
}

byte TC::spi_transfer() {

    byte temporary = 0;

    for (int i = 7; i >= 0; i--) {

        digitalWrite(SCK, LOW);
        //pause(10);
        delayMicroseconds(10);

        if (digitalRead(MISO)) {

        temporary |= (1 << i);

        }

        digitalWrite(SCK, HIGH);
        //pause(10);
        delayMicroseconds(10);

    }

    return temporary;

}