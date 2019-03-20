#include "spc_writer.h"
#include "spc_bus.h"

#include "Arduino.h"

SpcWriter::SpcWriter(SpcBus& spcBus) : _spcBus(spcBus) {};

void SpcWriter::init() {
    this->_spcBus.init();
    this->_firstTransfer = true;
    // Wait for SPC to be ready.
    while (this->_spcBus.read(0) != 0xAA);
    while (this->_spcBus.read(1) != 0xBB);
}

void SpcWriter::reset() {
    this->_spcBus.reset();
    this->_firstTransfer = true;
}

void SpcWriter::writeBlock(uint16_t address, uint8_t* data, int length) {
    this->_spcBus.write(1, 1);
    this->_spcBus.write(2, address & 0xFF);
    this->_spcBus.write(3, address >> 8);
    if (this->_firstTransfer) {
        this->_spcBus.write(0, 0xCC);
        while (this->_spcBus.read(0) != 0xCC);
        this->_firstTransfer = false;
    } else {
        uint8_t lastValue = this->_spcBus.read(0) + 2;
        // Next value cannot be zero, so add 1 or more.
        if (lastValue == 0)
            lastValue = 1;
        this->_spcBus.write(0, lastValue);
        while (this->_spcBus.read(0) != lastValue);
    }
    for (int i = 0; i < length; i++) {
        this->_spcBus.write(1, data[i]);
        this->_spcBus.write(0, (i & 0xFF));
        while (this->_spcBus.read(0) != (i & 0xFF));
    };
}

void SpcWriter::start(uint16_t address) {
    // Port 0 value needs to be incremented by 2 or more to tell SPC to start
    // execution. If first time then 0xCC.
    uint8_t lastValue = this->_spcBus.read(0) + 2;
    if (this->_firstTransfer) {
        lastValue = 0xCC;
        this->_firstTransfer = false;
    }
    // Write execution address and wait for response.
    this->_spcBus.write(1, 0x00);
    this->_spcBus.write(2, address & 0xFF);
    this->_spcBus.write(3, address >> 8);
    this->_spcBus.write(0, lastValue);
    while (this->_spcBus.read(0) != lastValue);
}

