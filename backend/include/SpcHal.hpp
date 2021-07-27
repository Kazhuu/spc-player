#ifndef SPC_PLAYER_SPC_HAL_HPP
#define SPC_PLAYER_SPC_HAL_HPP

#include <stdint.h>

class SpcHal {
public:
    SpcHal();
    void reset();
    uint8_t read(uint8_t port);
    void write(uint8_t port, uint8_t value);

private:
    void dataDirection(bool output);
    void setPortNumber(uint8_t port);
};

#endif
