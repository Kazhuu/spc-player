#ifndef SPC_PLAYER_SPC_WRITER_H
#define SPC_PLAYER_SPC_WRITER_H

#include "spc_bus.h"

#include <stdint.h>

class SpcWriter {
    public:
    SpcWriter(SpcBus& spcBus);
    void init();
    void reset();
    void writeBlock(uint16_t address, uint8_t* data, int length);
    void start(uint16_t address);

    private:
    SpcBus& _spcBus;
    bool _firstTransfer;
};

#endif
