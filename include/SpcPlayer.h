#ifndef SpcPlayer_h
#define SpcPlayer_h

#include <stdint.h>

class SpcPlayer {
    public:
    SpcPlayer(uint8_t readPin, uint8_t writePin, uint8_t resetPin);
    void setDataPins(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    void setPortPins(uint8_t port0Pin, uint8_t port1Pin);
    void init();
    void reset();
    uint8_t read(uint8_t port);
    void write(uint8_t port, uint8_t value);
    void writeBlock(uint16_t address, uint8_t* data, int length);

    private:
    uint8_t _readPin;
    uint8_t _writePin;
    uint8_t _resetPin;
    uint8_t _dataPins[8];
    uint8_t _portPins[2];
    bool _firstTransfer;

    void _dataDirection(uint8_t mode);
};

#endif
