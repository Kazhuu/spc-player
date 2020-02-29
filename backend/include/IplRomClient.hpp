#ifndef SPC_PLAYER_IPL_ROM_CLIENT_HPP
#define SPC_PLAYER_IPL_ROM_CLIENT_HPP

#include "SpcHal.hpp"
#include <stdint.h>

class IplRomClient {
public:
    IplRomClient(SpcHal& spcHal, uint32_t timeoutValue=500);
    bool reset();
    bool setAddress(uint16_t address);
    bool write(uint8_t data);
    bool start(uint16_t address);

private:
    void resetState();
    bool waitForInput(uint8_t port, uint8_t expectedValue);

    SpcHal& mSpcHal;
    bool mFirstTransfer;
    uint8_t mWriteCounter;
    uint32_t mTimeoutValue;
};

#endif
