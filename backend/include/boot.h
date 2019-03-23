#ifndef SPC_PLAYER_BOOT_H
#define SPC_PLAYER_BOOT_H

uint8_t boot_code[] = {
    0x8F, 0x00, 0x00, // mov (0x00) #lower_byte
    0x8F, 0x00, 0x01  // mov (0x01) #higher_byte
    // Continue and add rest of the boot code here.
};

#endif
