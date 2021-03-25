#include <Arduino.h>

class TC {

  public:
    TC(int8_t sck, int8_t miso, int8_t cs);
    void probe();  
    float value = 0;
    //void (TC::*T)() = &TC::get_temperature;

    
  private:
    int8_t SCK, MISO, CS;
    uint8_t spi_transfer();
    void pause(float duration);
    //hw_timer_t * timer = NULL;

};

