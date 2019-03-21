import argparse

from spc import Spc

parser = argparse.ArgumentParser()
parser.add_argument('spc_file', help='path to the SPC sound file')
args = parser.parse_args()

with open(args.spc_file, 'rb') as f:
    spc = Spc(f)
