import argparse

parser = argparse.ArgumentParser()
parser.add_argument('spc_file', help='path to the SPC sound file')
args = parser.parse_args()

print(args.spc_file)
