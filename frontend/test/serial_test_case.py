import unittest
import os

from serial import Serial

from exceptions import SpcExpection


class SerialTestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.port = os.environ.get('SPC_PORT')
        if cls.port is None:
            raise RuntimeError('set SPC_PORT environment variable to specify port used for testing')
        cls.serial = Serial(cls.port, 115200, timeout=3)
        result = cls.serial.read()
        if result != b'R':
            raise SpcExpection('SPC does not respond with R to signal ready')

    @classmethod
    def tearDownClass(cls):
        cls.serial.close()
        super().tearDownClass()
