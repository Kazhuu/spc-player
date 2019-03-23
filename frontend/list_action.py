import argparse

from serial.tools import list_ports


class ListAction(argparse._HelpAction):

    def list_ports(self):
        for port in list_ports.comports():
            print(port.device)

    def __call__(self, parser, namespace, values, option_string=None):
        self.list_ports()
        parser.exit()
