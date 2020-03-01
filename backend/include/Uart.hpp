#ifndef SPC_PLAYER_UART_HPP
#define SPC_PLAYER_UART_HPP

#include <stdint.h>

#define READ_TIMEOUT 500

class Uart {
public:
    static uint8_t readByte(bool* success);
    static uint16_t readShort(bool* success);
};

#endif
