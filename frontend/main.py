import argparse

from serial import Serial

from spc import Spc
from list_action import ListAction
from uart import Uart

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
    with Serial(args.serial_port, timeout=3) as serial:
        Uart.reset(serial)
        print('opened port {0}'.format(serial.name))
        print(Uart.readPorts(serial))
        Uart.writeBlock(serial, 0x0002, data)
        print(Uart.readPorts(serial))
        Uart.start(serial, 0x0002)
        print(Uart.readPorts(serial))
