import argparse
import timeit

from serial import Serial

from spc import Spc
from list_action import ListAction
from uart import Uart


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Program to upload a SNES SPC music file to the original sound module via Arduino over serial'
    )
    parser.add_argument('serial_port', help='Serial port to use. Usually COM1 on Windows or /dev/ttyUSB0 on Linux')
    parser.add_argument('spc_file', help='Path to the SPC sound file')
    parser.add_argument("-l", "--list", help="Print available serial ports", action=ListAction)
    args = parser.parse_args()

    with open(args.spc_file, 'rb') as f:
        spc = Spc(f)
        with Serial(args.serial_port, 38400, timeout=3) as serial:
            Uart.reset(serial)
            start_time = timeit.default_timer()
            print('opened port {0}'.format(serial.name))
            Uart.write_cpu_registers(serial, spc.program_couter, spc.a_register, spc.x_register, spc.y_register, spc.stack_pointer, spc.program_status_word)
            print('write CPU registers successful')
            Uart.write_dsp_registers(serial, spc.dsp_registers)
            print('write DSP registers successful')
            Uart.write_first_page_ram(serial, spc.first_page_ram)
            print('write first page RAM successful')
            Uart.write_second_page_ram(serial, spc.second_page_ram)
            print('write second page RAM successful')
            Uart.write_rest_of_the_ram(serial, spc.rest_of_the_ram)
            print('write rest of the RAM successful')
            Uart.start(serial)
            print('SPC exexution started successfully, uploading took {0}s'.format(round(timeit.default_timer() - start_time, 2)))
