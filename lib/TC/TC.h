#include <Arduino.h>

class TC {

  public:

    TC(int PIN_SCK, int PIN_MISO, int PIN_CS, String NAME);

    void probe();  

    float mesument = 0;

    struct {
      String temperature;
    } parameters;

  private:

    int pin_sck, pin_miso, pin_cs;

    byte spi_transfer();

};

