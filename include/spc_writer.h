#ifndef SPC_PLAYER_SPC_WRITER_H
#define SPC_PLAYER_SPC_WRITER_H

#include "spc_bus.h"

#include <stdint.h>

class SpcWriter {
    public:
    SpcWriter(SpcBus& spcBus);
    void reset();
    void setAddress(uint16_t address);
    uint16_t write(uint8_t data);
    void start(uint16_t address);

    private:
    SpcBus& _spcBus;
    bool _firstTransfer;
    uint8_t _writeCounter;
};

#endif
