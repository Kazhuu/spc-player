#ifndef SPC_PLAYER_SPC_WRITER_H
#define SPC_PLAYER_SPC_WRITER_H

#include "SpcBus.hpp"
#include <stdint.h>

class SpcWriter {
public:
    SpcWriter(SpcBus& spcBus);
    void reset();
    void setAddress(uint16_t address);
    uint16_t write(uint8_t data);
    void start(uint16_t address);

private:
    SpcBus& mSpcBus;
    bool mFirstTransfer;
    uint8_t mWriteCounter;
};

#endif
