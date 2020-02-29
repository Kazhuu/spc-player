- From PC upload 128 bytes of DSP registers and 256 bytes of ram first page to
    Arduino. Last byte of ram first page is modified to include stack pointer value
    from SPC file. This byte in ram is meant for timer counter 3 read only
    value. This is used to transfer stack pointer silently to SPC. Stack pointer
    is used to transfer some values to stack which at the end custom boot code
    can pop.
- Arduino code has custom upload code which will be modified to include earlier
    uploaded timer 1, 2 and 3 values and stack pointer value before uploading it
    to the SPC ram.
- Arduino uploads custom upload code to SPC ram and transfers 128 bytes if DSP
    registers with it. After this custom code writes timer 1, 2, 3 values and
    stack pointer, then jumping to IPL ROM execution just before it puts AA and
    BB on port 0 and port 1 consecutively (address of 0xFFC9). It does this by
    underflowing program counter which will but it to IPL ROM area at end of the
    ram. Stack pointer will be at 0xFFC8 and execution will start at 0xFFC9.
- After uploading custom code it will read port 0 for DSP address register and
    port 1 for DSP data register. This will continue until all 128 bytes are
    written. Two registers are modified during the write process to mute voices
    and disable DSP echo feature. This done by writing address 0x4C to 0x00 and
    address 0x6C to 0x60. After DSP registers code will write timer 1, 2 and 3
    values and stack pointer. Then it will branch to IPL ROM boot code.
- Arduino will upload first page of ram with IPL ROM upload code starting from
    0x0002 to 0x00EF. This will also overwrite custom upload code at the
    beginning of the ram written earlier.
- PC will transfer 32 bytes of ram to Arduino. Arduino will write these bytes
    starting from 0x100 until 0x120. After this 160 bytes of data is skipped. In
    Super Mario World's case these bytes all are of value 0xFF. These bytes are
    written to stack area. Also in this chunk write modified stack variables
    starting from 0x1c9 which will be popped by custom boot code. Values are
    aligned so that when popped original stack pointer value will remain.
- PC will send 576 bytes of ram to Arduino. Arduino will write these bytes to
    SCP starting from address of 0x1c0. This will include top of the stack area
    and little bit over that until address 0x0400.
- Again skip all next 0xFF bytes and start writing ram data from address 0x500.
- Continue like this until all ram is written before IPL ROM code.
- Write custom boot loader code of 48 bytes just before IPL ROM.
- Jump to boot code execution.
Boot code:
0x8F 0x00 0x00  ;    MOV [0x00], #0x00 restore 0x0000 byte
0x8F 0x00 0x01  ;    MOV [0x01], #0x00 restore 0x0001 byte
0x8F 0xB0 0xF1  ;    MOV [0xF1], #0xB0 restore control byte
0xCD 0x53       ;    MOV X, #0x53      acknowledge byte
0xD8 0xF4       ;    MOV [0xF4], X     X -> port 0
0xE4 0xF4       ;L1: MOV A, [0xF4]     port 0 -> A
0x68 0x01       ;    CMP A, #0x01      compare A and 0x01
0xD0 0xFA       ;    BNE LO            branch to L1 if A is not equal to 0x01
0xE4 0xF7       ;L2: MOV A, [0xF7]     port 3 -> A
0x68 0x00       ;    CMP A, #0x00      compare A and 0x00
0xD0 0xFA       ;    BNE A, LO2        branch to L2 if A is not equal to 0x00
0x8F 0x6C 0xF2  ;    MOV [0xF2], #0x6C 0x6C -> DSP register address
0x8F 0x20 0xF3  ;    MOV [0xF3], #0x20 0x20 -> DSP register data, restore original flag register
0x8F 0x4C 0xF2  ;    MOV [0xF2], #0x4C 0x4C -> DSP register address
0x8F 0x0F 0xF3  ;    MOV [0xF3], #0x0F 0x0F -> DSP register data, restore original key on register
0x8F 0x4C 0xF2  ;    MOV [0xF2], #0x4C 0x4C -> DSP register address, restore original register value
0xAE            ;    POP A             restore A from stack
0xCE            ;    POP X             restore X from stack
0xEE            ;    POP Y             restore Y from stack
0x7F            ;    RETI              return and restore PSW (1 byte) and then PC (2 bytes)


Planned UART API calls. Every UART code return 1 on success and 0 on error.
- CPU registers (7 bytes)
- DSP registers (128 bytes)
- 0-page (256 bytes)
- 1-page (256 bytes)
- rest of the ram (64 960 bytes)
- bootloader location, if not called defaults to bottom of the stack 0x0100
- command to write single port
- command to read all ports
- command to reset SPC
