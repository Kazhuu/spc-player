#ifndef SPC_PLAYER_SPC_PLAYER_HPP
#define SPC_PLAYER_SPC_PLAYER_HPP

#include "IplRomClient.hpp"
#include <stdint.h>

class SpcPlayer {
public:
    SpcPlayer(IplRomClient& iplRomClient);
    void writeCpuRegisters(uint16_t programCounter, uint8_t aRegister, uint8_t xRegister, uint8_t yRegister, uint8_t stackPointer, uint8_t programStatusWord);
    bool writeDspRegisters(uint8_t* dspRegisters);
    bool writeFirstPageRam(uint8_t* firstPageRam);
    bool writeSecondPageRam(uint8_t* secondPageRam);
    uint32_t writeRamByte(uint8_t byte);
    bool writeRamPacket(uint8_t* byte, uint32_t size);
    void resetRamWrite();
    bool start(uint16_t bootCodeAddress=0xFF85);
    uint8_t* getBootCode(uint32_t& size);

private:
    IplRomClient& mIplRomClient;
    bool mRestOfRamWriteStarted;
    uint32_t mRestOfRamWriteCount;
    uint16_t mProgramCounter;
    uint8_t mStackPointer;
    uint8_t mProgramStatusWord;
    uint8_t mPort0Value;
    uint8_t mPort1Value;
    uint8_t mPort2Value;
    uint8_t mPort3Value;
};

#endif
