#include "spc_writer.h"
#include "spc_bus.h"

#include "Arduino.h"

SpcWriter::SpcWriter(SpcBus& spcBus) : _spcBus(spcBus), _firstTransfer(true), _writeCounter(0) {};

void SpcWriter::reset() {
    this->_spcBus.reset();
    this->_writeCounter = 0;
    this->_firstTransfer = true;
    // Wait for SPC to be ready.
    while (this->_spcBus.read(0) != 0xAA);
    while (this->_spcBus.read(1) != 0xBB);
}

void SpcWriter::setAddress(uint16_t address) {
    this->_spcBus.write(1, 1);
    this->_spcBus.write(2, address & 0xFF);
    this->_spcBus.write(3, address >> 8);
    if (this->_firstTransfer) {
        this->_writeCounter = 0xCC;
        this->_spcBus.write(0, this->_writeCounter);
        while (this->_spcBus.read(0) != this->_writeCounter);
        this->_firstTransfer = false;
    } else {
        this->_writeCounter += 2;
        // Next value cannot be zero, so add 1 or more.
        if (this->_writeCounter == 0)
            this->_writeCounter = 1;
        this->_spcBus.write(0, this->_writeCounter);
        while (this->_spcBus.read(0) != this->_writeCounter);
    }
    this->_writeCounter = 0;
}

uint16_t SpcWriter::write(uint8_t data) {
    this->_spcBus.write(1, data);
    this->_spcBus.write(0, this->_writeCounter);
    while (this->_spcBus.read(0) != this->_writeCounter);
    this->_writeCounter++;
    return this->_writeCounter;
}

void SpcWriter::start(uint16_t address) {
    // Port 0 value needs to be incremented by 2 or more to tell SPC to start
    // execution. If first time then 0xCC.
    if (this->_firstTransfer) {
        this->_writeCounter = 0xCC;
        this->_firstTransfer = false;
    } else {
        this->_writeCounter += 2;
    }
    // Write execution address and wait for response.
    this->_spcBus.write(1, 0x00);
    this->_spcBus.write(2, address & 0xFF);
    this->_spcBus.write(3, address >> 8);
    this->_spcBus.write(0, this->_writeCounter);
    while (this->_spcBus.read(0) != this->_writeCounter);
}

