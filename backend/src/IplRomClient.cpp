#include "IplRomClient.hpp"

IplRomClient::IplRomClient(SpcHal& spcHal, uint32_t timeoutValue) :
    mSpcHal(spcHal),
    mFirstTransfer(true),
    mWriteCounter(0),
    mTimeoutValue(timeoutValue) {};

bool IplRomClient::reset() {
    resetState();
    mSpcHal.reset();
    // Wait for SPC to be ready.
    bool result1 = waitForInput(0, 0xAA);
    bool result2 = waitForInput(1, 0xBB);
    return result1 && result2;
}

bool IplRomClient::setAddress(uint16_t address) {
    mSpcHal.write(1, 1);
    mSpcHal.write(2, address & 0xFF);
    mSpcHal.write(3, address >> 8);
    if (mFirstTransfer) {
        mWriteCounter = 0xCC;
        mSpcHal.write(0, mWriteCounter);
        // TODO: Remove these lock potential statements with waitForInput()
        // instead.
        while (mSpcHal.read(0) != mWriteCounter);
        mFirstTransfer = false;
    } else {
        mWriteCounter += 2;
        // Next value cannot be zero, so add 1 or more.
        if (mWriteCounter == 0)
            mWriteCounter = 1;
        mSpcHal.write(0, mWriteCounter);
        while (mSpcHal.read(0) != mWriteCounter);
    }
    mWriteCounter = 0;
}

bool IplRomClient::write(uint8_t data) {
    mSpcHal.write(1, data);
    mSpcHal.write(0, mWriteCounter);
    while (mSpcHal.read(0) != mWriteCounter);
    mWriteCounter++;
    return mWriteCounter;
}

bool IplRomClient::start(uint16_t address) {
    // Port 0 value needs to be incremented by 2 or more to tell SPC to start
    // execution. If first time then 0xCC.
    if (mFirstTransfer) {
        mWriteCounter = 0xCC;
        mFirstTransfer = false;
    } else {
        mWriteCounter += 2;
    }
    // Write execution address and wait for response.
    mSpcHal.write(1, 0x00);
    mSpcHal.write(2, address & 0xFF);
    mSpcHal.write(3, address >> 8);
    mSpcHal.write(0, mWriteCounter);
    while (mSpcHal.read(0) != mWriteCounter);
}

void IplRomClient::resetState() {
    mWriteCounter = 0;
    mFirstTransfer = true;
}

bool IplRomClient::waitForInput(uint8_t port, uint8_t expectedValue) {
    uint32_t counter = 0;
    uint8_t input = expectedValue - 1;
    while (input != expectedValue) {
        if (counter > mTimeoutValue) {
            return false;
        }
        counter++;
        input = mSpcHal.read(port);
    }
    return true;
}
