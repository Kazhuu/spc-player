#include "SpcHal.hpp"
#include "IplRomClient.hpp"
#include "Uart.hpp"
#include "Arduino.h"

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


void readPorts() {
    for (uint8_t i = 0; i < 4; i++) {
        Serial.write(spcHal.read(i));
    }
}

void serialWriteResult(bool result) {
    if (result) {
        Serial.write('1');
    } else {
        Serial.write('0');
    }
}

void handleSerialCommand() {
    if (Serial.available()) {
        char result = Serial.read();
        int error;
        uint16_t ramAddress;
        uint16_t length;
        uint8_t dspAddress;
        uint8_t dspLength;
        uint8_t data;
        uint8_t port;

        switch (result) {
            case 'Q': // Read value of all four ports.
                readPorts();
                break;

            case 'R': // Reset SPC.
                serialWriteResult(iplRomClient.reset());
                break;

            case 'S': // Start SPC execution from given ram address.
                ramAddress = Uart::readShort(&error);
                if (error)
                    break;
                iplRomClient.start(ramAddress);
                Serial.write((uint8_t)(ramAddress >> 8));
                Serial.write((uint8_t)(ramAddress & 0xff));
                break;

            // Write DSP registers starting from given address within DSP and
            // amount of given length.
            case 'D':
                dspAddress = Uart::readByte(&error);
                if (error)
                    break;
                Serial.write(dspAddress);
                dspLength = Uart::readByte(&error);
                if (error)
                    break;
                Serial.write(dspLength);
                for (uint8_t i = dspAddress; i < dspAddress + dspLength; i++) {
                    data = Uart::readByte(&error);
                    if (error)
                        break;
                    iplRomClient.setAddress(0x00F2);
                    iplRomClient.write(i); // Write DSP register address
                    iplRomClient.write(data); // Write DSP register value
                    Serial.write(data);
                }
                break;

            // Write block of bytes to ram from given address and amount of
            // given length.
            case 'B':
                ramAddress = Uart::readShort(&error);
                if (error)
                    break;
                Serial.write((uint8_t)(ramAddress >> 8));
                Serial.write((uint8_t)(ramAddress & 0xff));
                length = Uart::readShort(&error);
                if (error)
                    break;
                Serial.write((uint8_t)(length >> 8));
                Serial.write((uint8_t)(length & 0xff));
                iplRomClient.setAddress(ramAddress);
                for (unsigned int i = 0; i < length; i++) {
                    data = Uart::readByte(&error);
                    if (error)
                        break;
                    iplRomClient.write(data);
                }
                Serial.write((uint8_t)(length >> 8));
                Serial.write((uint8_t)(length & 0xff));
                break;

             // Write single port. Two following bytes are read for address and
             // value.
            case 'W':
                port = Uart::readByte(&error);
                if (error) {
                    Serial.write(0x00);
                    break;
                }
                data = Uart::readByte(&error);
                if (error) {
                    Serial.write(0x00);
                    break;
                }
                spcHal.write(port, data);
                Serial.write(0x01);
                break;
        }
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    spcHal.setPortPins(PORT_0_PIN, PORT_1_PIN);
    spcHal.setDataPins(DATA_0_PIN, DATA_1_PIN, DATA_2_PIN, DATA_3_PIN, DATA_4_PIN, DATA_5_PIN, DATA_6_PIN, DATA_7_PIN);
    iplRomClient.reset();
}

void loop() {
    handleSerialCommand();
}
