#!/usr/bin/python3
import sys

from xml_loader import XML_Loader
from program_syntax import SyntaxCheck
from program_execution import ProgramExecuter

# Only 1 argument ?
if len(sys.argv) == 2:
    # Accepted is only --input=
    if sys.argv[1][:8] == "--input=":
        arguments = [sys.stdin,sys.argv[1][8:]]
    # Or --source=
    elif sys.argv[1][:9] == "--source=":
        arguments = [sys.argv[1][9:],sys.stdin]
    # Or it can be --help
    elif sys.argv[1][:6] == "--help":
        print("Usage:")
        print("python3.6",sys.argv[0]," [options]")
        print("Options:")
        print("\t--help\t\tShow this help message.")
        print("\t--source=file\t\tSource file containing xml to interpret.")
        print("\t--input=file\t\tInput file containing values which will READ receive.")
        print("Source or input need to be given and the other one should be given on stdin.")
        exit(0)
    # Everything else error
    else:
        arguments = []
# 2 arguments ?
elif len(sys.argv) == 3:
    # Accepted is --input= and --source=
    if sys.argv[1][:8] == "--input=" and sys.argv[2][:9] == "--source=":
        arguments = [sys.argv[1][8:],sys.argv[2][9:]]
    # Accepted is --source= and --input=
    elif sys.argv[1][:9] == "--source=" and sys.argv[2][:8] == "--input=":
        arguments = [sys.argv[1][9:],sys.argv[2][8:]]
    # Everything else error
    else:
        arguments = []
# More than 2 ? Error
else:
    arguments = []

# If we send empty list error wrong arguments
if not arguments:
    print("Wrong arguments",file=sys.stderr)
    exit(10)

# Load XML from file into list
XML = XML_Loader(arguments[0])
xml_file = XML.LoadXML()

# Check if we return error or the list
if not isinstance(xml_file,dict):
    #print("XML Error",file=sys.stderr)
    exit(xml_file)

# Check syntax of the program
result = SyntaxCheck(xml_file).Syntax()

# If there was syntax error exit
if result != 0:
    #print("Syntax error",file=sys.stderr)
    exit(result)

# Create class for the program to be executed in
program_execution = ProgramExecuter(xml_file,arguments[1])

# Execute program
result = program_execution.Execute()

# Check if there was any error
exit(result)
