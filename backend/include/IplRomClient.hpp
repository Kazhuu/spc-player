#ifndef SPC_PLAYER_IPL_ROM_CLIENT_HPP
#define SPC_PLAYER_IPL_ROM_CLIENT_HPP

#include "SpcHal.hpp"
#include <stdint.h>

class IplRomClient {
public:
    IplRomClient(SpcHal& spcHal);
    SpcHal& getSpcHal();
    bool reset();
    bool setAddress(uint16_t address);
    bool write(uint8_t value);
    void writeWithouAcknowledge(uint8_t value);
    bool start(uint16_t address);

private:
    /**
     * @brief Wait for the SPC to write given expectedValue value to given
     * outport port.
     *
     * @param port Port to read from 0 to 4.
     * @param expectedValue Expected value from SPC.
     * @param timeoutCounter Loop counter to wait for the input.
     *
     * @return Return true on successful read of expected value or false if value is not
     * received before timeout.
     */
    bool waitForInput(uint8_t port, uint8_t expectedValue, uint32_t timeoutCounter);

    SpcHal& mSpcHal;
    bool mFirstTransfer;
    uint8_t mWriteCounter;
};

#endif
