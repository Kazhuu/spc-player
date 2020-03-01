#ifndef SPC_PLAYER_SPC_PLAYER_HPP
#define SPC_PLAYER_SPC_PLAYER_HPP

#include "IplRomClient.hpp"
#include <stdint.h>

class SpcPlayer {
public:
    SpcPlayer(IplRomClient& iplRomClient);
    void setCpuRegisters(uint16_t programCounter, uint8_t aRegister, uint8_t xRegister, uint8_t yRegister, uint8_t stackPointer, uint8_t programStatusWord);
    bool setDspRegisters(uint8_t* dspRegisters);
    bool setFirstPageRam(uint8_t* firstPageRam);
    bool setSecondPageRam(uint8_t* secondPageRam);
    uint32_t writeRamByte(uint8_t byte);

private:
    IplRomClient& mIplRomClient;
    uint16_t mProgramCounter;
    uint8_t mARegister, mXRegister, mYRegister;
    uint8_t mStackPointer;
    uint8_t mProgramStatusWord;
    uint8_t mDspKeyOnRegister;
    uint8_t mDspFlagRegister;
};

#endif
