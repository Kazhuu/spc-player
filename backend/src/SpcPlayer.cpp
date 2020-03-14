#include "SpcPlayer.hpp"

static uint8_t bootCode[] = {
    0x8F, 0x00, 0x00, //      Mov [0], #byte_0
    0x8F, 0x00, 0x01, //      Mov [1], #byte_1
    0x8F, 0xFF, 0xFC, //      Mov [0FCh], #timer_2
    0x8F, 0xFF, 0xFB, //      Mov [0FBh], #timer_1
    0x8F, 0x4F, 0xFA, //      Mov [0FAh], #timer_0
    0x8F, 0x00, 0xF1, //      Mov [0F1h], #ctrl_byte
    0xCD, 0x23,       //      Mov X, #Ack_byte
    0xD8, 0xF5,       //      Mov [0F5h], X

    0xE4, 0xF4,       //IN0:  Mov A, [0F4h]
    0x68, 0x01,       //      Cmp A, #IO_Byte_0
    0xD0, 0xFA,       //      Bne IN0

    0xE4, 0xF7,       //IN3:  Mov A, [0F7h]
    0x68, 0x00,       //      Cmp A, #IO_Byte_3
    0xD0, 0xFA,       //      Bne IN3

    0x8F, 0x6C, 0xF2, //      Mov [0F2h], 6Ch
    0x8F, 0x00, 0xF3, //      Mov [0F3h], #echo_control_byte
    0x8F, 0x4C, 0xF2, //      Mov [0F2h], 4Ch
    0x8F, 0x00, 0xF3, //      Mov [0F3h], #key_on_byte
    0x8F, 0x7F, 0xF2, //      Mov [0F2h], #dsp_control_register_byte
    0xCD, 0xEF,       //      Mov X, #stack_pointer
    0xBD,             //      Mov SP, X
    0xE8, 0x00,       //      Mov A, #A_register_byte
    0xCD, 0x00,       //      Mov X, #X_register_byte
    0x8D, 0x00,       //      Mov Y, #Y_register_byte
    0x7F              //      RetI
};

SpcPlayer::SpcPlayer(IplRomClient& iplRomClient) :
    mIplRomClient(iplRomClient),
    mRestOfRamWriteStarted(false),
    mRestOfRamWriteCount(0),
    mProgramCounter(0),
    mStackPointer(0xEF),
    mProgramStatusWord(0),
    mPort0Value(0),
    mPort1Value(0),
    mPort2Value(0),
    mPort3Value(0) {}

void SpcPlayer::writeCpuRegisters(uint16_t programCounter, uint8_t aRegister, uint8_t xRegister, uint8_t yRegister, uint8_t stackPointer, uint8_t programStatusWord) {
    mProgramCounter = programCounter;
    bootCode[53] = aRegister;
    bootCode[55] = xRegister;
    bootCode[57] = yRegister;
    if (stackPointer < 3) {
        mStackPointer = 0x03;
    } else {
        mStackPointer = stackPointer;
    }
    mProgramStatusWord = programStatusWord;
}

bool SpcPlayer::writeDspRegisters(uint8_t* dspRegisters) {
    bool addressResult, write1Result, write2Result;
    for (uint8_t i = 0; i < 128; ++i) {
        uint8_t value = dspRegisters[i];
        // DSP key on (KON) register, mute all voices from 0 to 7.
        if (i == 0x4C) {
            bootCode[44] = value;
            value = 0x00;
        // DSP control (FLG) register, force mute and disable echo from messing memory.
        } else if (i == 0x6C) {
            bootCode[38] = value;
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

bool SpcPlayer::writeFirstPageRam(uint8_t* firstPageRam) {
    bool result = mIplRomClient.setAddress(0x0002);
    if (!result) {
        return false;
    }
    // Preserve first two bytes of RAM. They are used by IPL ROM during writing
    // procedure.
    bootCode[1] = firstPageRam[0];
    bootCode[4] = firstPageRam[1];
    // Write first page ram from 0x0002 to until peripheral registers which start from 0x00F0.
    for (uint32_t i = 0x0002; i < 0x00F0; ++i) {
        uint8_t value = firstPageRam[i];
        result = mIplRomClient.write(value);
        if (!result) {
            return false;
        }
    }
     // Control register.
    bootCode[16] = firstPageRam[0x00F1];
     // DSP address register.
    bootCode[47] = firstPageRam[0x00F2];
    // Store original port values. Will be written after boot code is executed.
    mPort0Value = firstPageRam[0x00F4];
    mPort1Value = firstPageRam[0x00F5];
    mPort2Value = firstPageRam[0x00F6];
    mPort3Value = firstPageRam[0x00F7];
    // This byte boot code will expect from port 3.
    bootCode[31] = mPort3Value;
    // Store original timer values.
    bootCode[7] = firstPageRam[0x00FA];
    bootCode[10] = firstPageRam[0x00FB];
    bootCode[13] = firstPageRam[0x00FC];
    return true;
}

bool SpcPlayer::writeSecondPageRam(uint8_t* secondPageRam) {
    bool result = mIplRomClient.setAddress(0x0100);
    if (!result) {
        return false;
    }
    uint16_t stackPointer = mStackPointer + 0x100;
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t ramAddress = i + 0x100;
        uint8_t value = secondPageRam[i];
        // Stack pointer for the boot code.
        if (stackPointer - 3 == ramAddress) {
            bootCode[50] = mStackPointer - 3;
        // Boot code RETI opcode will first pop PSW from the stack.
        } else if (stackPointer - 2 == ramAddress) {
            value = mProgramStatusWord;
        // Then RETI opcode will pop two bytes of program counter. This is the
        // start of the program in the SPC file.
        } else if (stackPointer - 1 == ramAddress) {
            value = (uint8_t)(mProgramCounter >> 8);
        } else if (stackPointer == ramAddress) {
            value = (uint8_t)(mProgramCounter & 0xFF);
        }
        result = mIplRomClient.write(value);
        if (!result) {
            return false;
        }
    }
    return true;
}

uint32_t SpcPlayer::writeRamByte(uint8_t byte) {
    if (mRestOfRamWriteCount > 0xFFC0 - 0x200) {
        return 0xFFC0 - 0x200;
    }
    if (!mRestOfRamWriteStarted) {
        bool result = mIplRomClient.setAddress(0x0200);
        if (!result) {
            mRestOfRamWriteCount = 0;
            return 0;
        }
        mRestOfRamWriteCount = 0;
        mRestOfRamWriteStarted = true;
    }
    mIplRomClient.writeWithouAcknowledge(byte);
    mRestOfRamWriteCount++;
    return mRestOfRamWriteCount;
}

void SpcPlayer::resetRamWrite() {
    mRestOfRamWriteCount = 0;
    mRestOfRamWriteStarted = false;
}

bool SpcPlayer::start(uint16_t bootCodeAddress) {
    bool result = mIplRomClient.setAddress(bootCodeAddress);
    if (!result) {
        return false;
    }
    for (uint32_t i = 0; i < sizeof(bootCode); ++i) {
        result = mIplRomClient.write(bootCode[i]);
        if (!result) {
            return false;
        }
    }
    result = mIplRomClient.start(bootCodeAddress);
    if (!result) {
        mIplRomClient.reset();
        return false;
    }
    SpcHal& spcHal = mIplRomClient.getSpcHal();
    uint8_t acknowledge = spcHal.read(1);
    if (acknowledge != 0x23) {
        mIplRomClient.reset();
        return false;
    }
    spcHal.write(1, mPort1Value);
    spcHal.write(2, mPort2Value);
    spcHal.write(3, mPort3Value);
    spcHal.write(0, 0x01);
    spcHal.write(0, mPort0Value);
    return true;
}

uint8_t* SpcPlayer::getBootCode(uint32_t& size) {
    size = sizeof(bootCode);
    return bootCode;
}
