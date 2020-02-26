#include "Uart.hpp"
#include "Arduino.h"

static unsigned long previousMillis;

uint8_t Uart::readByte(int* error) {
    uint8_t result = 0;
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;

    do {
        currentMillis = millis();
        if (Serial.available()) {
            result = Serial.read();
            *error = UART_OK;
            break;
        }
        if (currentMillis - previousMillis > READ_TIMEOUT) {
            *error = UART_TIMEOUT_ERROR;
            break;
        }
    } while (true);
    return result;
};

uint16_t Uart::readShort(int* error) {
    uint16_t result = 0;
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;

    do {
        currentMillis = millis();
        if (Serial.available() >= 2) {
            result = Serial.read() << 8;
            result |= Serial.read();
            *error = UART_OK;
            break;
        }
        if (currentMillis - previousMillis > READ_TIMEOUT) {
            *error = UART_TIMEOUT_ERROR;
            break;
        }
    } while (true);
    return result;
};
