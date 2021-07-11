#include <WebUSB.h>

#include "SpcPlayer.hpp"
#include "SpcHal.hpp"
#include "IplRomClient.hpp"
#include "Uart.hpp"
#include "Arduino.h"

#define SERIAL_WRITE_SUCCESS '1'
#define SERIAL_WRITE_ERROR '0'

#define PORT_0_PIN 8
#define PORT_1_PIN 9

#define READ_PIN 10
#define WRITE_PIN 11

#define DATA_0_PIN 12
#define DATA_1_PIN 13
#define DATA_2_PIN 2
#define DATA_3_PIN 3
#define DATA_4_PIN 4
#define DATA_5_PIN 5
#define DATA_6_PIN 6
#define DATA_7_PIN 7

#define RESET_PIN A0

#define BAUD_RATE 38400

WebUSB WebUSBSerial(0 /* http:// */, "localhost:8080");

#define Serial WebUSBSerial

SpcHal spcHal(READ_PIN, WRITE_PIN, RESET_PIN);
IplRomClient iplRomClient(spcHal);
SpcPlayer spcPlayer(iplRomClient);

void readPorts() {
    for (uint8_t i = 0; i < 4; ++i) {
        Serial.write(spcHal.read(i));
    }
}

void serialWriteResult(bool result) {
    if (result) {
        Serial.write(SERIAL_WRITE_SUCCESS);
    } else {
        Serial.write(SERIAL_WRITE_ERROR);
    }
}

void handleSerialCommand() {
    if (Serial.available()) {
        char result = Serial.read();
        bool uartReadResult;
        uint32_t ramWrittenAmount = 0;
        uint8_t* bootCodePointer;
        uint32_t bootCodeSize;
        uint8_t value;
        uint8_t port;
        uint16_t programCounter;
        uint8_t aRegister, xRegister, yRegister, stackPointer, cpuFlags;
        uint8_t page[256];

        switch (result) {
            case 'R': // Reset SPC.
                serialWriteResult(iplRomClient.reset());
                break;

            case 'C': // Get CPU registers PC, A, X, Y, SP and PSW.
                programCounter = Uart::readShort(&uartReadResult);
                if (!uartReadResult) {
                    Serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                aRegister = Uart::readByte(&uartReadResult);
                if (!uartReadResult) {
                    Serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                xRegister = Uart::readByte(&uartReadResult);
                if (!uartReadResult) {
                    Serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                yRegister = Uart::readByte(&uartReadResult);
                if (!uartReadResult) {
                    Serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                stackPointer = Uart::readByte(&uartReadResult);
                if (!uartReadResult) {
                    Serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                cpuFlags = Uart::readByte(&uartReadResult);
                if (!uartReadResult) {
                    Serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                spcPlayer.writeCpuRegisters(programCounter, aRegister, xRegister, yRegister, stackPointer, cpuFlags);
                Serial.write(SERIAL_WRITE_SUCCESS);
                break;

            // Write all 128 DSP register. One byte per register.
            case 'D':
                for (uint32_t i = 0; i < 128; ++i) {
                    page[i] = Uart::readByte(&uartReadResult);
                    if (!uartReadResult) {
                        Serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (uartReadResult) {
                    bool dspResult = spcPlayer.writeDspRegisters(page);
                    if (dspResult) {
                        Serial.write(SERIAL_WRITE_SUCCESS);
                    } else {
                        Serial.write(SERIAL_WRITE_ERROR);
                    }
                }
                break;

            // Write page 0 of SPC ram, 256 bytes.
            case '0':
                for (uint32_t i = 0; i < 256; ++i) {
                    page[i] = Uart::readByte(&uartReadResult);
                    if (!uartReadResult) {
                        Serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (uartReadResult) {
                    bool firstPageResult = spcPlayer.writeFirstPageRam(page);
                    if (firstPageResult) {
                        Serial.write(SERIAL_WRITE_SUCCESS);
                    } else {
                        Serial.write(SERIAL_WRITE_ERROR);
                    }
                }
                break;

            // Write page 1 of SPC ram, 256 bytes.
            case '1':
                for (uint32_t i = 0; i < 256; ++i) {
                    page[i] = Uart::readByte(&uartReadResult);
                    if (!uartReadResult) {
                        Serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (uartReadResult) {
                    bool secondPageResult = spcPlayer.writeSecondPageRam(page);
                    if  (secondPageResult) {
                        Serial.write(SERIAL_WRITE_SUCCESS);
                    } else {
                        Serial.write(SERIAL_WRITE_ERROR);
                    }
                }
                break;

            // Write rest of the ram bytes after page 1. Needs to followed by 64960
            // bytes, will only then be successful.
            case '2':
                for (uint32_t i = 0; i < 0xFFC0 - 0x200; ++i) {
                    value = Uart::readByte(&uartReadResult);
                    if (!uartReadResult) {
                        spcPlayer.resetRamWrite();
                        Serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                    ramWrittenAmount = spcPlayer.writeRamByte(value);
                    if (ramWrittenAmount == 0) {
                        spcPlayer.resetRamWrite();
                        Serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (ramWrittenAmount == 0xFFC0 - 0x200) {
                    Serial.write(SERIAL_WRITE_SUCCESS);
                }
                break;

            // Start executing.
            case 'S':
                serialWriteResult(spcPlayer.start());
                break;

            case 'Q': // Read value of all four ports.
                readPorts();
                break;

            // Write single port. Two following bytes are read for address and
            // value.
            case 'W':
                port = Uart::readByte(&uartReadResult);
                if (!uartReadResult) {
                    Serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                value = Uart::readByte(&uartReadResult);
                if (!uartReadResult) {
                    Serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                spcHal.write(port, value);
                Serial.write(SERIAL_WRITE_SUCCESS);
                break;

            // Read boot code by first writing it's size in one byte and
            // followed by boot code amount of size.
            case 'B':
                bootCodePointer = spcPlayer.getBootCode(bootCodeSize);
                Serial.write((uint8_t)bootCodeSize);
                for (uint32_t i = 0; i < bootCodeSize; ++i) {
                    Serial.write(bootCodePointer[i]);
                }
                break;
        }
    }
}

void setup() {
    while (!Serial) {
        ;
    }
    Serial.begin(BAUD_RATE);
    spcHal.setPortPins(PORT_0_PIN, PORT_1_PIN);
    spcHal.setDataPins(DATA_0_PIN, DATA_1_PIN, DATA_2_PIN, DATA_3_PIN, DATA_4_PIN, DATA_5_PIN, DATA_6_PIN, DATA_7_PIN);
}

void loop() {
    handleSerialCommand();
    Serial.flush();
}
