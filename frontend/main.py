import argparse

from serial import Serial

from spc import Spc
from list_action import ListAction
from uart import Uart


boot_code = [
    # first two bytes of ram
    0x8F, 0x00, 0x00,  # mov (0x00) #imm
    0x8F, 0x00, 0x01,  # mov (0x01) #imm
    # stack pointer
    0xCD, 0x00,        # mov x, #imm
    0xBD,              # mov SP, x
    # push PWS to stack
    0xCD, 0x00,        # mov x, #imm
    0x4D,              # push x
    # A, X, Y registers
    0xEA, 0x00,        # mov a, #imm
    0xCD, 0x00,        # mov x, #imm
    0x8D, 0x00,        # mov y, #imm
    # restore PSW from stack
    0x8E,              # pop psw
    # jump to saved program counter
    0x5f, 0x00, 0x00   # jmp !a
]

parser = argparse.ArgumentParser(
    description='Program to upload a SNES SPC music file to the original sound module via Arduino over serial'
)
parser.add_argument('serial_port', help='Serial port to use. Usually COM1 on Windows or /dev/ttyUSB0 on Linux')
parser.add_argument('spc_file', help='Path to the SPC sound file')
parser.add_argument("-l", "--list", help="Print available serial ports", action=ListAction)
args = parser.parse_args()

with open(args.spc_file, 'rb') as f:
    spc = Spc(f)
    with Serial(args.serial_port, 115200, timeout=3) as serial:
        Uart.reset(serial)
        print('opened port {0}'.format(serial.name))
        print('ram')
        Uart.write_block(serial, 0x0100, spc.ram[0x0100:0xFFC0])
        print('page 0')
        Uart.write_block(serial, 0x0002, spc.ram[0x0002:0x00F0])
        print('dsp registers')
        Uart.write_dsp_registers(serial, 0, spc.dsp_registers)

        # Prepare boot code.
        boot_code[1] = spc.ram[0]
        boot_code[4] = spc.ram[1]
        boot_code[7] = spc.stack_pointer
        boot_code[10] = spc.program_status_word
        boot_code[13] = spc.a_register
        boot_code[15] = spc.x_register
        boot_code[17] = spc.y_register
        boot_code[20] = spc.program_couter[1]
        boot_code[21] = spc.program_couter[0]
        boot_code_address = 0xFFC0 - len(boot_code)

        Uart.write_block(serial, 0x00FA, spc.ram[0x00FA:0x00FD])
        print('timers')
        Uart.write_block(serial, boot_code_address, boot_code)
        print('boot code')
        # Uart.write_block(serial, 0x00F1, [0x07])
        Uart.start(serial, boot_code_address)
        print('started execution from: 0x{:02x}{:02x}'.format(spc.program_couter[0], spc.program_couter[1]))
        print(Uart.read_ports(serial))
