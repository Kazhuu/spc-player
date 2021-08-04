#include "Arduino.h"
#include "SpcPlayer.hpp"
#include "SpcHal.hpp"
#include "IplRomClient.hpp"
#include "Uart.hpp"
#include "Serial.hpp"

#ifdef USE_WEBUSB_SERIAL
    #include <WebUSB.h>
    Serial_t webUsbSerial(1 /* https:// */, "kazhuu.github.io/spc-player");
    #define serial webUsbSerial
#else
    #define serial Serial
#endif

#define BAUD_RATE 38400

#define SERIAL_WRITE_SUCCESS '1'
#define SERIAL_WRITE_ERROR '0'

// Maximum transfer packet one time. Must be bigger than 256 and must be even
// divisor of 64960. Which correspond to ram size in bytes which is transferred
// last.
#define RAM_PACKET_MAX_SIZE 1015

SpcHal spcHal;
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
        bool packetResult;
        uint8_t* bootCodePointer;
        uint32_t bootCodeSize;
        uint8_t value;
        uint8_t port;
        uint16_t programCounter;
        uint8_t aRegister, xRegister, yRegister, stackPointer, cpuFlags;
        uint8_t packet[RAM_PACKET_MAX_SIZE];

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
                    packet[i] = Uart::readByte(serial, &uartReadResult);
                    if (!uartReadResult) {
                        serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (uartReadResult) {
                    bool dspResult = spcPlayer.writeDspRegisters(packet);
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
                    packet[i] = Uart::readByte(serial, &uartReadResult);
                    if (!uartReadResult) {
                        serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (uartReadResult) {
                    bool firstPageResult = spcPlayer.writeFirstPageRam(packet);
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
                    packet[i] = Uart::readByte(serial, &uartReadResult);
                    if (!uartReadResult) {
                        serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                }
                if (uartReadResult) {
                    bool secondPageResult = spcPlayer.writeSecondPageRam(packet);
                    if  (secondPageResult) {
                        serial.write(SERIAL_WRITE_SUCCESS);
                    } else {
                        serial.write(SERIAL_WRITE_ERROR);
                    }
                }
                break;

            // Write rest of the ram bytes after page 1. Rest of the ram is
            // 64960 bytes long from address 0x0200 to 0xFFC0. Transfer this
            // amount of bytes in 64960 / RAM_PACKET_MAX_SIZE packets and each
            // RAM_PACKET_MAX_SIZE bytes in length. Between each packet
            // transfer success or error result.
            case '2':
                for (uint32_t i = 0; i < (0xFFC0 - 0x200) / RAM_PACKET_MAX_SIZE; ++i) {
                    uartReadResult = false;
                    for (uint32_t j = 0; j < RAM_PACKET_MAX_SIZE; ++j) {
                        packet[j] = Uart::readByte(serial, &uartReadResult);
                        if (!uartReadResult) {
                            break;
                        }
                    }
                    if (!uartReadResult) {
                        spcPlayer.resetRamWrite();
                        serial.write(SERIAL_WRITE_ERROR);
                        break;
                    }
                    packetResult = spcPlayer.writeRamPacket(packet, RAM_PACKET_MAX_SIZE);
                    if (!packetResult) {
                        spcPlayer.resetRamWrite();
                        serial.write(SERIAL_WRITE_ERROR);
                        break;
                    } else {
                        serial.write(SERIAL_WRITE_SUCCESS);
                    }
                    // Flush here with WebUSB makes each iteration different
                    // transfer packet.
                    serial.flush();
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
                // Flush here with WebUSB makes the bootcode size and bootcode
                // as separate packets.
                serial.flush();
                serial.write(bootCodePointer, bootCodeSize);
                break;
        }
    }
    serial.flush();
}

void setup() {
    while (!serial) {
        ;
    }
    serial.begin(BAUD_RATE);
}

void loop() {
    handleSerialCommand();
}
