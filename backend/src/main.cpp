#include "Arduino.h"
#include "SpcPlayer.hpp"
#include "SpcHal.hpp"
#include "IplRomClient.hpp"
#include "Uart.hpp"
#include "Serial.hpp"

#ifdef USE_USB_SERIAL
    #include <WebUSB.h>
    Serial_t webUsbSerial(0 /* http:// */, "localhost:8080");
    #define serial webUsbSerial
#else
    #define serial Serial
#endif

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

#define SERIAL_WRITE_SUCCESS '1'
#define SERIAL_WRITE_ERROR '0'

SpcHal spcHal(READ_PIN, WRITE_PIN, RESET_PIN);
IplRomClient iplRomClient(spcHal);
SpcPlayer spcPlayer(iplRomClient);

void readPorts() {
    for (uint8_t i = 0; i < 4; ++i) {
        serial.write(spcHal.read(i));
    }
}

void serialWriteResult(bool result) {
    if (result) {
        serial.write(SERIAL_WRITE_SUCCESS);
    } else {
        serial.write(SERIAL_WRITE_ERROR);
    }
}

void handleSerialCommand() {
    if (serial.available()) {
        char result = serial.read();
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
                programCounter = Uart::readShort(serial, &uartReadResult);
                if (!uartReadResult) {
                    serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                aRegister = Uart::readByte(serial, &uartReadResult);
                if (!uartReadResult) {
                    serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                xRegister = Uart::readByte(serial, &uartReadResult);
                if (!uartReadResult) {
                    serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                yRegister = Uart::readByte(serial, &uartReadResult);
                if (!uartReadResult) {
                    serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                stackPointer = Uart::readByte(serial, &uartReadResult);
                if (!uartReadResult) {
                    serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                cpuFlags = Uart::readByte(serial, &uartReadResult);
                if (!uartReadResult) {
                    serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                spcPlayer.writeCpuRegisters(programCounter, aRegister, xRegister, yRegister, stackPointer, cpuFlags);
                serial.write(SERIAL_WRITE_SUCCESS);
                break;

            // Write all 128 DSP register. One byte per register.
            case 'D':
                for (uint32_t i = 0; i < 128; ++i) {
                    page[i] = Uart::readByte(serial, &uartReadResult);
                    if (!uartReadResult) {
                        serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (uartReadResult) {
                    bool dspResult = spcPlayer.writeDspRegisters(page);
                    if (dspResult) {
                        serial.write(SERIAL_WRITE_SUCCESS);
                    } else {
                        serial.write(SERIAL_WRITE_ERROR);
                    }
                }
                break;

            // Write page 0 of SPC ram, 256 bytes.
            case '0':
                for (uint32_t i = 0; i < 256; ++i) {
                    page[i] = Uart::readByte(serial, &uartReadResult);
                    if (!uartReadResult) {
                        serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (uartReadResult) {
                    bool firstPageResult = spcPlayer.writeFirstPageRam(page);
                    if (firstPageResult) {
                        serial.write(SERIAL_WRITE_SUCCESS);
                    } else {
                        serial.write(SERIAL_WRITE_ERROR);
                    }
                }
                break;

            // Write page 1 of SPC ram, 256 bytes.
            case '1':
                for (uint32_t i = 0; i < 256; ++i) {
                    page[i] = Uart::readByte(serial, &uartReadResult);
                    if (!uartReadResult) {
                        serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (uartReadResult) {
                    bool secondPageResult = spcPlayer.writeSecondPageRam(page);
                    if  (secondPageResult) {
                        serial.write(SERIAL_WRITE_SUCCESS);
                    } else {
                        serial.write(SERIAL_WRITE_ERROR);
                    }
                }
                break;

            // Write rest of the ram bytes after page 1. Needs to followed by 64960
            // bytes, will only then be successful.
            case '2':
                for (uint32_t i = 0; i < 0xFFC0 - 0x200; ++i) {
                    value = Uart::readByte(serial, &uartReadResult);
                    if (!uartReadResult) {
                        spcPlayer.resetRamWrite();
                        serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                    ramWrittenAmount = spcPlayer.writeRamByte(value);
                    if (ramWrittenAmount == 0) {
                        spcPlayer.resetRamWrite();
                        serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (ramWrittenAmount == 0xFFC0 - 0x200) {
                    serial.write(SERIAL_WRITE_SUCCESS);
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
                port = Uart::readByte(serial, &uartReadResult);
                if (!uartReadResult) {
                    serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                value = Uart::readByte(serial, &uartReadResult);
                if (!uartReadResult) {
                    serial.write(SERIAL_WRITE_ERROR);
                    break;
                }
                spcHal.write(port, value);
                serial.write(SERIAL_WRITE_SUCCESS);
                break;

            // Read boot code by first writing it's size in one byte and
            // followed by boot code amount of size.
            case 'B':
                bootCodePointer = spcPlayer.getBootCode(bootCodeSize);
                serial.write(bootCodeSize);
                // Flush here with webusb makes the bootcode size and bootcode
                // as separate packets.
                serial.flush();
                serial.write(bootCodePointer, bootCodeSize);
                break;
        }
    }
}

void setup() {
    while (!serial) {
        ;
    }
    serial.begin(BAUD_RATE);
    spcHal.setPortPins(PORT_0_PIN, PORT_1_PIN);
    spcHal.setDataPins(DATA_0_PIN, DATA_1_PIN, DATA_2_PIN, DATA_3_PIN, DATA_4_PIN, DATA_5_PIN, DATA_6_PIN, DATA_7_PIN);
}

void loop() {
    handleSerialCommand();
    serial.flush();
}
