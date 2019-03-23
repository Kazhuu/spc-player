#ifndef SPC_PLAYER_SPC_BUS_H
#define SPC_PLAYER_SPC_BUS_H

#include <stdint.h>

class SpcBus {
    public:
    SpcBus(uint8_t readPin, uint8_t writePin, uint8_t resetPin);
    void setDataPins(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    void setPortPins(uint8_t port0Pin, uint8_t port1Pin);
    void reset();
    uint8_t read(uint8_t port);
    void write(uint8_t port, uint8_t value);

    private:
    uint8_t _readPin;
    uint8_t _writePin;
    uint8_t _resetPin;
    uint8_t _dataPins[8];
    uint8_t _portPins[2];

    void _dataDirection(uint8_t mode);
};

#endif
