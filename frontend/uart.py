from exceptions import SpcExpection


class Uart:

    @classmethod
    def write_block(cls, serial, address, data):
        serial.write(b'B')
        # address
        address = cls.int_to_short(address)
        serial.write(address)
        result = serial.read(2)
        if result != address:
            raise SpcExpection('writing block address timed out')
        # length
        length = cls.int_to_short(len(data))
        serial.write(length)
        result = serial.read(2)
        if result != length:
            raise SpcExpection('writing block length timed out')
        # data
        for index, value in enumerate(data):
            byte = cls.int_to_byte(value)
            serial.write(byte)
            print('writing {} address 0x{:04x}'.format(index + 1, index + cls.int_from_bytes(address)))
        result = serial.read(2)
        if result != length:
            raise SpcExpection(
                'writing block to address 0x{:04x} timed out'.format(cls.int_from_bytes(address))
            )

    @classmethod
    def write_dsp_registers(cls, serial, dsp_registers):
        if len(dsp_registers) != 128:
            raise SpcExpection('dsp_registers length must be equal to 128, now {0}'.format(len(dsp_registers)))
        serial.write(b'D')
        for index, value in enumerate(dsp_registers):
            data = cls.int_to_byte(value)
            serial.write(data)
            result = serial.read()
            if result != data:
                raise SpcExpection('writing {0} of DSP registers timed out'.format(index))

    @classmethod
    def read_ports(cls, serial):
        serial.write(b'Q')
        data = []
        for i in range(4):
            data.append(serial.read())
        return data

    @classmethod
    def start(cls, serial, address):
        serial.write(b'S')
        address = cls.int_to_short(address)
        serial.write(address)
        result = serial.read(2)
        if result != address:
            raise SpcExpection('starting SPC execution timed out')

    @classmethod
    def reset(cls, serial):
        serial.write(b'R')
        result = serial.read()
        if result != b'R':
            raise SpcExpection('SPC reset failed timed out')

    @classmethod
    def int_to_byte(cls, value):
        return value.to_bytes(1, 'big')

    @classmethod
    def int_to_short(cls, value):
        return value.to_bytes(2, 'big')

    @classmethod
    def int_from_bytes(cls, value):
        return int.from_bytes(value, 'big')
