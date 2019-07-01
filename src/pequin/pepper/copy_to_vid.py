import argparse
import os
from shutil import copyfile

parser = argparse.ArgumentParser()
parser.add_argument("source", help="source prefix")
parser.add_argument("dest", help="destination location")
args = parser.parse_args()

cur_dir = os.getcwd()

for (_, _, filenames) in os.walk(args.dest):
    for filename in filenames:
        os.remove(args.dest + '/' + filename)

for (_, _, filenames) in os.walk(cur_dir + "/bin"):
    for filename in filenames:
        if args.source in filename:
            _, ext = os.path.splitext(filename)
            ext = ext[1:]
            if len(ext):
                copyfile(cur_dir + "/bin" + '/' + filename, args.dest + '/' + ext)
