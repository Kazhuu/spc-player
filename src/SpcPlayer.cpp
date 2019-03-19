#include "SpcPlayer.h"

#include "Arduino.h"

SpcPlayer::SpcPlayer(uint8_t readPin, uint8_t writePin, uint8_t resetPin) {
    this->_readPin = readPin;
    this->_writePin = writePin;
    this->_resetPin = resetPin;
    pinMode(readPin, OUTPUT);
    pinMode(writePin, OUTPUT);
    pinMode(resetPin, OUTPUT);
}

void SpcPlayer::setDataPins(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
    this->_dataPins[0] = d0;
    this->_dataPins[1] = d1;
    this->_dataPins[2] = d2;
    this->_dataPins[3] = d3;
    this->_dataPins[4] = d4;
    this->_dataPins[5] = d5;
    this->_dataPins[6] = d6;
    this->_dataPins[7] = d7;
    this->_dataDirection(INPUT);
}

void SpcPlayer::setPortPins(uint8_t port0Pin, uint8_t port1Pin) {
    this->_portPins[0] = port0Pin;
    this->_portPins[1] = port1Pin;
    pinMode(port0Pin, OUTPUT);
    pinMode(port1Pin, OUTPUT);
}

/**
 * Read, write and reset lines are all active low. So first put read and write
 * to high and last reset line to prevent accidental read or writes to the SPC.
 */
void SpcPlayer::init() {
    digitalWrite(this->_readPin, HIGH);
    digitalWrite(this->_writePin, HIGH);
    digitalWrite(this->_resetPin, HIGH);
    this->_firstTransfer = true;
    // Wait for SPC to be ready.
    while (this->read(0) != 0xAA);
    while (this->read(1) != 0xBB);
}

void SpcPlayer::reset() {
    digitalWrite(this->_resetPin, LOW);
    digitalWrite(this->_resetPin, HIGH);
    this->_firstTransfer = true;
}

uint8_t SpcPlayer::read(uint8_t port) {
    uint8_t data = 0;

    // Set data lines as inputs.
    this->_dataDirection(INPUT);
    // Set port number.
    digitalWrite(this->_portPins[0], port & 0x01);
    digitalWrite(this->_portPins[1], port & 0x02);
    // Set read line low and delay a bit for data to appear in data lines.
    digitalWrite(this->_readPin, LOW);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    // Read eight data lines.
    for (int i = 0; i < 8; i++) {
        data |= (digitalRead(this->_dataPins[i]) << i);
    }
    // Set read line high again.
    digitalWrite(this->_readPin, HIGH);

    return data;
}

void SpcPlayer::write(uint8_t port, uint8_t value) {
    // Set data lines as outputs.
    this->_dataDirection(OUTPUT);
    // Set port.
    digitalWrite(this->_portPins[0], port & 0x01);
    digitalWrite(this->_portPins[1], port & 0x02);
    // Put value on data lines.
    for (int i = 0; i < 8; i++) {
        digitalWrite(this->_dataPins[i], value & (1 << i));
    }
    // Set write line low and delay a bit to give time for SPC to read it.
    digitalWrite(this->_writePin, LOW);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    // Set write line high again.
    digitalWrite(this->_writePin, HIGH);
}

void SpcPlayer::writeBlock(uint16_t address, uint8_t* data, int length) {
    this->write(1, 1);
    this->write(2, address & 0xFF);
    this->write(3, address >> 8);
    if (this->_firstTransfer) {
        this->write(0, 0xCC);
        while (this->read(0) != 0xCC);
        this->_firstTransfer = false;
    } else {
        uint8_t lastValue = this->read(0) + 2;
        // Next value cannot be zero, so add 1 or more.
        if (lastValue == 0)
            lastValue = 1;
        this->write(0, lastValue);
        while (this->read(0) != lastValue);
    }
    for (int i = 0; i < length; i++) {
        this->write(1, data[i]);
        this->write(0, (i & 0xFF));
        while (this->read(0) != (i & 0xFF));
    };
}

void SpcPlayer::start(uint16_t address) {
    // Port 0 value needs to be incremented by 2 or more to tell SPC to start
    // execution. If first time then 0xCC.
    uint8_t lastValue = this->read(0) + 2;
    if (this->_firstTransfer) {
        lastValue = 0xCC;
        this->_firstTransfer = false;
    }
    // Write execution address and wait for response.
    this->write(1, 0x00);
    this->write(2, address & 0xFF);
    this->write(3, address >> 8);
    this->write(0, lastValue);
    while (this->read(0) != lastValue);
}

void SpcPlayer::_dataDirection(uint8_t mode) {
    pinMode(this->_dataPins[0], mode);
    pinMode(this->_dataPins[1], mode);
    pinMode(this->_dataPins[2], mode);
    pinMode(this->_dataPins[3], mode);
    pinMode(this->_dataPins[4], mode);
    pinMode(this->_dataPins[5], mode);
    pinMode(this->_dataPins[6], mode);
    pinMode(this->_dataPins[7], mode);
}
