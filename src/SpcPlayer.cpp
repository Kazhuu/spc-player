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
    this->_setReadMode();
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

}

void SpcPlayer::reset() {
    digitalWrite(this->_resetPin, LOW);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    digitalWrite(this->_resetPin, HIGH);
}

uint8_t SpcPlayer::read(uint8_t port) {
    uint8_t data = 0;

    // Set port number.
    digitalWrite(this->_portPins[0], port & 0x01);
    digitalWrite(this->_portPins[1], port & 0x02);
    // Set read line low and delay a bit for data to appear in data pins.
    digitalWrite(this->_readPin, LOW);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    // Read eight data pins.
    for (int i = 0; i < 8; i++) {
        data |= (digitalRead(this->_dataPins[i]) << i);
    }
    // Set read line high again.
    digitalWrite(this->_readPin, HIGH);

    return data;
}

void SpcPlayer::_setReadMode() {
    this->_dataDirection(INPUT);
}

void SpcPlayer::_setWriteMode() {
    this->_dataDirection(OUTPUT);
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
