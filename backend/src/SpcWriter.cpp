#include "SpcWriter.hpp"
#include "SpcBus.hpp"
#include "Arduino.h"

SpcWriter::SpcWriter(SpcBus& spcBus) : mSpcBus(spcBus), mFirstTransfer(true), mWriteCounter(0) {};

void SpcWriter::reset() {
    mSpcBus.reset();
    mWriteCounter = 0;
    mFirstTransfer = true;
    // Wait for SPC to be ready.
    while (mSpcBus.read(0) != 0xAA);
    while (mSpcBus.read(1) != 0xBB);
}

void SpcWriter::setAddress(uint16_t address) {
    mSpcBus.write(1, 1);
    mSpcBus.write(2, address & 0xFF);
    mSpcBus.write(3, address >> 8);
    if (mFirstTransfer) {
        mWriteCounter = 0xCC;
        mSpcBus.write(0, mWriteCounter);
        while (mSpcBus.read(0) != mWriteCounter);
        mFirstTransfer = false;
    } else {
        mWriteCounter += 2;
        // Next value cannot be zero, so add 1 or more.
        if (mWriteCounter == 0)
            mWriteCounter = 1;
        mSpcBus.write(0, mWriteCounter);
        while (mSpcBus.read(0) != mWriteCounter);
    }
    mWriteCounter = 0;
}

uint16_t SpcWriter::write(uint8_t data) {
    mSpcBus.write(1, data);
    mSpcBus.write(0, mWriteCounter);
    while (mSpcBus.read(0) != mWriteCounter);
    mWriteCounter++;
    return mWriteCounter;
}

void SpcWriter::start(uint16_t address) {
    // Port 0 value needs to be incremented by 2 or more to tell SPC to start
    // execution. If first time then 0xCC.
    if (mFirstTransfer) {
        mWriteCounter = 0xCC;
        mFirstTransfer = false;
    } else {
        mWriteCounter += 2;
    }
    // Write execution address and wait for response.
    mSpcBus.write(1, 0x00);
    mSpcBus.write(2, address & 0xFF);
    mSpcBus.write(3, address >> 8);
    mSpcBus.write(0, mWriteCounter);
    while (mSpcBus.read(0) != mWriteCounter);
}

