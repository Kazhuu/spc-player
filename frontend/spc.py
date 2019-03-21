class Spc:

    def __init__(self, spc_file):
        self._cpu_registers(spc_file)
        self._ram(spc_file)
        self._dps_registers(spc_file)

    def _cpu_registers(self, spc_file):
        spc_file.seek(0x25)
        self.program_couter = spc_file.read(2)
        self.a_register = spc_file.read(1)
        self.x_register = spc_file.read(1)
        self.y_register = spc_file.read(1)
        self.program_status_word = spc_file.read(1)
        self.stack_pointer = spc_file.read(1)

    def _ram(self, spc_file):
        spc_file.seek(0x100)
        self.ram = spc_file.read(65536)

    def _dsp_registers(self, spc_file):
        spc_file.seek(0x10100)
        self.dsp_registers = spc_file.read(128)
