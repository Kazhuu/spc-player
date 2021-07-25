#include "Uart.hpp"

static unsigned long previousMillis;

uint8_t Uart::readByte(Serial_t &serial, bool* success) {
    uint8_t result = 0;
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;

    while (true) {
        currentMillis = millis();
        if (serial.available()) {
            result = serial.read();
            *success = true;
            break;
        }
        if (currentMillis - previousMillis > READ_TIMEOUT_MS) {
            *success = false;
            break;
        }
    }
    return result;
};

uint16_t Uart::readShort(Serial_t &serial, bool* success) {
    uint16_t result = 0;
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;

    while (true) {
        currentMillis = millis();
        if (serial.available() >= 2) {
            result = serial.read() << 8;
            result |= serial.read();
            *success = true;
            break;
        }
        if (currentMillis - previousMillis > READ_TIMEOUT_MS) {
            *success = false;
            break;
        }
    }
    return result;
};
