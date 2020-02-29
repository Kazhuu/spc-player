#ifndef SPC_PLAYER_SPC_HAL_HPP
#define SPC_PLAYER_SPC_HAL_HPP

#include <stdint.h>

class SpcHal {
public:
    SpcHal(uint8_t readPin, uint8_t writePin, uint8_t resetPin);
    void setDataPins(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    void setPortPins(uint8_t port0Pin, uint8_t port1Pin);
    void reset();
    uint8_t read(uint8_t port);
    void write(uint8_t port, uint8_t value);

private:
    void dataDirection(uint8_t mode);

    uint8_t mReadPin;
    uint8_t mWritePin;
    uint8_t mResetPin;
    uint8_t mDataPins[8];
    uint8_t mPortPins[2];
};

#endif
