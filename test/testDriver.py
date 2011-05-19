#!/usr/bin/python
import subprocess
import sys
import tempfile
import argparse
  
parser = argparse.ArgumentParser(description="Driver for lci tests")
parser.add_argument('pathToLCI', help="The absolute path the the lci executable")
parser.add_argument('lolcodeFile', help="The absolute path to the lolcode file to test")
parser.add_argument('-o', '--outputFile', type=argparse.FileType('r'), default=tempfile.TemporaryFile(), help="The expected output")
parser.add_argument('-e', '--errFile', type=argparse.FileType('w'), default=tempfile.TemporaryFile(), help="Pipe error output to this file")
parser.add_argument('-i', '--inputFile', type=argparse.FileType('r'), default=None, help="File to be used as input")
parser.add_argument('-q', '--quiet', action='store_true', help="Make the test quite")
parser.add_argument('-m', '--memCheck', action='store_true', help="Do a memory check")

args = parser.parse_args()

expectedOutput = args.outputFile.read()
args.outputFile.close()

  

p = subprocess.Popen([args.pathToLCI, args.lolcodeFile], stdin=args.inputFile, stdout=subprocess.PIPE, stderr=args.errFile if args.errFile else None)
results = p.communicate()
args.errFile.close()

if expectedOutput != results[0]:
  print("Expected output didn't match!")
  print("Expected output:")
  print(expectedOutput)
  print("Actual output:")
  print(results[0])
  sys.exit(1)

print("test pased")

