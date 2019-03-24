import time

class Uart:

    @classmethod
    def writeRam(cls, serial, spc):
        time.sleep(2)
        serial.write(b'B')
        serial.write(0x0002)
        print(serial.read(2))
        serial.write(0x0101)
        print(serial.read(2))
