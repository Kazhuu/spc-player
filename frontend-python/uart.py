import sys

from exceptions import SpcExpection


class Uart:

    @classmethod
    def write_cpu_registers(cls, serial, program_counter, a, x, y, stack_pointer, program_status_word):
        serial.write(b'C');
        serial.write(program_counter);
        serial.write(cls.int_to_byte(a));
        serial.write(cls.int_to_byte(x));
        serial.write(cls.int_to_byte(y));
        serial.write(cls.int_to_byte(stack_pointer));
        serial.write(cls.int_to_byte(program_status_word));
        result = serial.read()
        if result != b'1':
            raise SpcExpection('writing CPU registers failed')

    @classmethod
    def write_dsp_registers(cls, serial, dsp_registers):
        serial.write(b'D')
        for index, value in enumerate(dsp_registers):
            data = cls.int_to_byte(value)
            serial.write(data)
        result = serial.read()
        if result != b'1':
            raise SpcExpection('writing DSP registers failed')

    @classmethod
    def write_first_page_ram(cls, serial, first_page_ram):
        serial.write(b'0')
        for index, value in enumerate(first_page_ram):
            data = cls.int_to_byte(value)
            serial.write(data)
        result = serial.read()
        if result != b'1':
            raise SpcExpection('writing first page ram failed')

    @classmethod
    def write_second_page_ram(cls, serial, second_page_ram):
        serial.write(b'1')
        for index, value in enumerate(second_page_ram):
            data = cls.int_to_byte(value)
            serial.write(data)
        result = serial.read()
        if result != b'1':
            raise SpcExpection('writing second page ram failed')

    @classmethod
    def write_rest_of_the_ram(cls, serial, rest_of_the_ram):
        serial.write(b'2')
        packet_size = 232
        packet_count = int(len(rest_of_the_ram) / packet_size)
        for i in range(packet_count):
            packet_index_start = i * packet_size
            packet_index_end = packet_index_start + packet_size
            for value in rest_of_the_ram[packet_index_start:packet_index_end]:
                data = cls.int_to_byte(value)
                serial.write(data)
            result = serial.read()
            if result != b'1':
                raise SpcExpection('writing ram packet {0} failed'.format(i))
            sys.stdout.write('\r%d%%' % ((i + 1) / packet_count * 100))
            sys.stdout.flush()
            sys.stdout.write('\n')

    @classmethod
    def read_ports(cls, serial):
        serial.write(b'Q')
        data = []
        for i in range(4):
            data.append(serial.read())
        return data

    @classmethod
    def write_port(cls, serial, port, value):
        serial.write(b'Q')
        serial.write(cls.int_to_byte(port))
        serial.write(cls.int_to_byte(value))

    @classmethod
    def start(cls, serial):
        serial.write(b'S')
        result = serial.read()
        if result != b'1':
            raise SpcExpection('starting SPC execution failed with error code {0}'.format(result))

    @classmethod
    def readBootCode(cls, serial):
        serial.write(b'B')
        size = cls.int_from_bytes(serial.read(1))
        code = serial.read(size)
        return code

    @classmethod
    def reset(cls, serial):
        serial.write(b'R')
        result = serial.read()
        if result != b'1':
            raise SpcExpection('SPC reset timed out')

    @classmethod
    def int_to_byte(cls, value):
        if type(value) == bytes:
            return value
        return value.to_bytes(1, 'big')

    @classmethod
    def int_to_short(cls, value):
        if type(value) == bytes:
            return value
        return value.to_bytes(2, 'big')

    @classmethod
    def int_from_bytes(cls, value):
        return int.from_bytes(value, 'big')
