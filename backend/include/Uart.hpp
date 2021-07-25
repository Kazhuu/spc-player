#ifndef SPC_PLAYER_UART_HPP
#define SPC_PLAYER_UART_HPP

#include <stdint.h>
#include "Arduino.h"
#include "Serial.hpp"

#define READ_TIMEOUT_MS 5000

class Uart {
public:
    static uint8_t readByte(Serial_t &serial, bool* success);
    static uint16_t readShort(Serial_t &serial, bool* success);
};

#endif
