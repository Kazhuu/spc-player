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

#define BAUD_RATE 115200


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
        uint8_t data;
        uint8_t port;
        uint16_t programCounter;
        uint8_t aRegister, xRegister, yRegister, stackPointer, cpuFlags;
        uint8_t page[256];

        switch (result) {
            case 'Q': // Read value of all four ports.
                readPorts();
                break;

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
                    bool dspResult = spcPlayer.setDspRegisters(page);
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
                    bool dspResult = spcPlayer.setFirstPageRam(page);
                    if (dspResult) {
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
                    Serial.write(SERIAL_WRITE_SUCCESS);
                }
                break;

            // Write single port. Two following bytes are read for address and
            // value.
            case 'W':
                port = Uart::readByte(&uartReadResult);
                if (!uartReadResult) {
                    Serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                data = Uart::readByte(&uartReadResult);
                if (!uartReadResult) {
                    Serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                spcHal.write(port, data);
                Serial.write(SERIAL_WRITE_SUCCESS);
                break;
        }
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    spcHal.setPortPins(PORT_0_PIN, PORT_1_PIN);
    spcHal.setDataPins(DATA_0_PIN, DATA_1_PIN, DATA_2_PIN, DATA_3_PIN, DATA_4_PIN, DATA_5_PIN, DATA_6_PIN, DATA_7_PIN);
    serialWriteResult(iplRomClient.reset());
}

void loop() {
    handleSerialCommand();
}
