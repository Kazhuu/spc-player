#include "SpcBus.hpp"
#include "Arduino.h"

SpcBus::SpcBus(uint8_t readPin, uint8_t writePin, uint8_t resetPin) {
    mReadPin = readPin;
    mWritePin = writePin;
    mResetPin = resetPin;
    pinMode(readPin, OUTPUT);
    pinMode(writePin, OUTPUT);
    pinMode(resetPin, OUTPUT);
}

void SpcBus::setDataPins(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
    mDataPins[0] = d0;
    mDataPins[1] = d1;
    mDataPins[2] = d2;
    mDataPins[3] = d3;
    mDataPins[4] = d4;
    mDataPins[5] = d5;
    mDataPins[6] = d6;
    mDataPins[7] = d7;
    dataDirection(INPUT);
}

/**
 * Read, write and reset lines are all active low. So first put read and write
 * to high and last reset line to prevent accidental read or writes to the SPC.
 */
void SpcBus::setPortPins(uint8_t port0Pin, uint8_t port1Pin) {
    mPortPins[0] = port0Pin;
    mPortPins[1] = port1Pin;
    pinMode(port0Pin, OUTPUT);
    pinMode(port1Pin, OUTPUT);
    digitalWrite(mReadPin, HIGH);
    digitalWrite(mWritePin, HIGH);
    digitalWrite(mResetPin, HIGH);
}

void SpcBus::reset() {
    digitalWrite(mResetPin, LOW);
    digitalWrite(mResetPin, HIGH);
}

uint8_t SpcBus::read(uint8_t port) {
    uint8_t data = 0;

    // Set data lines as inputs.
    dataDirection(INPUT);
    // Set port number.
    digitalWrite(mPortPins[0], port & 0x01);
    digitalWrite(mPortPins[1], port & 0x02);
    // Set read line low and delay a bit for data to appear in data lines.
    digitalWrite(mReadPin, LOW);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    // Read eight data lines.
    for (int i = 0; i < 8; i++) {
        data |= (digitalRead(mDataPins[i]) << i);
    }
    // Set read line high again.
    digitalWrite(mReadPin, HIGH);

    return data;
}

void SpcBus::write(uint8_t port, uint8_t value) {
    // Set data lines as outputs.
    dataDirection(OUTPUT);
    // Set port.
    digitalWrite(mPortPins[0], port & 0x01);
    digitalWrite(mPortPins[1], port & 0x02);
    // Put value on data lines.
    for (int i = 0; i < 8; i++) {
        digitalWrite(mDataPins[i], value & (1 << i));
    }
    // Set write line low and delay a bit to give time for SPC to read it.
    digitalWrite(mWritePin, LOW);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    // Set write line high again.
    digitalWrite(mWritePin, HIGH);
}

void SpcBus::dataDirection(uint8_t mode) {
    pinMode(mDataPins[0], mode);
    pinMode(mDataPins[1], mode);
    pinMode(mDataPins[2], mode);
    pinMode(mDataPins[3], mode);
    pinMode(mDataPins[4], mode);
    pinMode(mDataPins[5], mode);
    pinMode(mDataPins[6], mode);
    pinMode(mDataPins[7], mode);
}
