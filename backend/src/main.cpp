#include "Arduino.h"
#include "spc_bus.h"
#include "spc_writer.h"
#include "uart.h"

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



SpcBus spcBus(READ_PIN, WRITE_PIN, RESET_PIN);
SpcWriter spcWriter(spcBus);


uint8_t data[] = {
    0x3D,               // inc X
    0x3D,               // inc X
    0xD8, 0xF6,         // mov [0F6h], X
    0x5F, 0xC0, 0xFF    // jmp !$FFC0
};

void readPorts() {
    for (int i = 0; i < 4; i++) {
        Serial.print("port");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(spcBus.read(i), HEX);
    }
}

void handleSerialCommand() {
    if (Serial.available()) {
        char result = Serial.read();
        switch (result) {
            case 'Q':
                readPorts();
                break;

            case 'R':
                spcWriter.reset();
                Serial.write('R');
                break;

            case 'B':
                int error;
                uint16_t address = Uart::readShort(&error);
                if (error) {
                    Serial.write("ae");
                    break;
                }
                Serial.write((uint8_t)(address >> 8));
                Serial.write((uint8_t)(address & 0xff));
                uint16_t length = Uart::readShort(&error);
                if (error) {
                    break;
                    Serial.write("le");
                }
                Serial.write((uint8_t)(length >> 8));
                Serial.write((uint8_t)(length & 0xff));
                break;
        }
    }
}

void setup() {
    Serial.begin(9600);
    spcBus.setPortPins(PORT_0_PIN, PORT_1_PIN);
    spcBus.setDataPins(DATA_0_PIN, DATA_1_PIN, DATA_2_PIN, DATA_3_PIN, DATA_4_PIN, DATA_5_PIN, DATA_6_PIN, DATA_7_PIN);
    spcWriter.reset();
    Serial.write('R');
}

void loop() {
    handleSerialCommand();
}
