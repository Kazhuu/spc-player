#include "IplRomClient.hpp"

IplRomClient::IplRomClient(SpcHal& spcHal, uint32_t timeoutValue) :
    mSpcHal(spcHal),
    mFirstTransfer(true),
    mWriteCounter(0),
    mTimeoutValue(timeoutValue) {};

SpcHal& IplRomClient::getSpcHal() {
    return mSpcHal;
}

bool IplRomClient::reset() {
    mWriteCounter = 0;
    mFirstTransfer = true;
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
        // Wait for SPC to respond on with write counter from port 0.
        if (!waitForInput(0, mWriteCounter)) {
            reset();
            return false;
        }
        mFirstTransfer = false;
    } else {
        mWriteCounter += 2;
        // Next value cannot be zero, so add 1 or more.
        if (mWriteCounter == 0)
            mWriteCounter = 1;
        mSpcHal.write(0, mWriteCounter);
        // Wait for SPC to respond on with write counter from port 0.
        if (!waitForInput(0, mWriteCounter)) {
            reset();
            return false;
        }
    }
    mWriteCounter = 0;
    return true;
}

bool IplRomClient::write(uint8_t data) {
    mSpcHal.write(1, data);
    mSpcHal.write(0, mWriteCounter);
    // Wait for SPC to respond on with write counter from port 0.
    if (!waitForInput(0, mWriteCounter)) {
        reset();
        return false;
    }
    mWriteCounter++;
    return true;
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
    return waitForInput(0, mWriteCounter);
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
