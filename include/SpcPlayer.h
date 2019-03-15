#include <stdint.h>

class SpcPlayer {
    public:
    SpcPlayer(uint8_t readPin, uint8_t writePin, uint8_t resetPin);
    void setDataPins(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    void setPortPins(uint8_t port0Pin, uint8_t port1Pin);
    uint8_t read(uint8_t port);
};
