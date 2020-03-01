#include "SpcPlayer.hpp"

static uint8_t bootCode[] = {
    0x8F, 0x00, 0x00, //      Mov [0], #byte_0
    0x8F, 0x00, 0x01, //      Mov [1], #byte_1
    0x8F, 0xFF, 0xFC, //      Mov [0FCh], #timer_2
    0x8F, 0xFF, 0xFB, //      Mov [0FBh], #timer_1
    0x8F, 0x4F, 0xFA, //      Mov [0FAh], #timer_0
    0x8F, 0x31, 0xF1, //      Mov [0F1h], #ctrl_byte
    0xCD, 0x53,       //      Mov X, #Ack_byte
    0xD8, 0xF4,       //      Mov [0F4h], X

    0xE4, 0xF4,       //IN0:  Mov A, [0F4h]
    0x68, 0x00,       //      Cmp A, #IO_Byte_0
    0xD0, 0xFA,       //      Bne IN0

    0xE4, 0xF7,       //IN3:  Mov A, [0F7h]
    0x68, 0x00,       //      Cmp A, #IO_Byte_3
    0xD0, 0xFA,       //      Bne IN3

    0x8F, 0x6C, 0xF2, //      Mov [0F2h], 6Ch
    0x8F, 0x00, 0xF3, //      Mov [0F3h], #echo_control_byte
    0x8F, 0x4C, 0xF2, //      Mov [0F2h], 4Ch
    0x8F, 0x00, 0xF3, //      Mov [0F3h], #key_on_byte
    0x8F, 0x7F, 0xF2, //      Mov [0F2h], #dsp_control_register_byte
    0xCD, 0xF5,       //      Mov X, #stack_pointer
    0xBD,             //      Mov SP, X
    0xE8, 0xFF,       //      Mov A, #A_register_byte
    0x8D, 0x00,       //      Mov Y, #Y_register_byte
    0xCD, 0x00,       //      Mov X, #X_register_byte
    0x7F              //      RetI
};

SpcPlayer::SpcPlayer(IplRomClient& iplRomClient) :
    mIplRomClient(iplRomClient),
    mProgramCounter(0),
    mARegister(0),
    mXRegister(0),
    mYRegister(0),
    mStackPointer(0xEF),
    mProgramStatusWord(0),
    mDspKeyOnRegister(0),
    mDspFlagRegister(0) {}

void SpcPlayer::setCpuRegisters(uint16_t programCounter, uint8_t aRegister, uint8_t xRegister, uint8_t yRegister, uint8_t stackPointer, uint8_t programStatusWord) {
    mProgramCounter = programCounter;
    mARegister = aRegister;
    mXRegister = xRegister;
    mYRegister = yRegister;
    mStackPointer = stackPointer;
    mProgramStatusWord = programStatusWord;
}

bool SpcPlayer::setDspRegisters(uint8_t* dspRegisters) {
    uint8_t value;
    bool addressResult, write1Result, write2Result;
    for (uint8_t i = 0; i < 128; ++i) {
        uint8_t value = dspRegisters[i];
        // Key on register mute all voices.
        if (i == 0x4C) {
            mDspKeyOnRegister = dspRegisters[i];
            value = 0x00;
        // Control register, force mute and disable echo from messing memory.
        } else if (i == 0x6C) {
            mDspFlagRegister = dspRegisters[i];
            value = 0x60;
        }
        addressResult = mIplRomClient.setAddress(0x00F2);
        write1Result = mIplRomClient.write(i); // Write DSP address register
        write2Result = mIplRomClient.write(value); // Write DSP data register
        if (!addressResult || !write1Result || !write2Result) {
            return false;
        }
    }
    return true;
}

bool SpcPlayer::setFirstPageRam(uint8_t* firstPageRam) {
    return false;
}

bool SpcPlayer::setSecondPageRam(uint8_t* secondPageRam) {
    return false;
}

uint32_t SpcPlayer::writeRamByte(uint8_t byte) {
    return 0;
}
