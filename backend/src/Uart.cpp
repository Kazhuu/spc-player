#include "Uart.hpp"
#include "Arduino.h"

static unsigned long previousMillis;

uint8_t Uart::readByte(bool* success) {
    uint8_t result = 0;
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;

    while (true) {
        currentMillis = millis();
        if (Serial.available()) {
            result = Serial.read();
            *success = true;
            break;
        }
        if (currentMillis - previousMillis > READ_TIMEOUT) {
            *success = false;
            break;
        }
    }
    return result;
};

uint16_t Uart::readShort(bool* success) {
    uint16_t result = 0;
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;

    while (true) {
        currentMillis = millis();
        if (Serial.available() >= 2) {
            result = Serial.read() << 8;
            result |= Serial.read();
            *success = true;
            break;
        }
        if (currentMillis - previousMillis > READ_TIMEOUT) {
            *success = false;
            break;
        }
    }
    return result;
};
