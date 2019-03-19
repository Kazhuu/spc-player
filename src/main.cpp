#include "Arduino.h"
#include "SpcPlayer.h"

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


SpcPlayer spcPlyer(READ_PIN, WRITE_PIN, RESET_PIN);

uint8_t data[] = {
    0x3D,               // inc X
    0x3D,               // inc X
    0xD8, 0xF6,         // mov [0F6h], X
    0x5F, 0xC0, 0xFF    // jmp !$FFC0
};

void setup() {
    Serial.begin(9600);
    spcPlyer.setPortPins(PORT_0_PIN, PORT_1_PIN);
    spcPlyer.setDataPins(DATA_0_PIN, DATA_1_PIN, DATA_2_PIN, DATA_3_PIN, DATA_4_PIN, DATA_5_PIN, DATA_6_PIN, DATA_7_PIN);
    spcPlyer.init();
    spcPlyer.writeBlock(0x0002, data, sizeof(data));
    Serial.println("write done");
    spcPlyer.start(0x0002);
    Serial.println("spc start");
}

void loop() {
    for (int i = 0; i < 4; i++) {
        Serial.print("port");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(spcPlyer.read(i), HEX);
        delay(500);
    }
}
