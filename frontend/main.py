import argparse

from serial import Serial

from spc import Spc
from list_action import ListAction
from uart import Uart
from exceptions import SpcExpection

data = [
    0x3D,               # inc X
    0x3D,               # inc X
    0xD8, 0xF6,         # mov [0F6h], X
    0x5F, 0xC0, 0xFF    # jmp !$FFC0
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
        print(Uart.read_ports(serial))
        # Uart.write_dsp_registers(serial, spc.dsp_registers)
        print(Uart.read_ports(serial))
        Uart.write_block(serial, 0x00FF, spc.ram_trimmed())
        # Uart.write_block(serial, 0x0002, data)
        # print(Uart.read_ports(serial))
        # Uart.start(serial, 0x0002)
        # print(Uart.read_ports(serial))
