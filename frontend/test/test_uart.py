from test.serial_test_case import SerialTestCase
from uart import Uart
from exceptions import SpcExpection


class UartTestCase(SerialTestCase):

    def setUp(self):
        super().setUp()
        Uart.reset(self.serial)

    def test_reset(self):
        """
        Test reseting SPC. Should not raise any exceptions.
        """
        try:
            Uart.reset(self.serial)
        except SpcExpection as e:
            self.fail('Uart.reset() raised exception unexpectedly: {0}'.format(str(e)))

    def test_read_ports(self):
        """
        Test reading all four communication ports port 0-3. SPC boot code (IPL)
        should place 0xAA to port 0 and 0xBB to port 1 at beginning to indicate
        it is ready to transfer data. Rest of the ports should be zero.
        """
        result = Uart.read_ports(self.serial)
        self.assertEqual(result, [b'\xaa', b'\xbb', b'\x00', b'\x00'])

    def test_write_block(self):
        """
        Test writing block of bytes. Write bytes to address 0x00F5 (port 1) and
        0x00F6 (port 2) so they can be read back with read_ports() method
        easily.
        """
        data = [b'\x01', b'\x02']
        Uart.write_block(self.serial, 0x00F5, data)
        result = Uart.read_ports(self.serial)
        self.assertEqual(result, [b'\x01', b'\x01', b'\x02', b'\x00'])

    def test_start(self):
        """
        Test writing small code to SPC and execute it. Code will increments x
        register twice and output value to port 3. After this it will jump to
        IPL boot code. Port values will be read with read_ports() method to
        verify. Port 0 should contain 0xAA and port 1 should have 0xBB from IPL
        boot code and port 3 should have value 2 from X register.
        """
        data = [
            0x3D,             # inc X
            0x3D,             # inc X
            0xD8, 0xF6,       # mov ($F6), X
            0x5F, 0xC0, 0xFF  # jmp !$FFC0
        ]
        address = 0x0002
        Uart.write_block(self.serial, address, data)
        Uart.start(self.serial, address)
        result = Uart.read_ports(self.serial)
        self.assertEqual(result, [b'\xaa', b'\xbb', b'\x02', b'\x00'])

    def test_write_dsp_registers(self):
        """
        Test writing one DSP register and small asm code which will read
        written DSP register address and output it to port 3. Value is read
        back to Python and asserted it's correct.
        Asm code first place 0x01 to DSP register ram address 0xF2. This will
        point to register 0x01 within DSP registers. Then read DSP register
        value from ram address 0xF3 and output it to port 3 (ram address 0xF7).
        Lastly jump to IPL rom boot code  at 0xFFC0.
        """
        dsp_address = 0x01  # Right channel volume
        dsp_registers = [0x10]
        code_address = 0x0002
        asm = [
            0x8F, 0x01, 0xF2,  # mov ($F2), #$01
            0xFA, 0xF3, 0xF7,  # mov ($F7), ($F3)
            0x5F, 0xC0, 0xFF   # jmp !$FFC0
        ]
        Uart.write_dsp_registers(self.serial, dsp_address, dsp_registers)
        Uart.write_block(self.serial, code_address, asm)
        Uart.start(self.serial, code_address)
        result = Uart.read_ports(self.serial)
        self.assertEqual(result, [b'\xaa', b'\xbb', b'\x00', b'\x10'])
