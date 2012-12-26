#!/usr/bin/python
import subprocess
import sys
import tempfile
import argparse

MEMERR = 127
  
parser = argparse.ArgumentParser(description="Driver for lci tests")
parser.add_argument('pathToLCI', help="The absolute path the the lci executable")
parser.add_argument('lolcodeFile', help="The absolute path to the lolcode file to test")
parser.add_argument('-o', '--outputFile', type=argparse.FileType('r'), default=None, help="The expected output")
parser.add_argument('-i', '--inputFile', type=argparse.FileType('r'), default=None, help="File to be used as input")
parser.add_argument('-e', '--expectError', action="store_true", help="Specify that an error should occur")
parser.add_argument('-m', '--memCheck', action='store_true', help="Do a memory check")

args = parser.parse_args()


if args.inputFile == None:
  print("Not using an input file")
else:
  print("Using input file: " + args.inputFile.name) 

if args.expectError:
  print("Expecting an error") 
else:
  print("Not expecting an error")

if args.outputFile == None:
  print("Not using an output file")
else:
  print("Using output file: " + args.outputFile.name) 

if args.memCheck:
  print("Doing memory check.")
else:
  print("Not doing memory check.")

expectedOutput = ""
if args.outputFile != None:
  expectedOutput = args.outputFile.read()
  args.outputFile.close()

command = []
if args.memCheck:
  command.append("valgrind")
  command.append("-q")
  command.append("--leak-check=full")
  command.append("--error-exitcode=" + str(MEMERR))
command.append(args.pathToLCI)
command.append(args.lolcodeFile)

print("Command: " + " ".join(command))

p = subprocess.Popen(command, stdin=args.inputFile, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
results = p.communicate()

if p.returncode == MEMERR:
  print("Failure!\n Memory leak detected, check output for more information.)")
  sys.exit(1)

if args.expectError:
  if p.returncode == 0:
    print("Failure! Expected an error but did not recieve one")
    sys.exit(1)
  else:
    print("Success!")
    print("Error:")
    print(results[1])
 
if args.outputFile:
  if p.returncode != 0:
    print("Failure! Return error code: " + str(p.returncode))
    sys.exit(1)
  elif expectedOutput != results[0]:
    print("Expected output didn't match!")
    print("Expected output:")
    print(expectedOutput)
    print("Actual output:")
    print(results[0])
    sys.exit(1)
  else:
    print("Success!\n\n")

if args.inputFile !=None:
  args.inputFile
