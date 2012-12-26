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
<<<<<<< HEAD
  proc = subprocess.Popen(command, stdout=outputFile, stderr=subprocess.STDOUT)
=======
  if os.name == "nt":
    proc = subprocess.Popen(command, stdout=outputFile, stderr=subprocess.STDOUT, shell=True)
  else:
    proc = subprocess.Popen(command, stdout=outputFile, stderr=subprocess.STDOUT)
>>>>>>> future
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
<<<<<<< HEAD
if args.prefix != None:
  cmakeCommand.append("-DCMAKE_INSTALL_PREFIX:STRING="+args.prefix)
=======
makeCommand = "make"

# Support for Windows
if os.name == "nt":
  cmakeCommand.append("-G \"MinGW Makefiles\"")
  makeCommand = "mingw32-make"

if args.prefix != None:
  cmakeCommand.append("-DCMAKE_INSTALL_PREFIX:STRING=\""+args.prefix+"\"")
>>>>>>> future
if args.enableMemCheck:
  cmakeCommand.append("-DPERFORM_MEM_TESTS:BOOL=ON")
cmakeCommand.append(".")

<<<<<<< HEAD
runSubProc(
  cmakeCommand, 
  "Running cmake with command: \n\"" + " ".join(cmakeCommand)+"\"\n",
=======
# Windows does weird things if this is not joined...
cmakeCommand = " ".join(cmakeCommand)

runSubProc(
  cmakeCommand,
  "Running cmake with command: \n\"" + cmakeCommand + "\"\n",
>>>>>>> future
  "There was a CMake error",
  "configure.out")

runSubProc(
<<<<<<< HEAD
  ["make", "-j"+j], 
=======
  [makeCommand, "-j"+j], 
>>>>>>> future
  "Running make ",
  "There was a make error",
  "make.out")


if args.buildDocs:
  runSubProc(
<<<<<<< HEAD
    ["make", "-j"+j, "docs"], 
=======
    [makeCommand, "-j"+j, "docs"], 
>>>>>>> future
    "Building documentation ",
    "There was a documentation building error",
    "docs.out")
  
runSubProc(
<<<<<<< HEAD
  ["make", "install"], 
=======
  [makeCommand, "install"], 
>>>>>>> future
  "Installing ",
  "There was an installation error",
  "install.out")

if args.runTests:
  runSubProc(
    ["ctest", "-j"+j], 
    "Testing ",
    "There was a testing error",
    "test.out")
