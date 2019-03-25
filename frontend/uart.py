from exceptions import SpcExpection


class Uart:

    @classmethod
    def writeRam(cls, serial, spc):
        serial.write(b'B')
        serial.write(bytes.fromhex('0002'))
        result = serial.read(2)
        if result != b'\x00\x02':
            raise SpcExpection('writing ram address failed with result: {0}'.format(result))
        serial.write(bytes.fromhex('00ff'))
        result = serial.read(2)
        if result != (255).to_bytes(2, byteorder='big'):
            raise SpcExpection('writing ram length failed with result: {0}'.format(result))

    @classmethod
    def reset(cls, serial):
        serial.write(b'R')
        result = serial.read()
        if result != b'R':
            raise SpcExpection('spc reset failed with result: {0}'.format(result))
