import unittest
import os
import time

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
        # Wait for SPC being ready because Arduino is reseted when serial is opened.
        result = cls.serial.read()
        if result != b'1':
            raise SpcExpection('SPC does not respond with \'1\' to signal ready')

    @classmethod
    def tearDownClass(cls):
        cls.serial.close()
        super().tearDownClass()
