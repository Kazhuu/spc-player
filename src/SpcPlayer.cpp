#include "SpcPlayer.h"

#include "Arduino.h"

SpcPlayer::SpcPlayer(uint8_t readPin, uint8_t writePin, uint8_t resetPin) {
    pinMode(readPin, OUTPUT);
    pinMode(writePin, OUTPUT);
    pinMode(resetPin, OUTPUT);
}

void SpcPlayer::setDataPins(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
    pinMode(d0, INPUT);
    pinMode(d1, INPUT);
    pinMode(d2, INPUT);
    pinMode(d3, INPUT);
    pinMode(d4, INPUT);
    pinMode(d5, INPUT);
    pinMode(d6, INPUT);
    pinMode(d7, INPUT);
}

void SpcPlayer::setPortPins(uint8_t port0Pin, uint8_t port1Pin) {
    pinMode(port0Pin, OUTPUT);
    pinMode(port1Pin, OUTPUT);
}

uint8_t SpcPlayer::read(uint8_t port) {

}
