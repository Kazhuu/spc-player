#ifndef SPC_PLAYER_UART_HPP
#define SPC_PLAYER_UART_HPP

#include <stdint.h>

#define UART_OK             0
#define UART_TIMEOUT_ERROR  1

#define READ_TIMEOUT 500

class Uart {
public:
    static uint8_t readByte(int* error);
    static uint16_t readShort(int* error);
};

#endif
