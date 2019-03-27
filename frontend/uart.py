from exceptions import SpcExpection


class Uart:

    @classmethod
    def writeBlock(cls, serial, address, data):
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
            result = serial.read()
            if result != byte:
                raise SpcExpection('writing {0} byte of block timed out'.format(index))

    @classmethod
    def readPorts(cls, serial):
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
            raise SpcExpection('starting spc execution timed out')

    @classmethod
    def reset(cls, serial):
        serial.write(b'R')
        result = serial.read()
        if result != b'R':
            raise SpcExpection('spc reset failed timed out')

    @classmethod
    def int_to_byte(cls, value):
        return value.to_bytes(1, 'big')

    @classmethod
    def int_to_short(cls, value):
        return value.to_bytes(2, 'big')

    @classmethod
    def int_from_bytes(cls, value):
        return int.from_bytes(value, 'big')
