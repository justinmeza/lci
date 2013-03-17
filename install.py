#!/usr/bin/python
import subprocess
import argparse
import sys
import os

#Checks if a string is a positive integer
def positiveInt(string):
  value = int(string)
  if not value >= 1:
    msg = string + " is not a positive integer"
    raise argparse.ArgumentTypeError(msg)
  return value

"""
Runs a subprocess using the command parameter.
Before running the command it displays a message
that contains the provided description and where
the output will be sent. If an error occurs,
the errorMsg is displayed.
"""
def runSubProc(command, description, errorMsg, output):
  msg = description
  msg += "and writing results to " + output +"."
  print(msg + "\n")

  outputFile = open(output, "w")
  if os.name == "nt":
    proc = subprocess.Popen(command, stdout=outputFile, stderr=subprocess.STDOUT, shell=True)
  else:
    proc = subprocess.Popen(command, stdout=outputFile, stderr=subprocess.STDOUT, shell=True)
  proc.wait()
  if proc.returncode != 0:
    print("Error installing: " + errorMsg)
    sys.exit(1)

#Remove the CMakeCache.txt so we can garuntee a fresh configure
if os.path.exists("CMakeCache.txt"):
  os.remove("CMakeCache.txt")

parser = argparse.ArgumentParser(description="Installation script for lci")
parser.add_argument('-p', '--prefix', default=None, help="Installation prefix")
parser.add_argument('-m', '--enableMemCheck', action="store_true", help="Enable memory testing")
parser.add_argument('-d', '--buildDocs', action="store_true", help="Build documentation")
parser.add_argument('-t', '--runTests', action="store_true", help="Run Tests")
parser.add_argument('-j', metavar="NumProcs", type=positiveInt, default=1, help="Number of processes for make to use and (if enabled) how many processes CTest should use.")

args = parser.parse_args()
j = str(args.j)

cmakeCommand = ["cmake"]
makeCommand = "make"

# Support for Windows
if os.name == "nt":
  cmakeCommand.append("-G \"MinGW Makefiles\"")
  makeCommand = "mingw32-make"

if args.prefix != None:
  cmakeCommand.append("-DCMAKE_INSTALL_PREFIX:STRING=\""+args.prefix+"\"")
if args.enableMemCheck:
  cmakeCommand.append("-DPERFORM_MEM_TESTS:BOOL=ON")
cmakeCommand.append(".")

# Windows does weird things if this is not joined...
cmakeCommand = " ".join(cmakeCommand)

runSubProc(
  cmakeCommand,
  "Running cmake with command: \n\"" + cmakeCommand + "\"\n",
  "There was a CMake error",
  "configure.out")

runSubProc(
  [makeCommand, "-j"+j],
  "Running make ",
  "There was a make error",
  "make.out")


if args.buildDocs:
  runSubProc(
    [makeCommand, "-j"+j, "docs"],
    "Building documentation ",
    "There was a documentation building error",
    "docs.out")

runSubProc(
  makeCommand + " install",
  "Installing ",
  "There was an installation error",
  "install.out")

if args.runTests:
  runSubProc(
    ["ctest", "-j"+j],
    "Testing ",
    "There was a testing error",
    "test.out")
