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
    /**
     * @brief Wait for the SPC to write given expectedValue value to given
     * outport port.
     *
     * @param port Port to read from 0 to 4.
     * @param expectedValue Expected value from SPC.
     *
     * @return Return true on successful read of expected value or false if value is not
     * received before timeout.
     */
    bool waitForInput(uint8_t port, uint8_t expectedValue);

    SpcHal& mSpcHal;
    bool mFirstTransfer;
    uint8_t mWriteCounter;
    uint32_t mTimeoutValue;
};

#endif
