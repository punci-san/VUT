#!/usr/bin/python3

import xml.dom.minidom
import sys

class XML_Loader():

    def __init__(self, load_method):
        self.file = load_method
        self.program_array = {}

    def CheckProgramArray(self):
        # Return list of keys sorted from 1 to N
        tmp = sorted(self.program_array.keys(),key=lambda x: int(x))

        # No instruction
        if len(tmp) == 0:
            return self.program_array

        # Check if last argument is the count of all inserted instructions
        if not tmp[-1] == str(len(tmp)):
            return 32

        return self.program_array

    # Check argument
    def CheckArgument(self,arg):
        # Does XML argument contain attribute type ?
        if not arg.hasAttribute("type"):
            return []

        if arg.firstChild is None:
            return [arg.getAttribute("type"),""]
        # If yes then return him
        return [arg.getAttribute("type"),arg.firstChild.data]

    def LoadXML(self):
        # If no file was given use STDIN
        if self.file == False:
            self.file = sys.stdin
            pass

        # Parse XML file
        try:
            DOMTree = xml.dom.minidom.parse(self.file)
        except Exception as e:
            print("Error opening file: " + self.file)
            # Badly formatted XML
            return 31

        # Create collection from XML file
        collection = DOMTree.documentElement

        # Check all attributes
        if not (1 <= len(collection.attributes.items()) <= 3):
            return 32

        # Variabels for checking program
        descriptionFound = False
        nameFound = False
        languageFound = False

        # Loop through program and check values
        for (name,value) in collection.attributes.items():
            if name == "description" and (not descriptionFound):
                descriptionFound = True
                continue

            if name == "name" and (not nameFound):
                nameFound = True
                continue

            if name == "language" and (not languageFound) and value == "IPPcode19":
                languageFound = True
                continue

            return 32

        # language required
        if not languageFound:
            return 32

        # Loop through all instructions and check them
        for instruction in collection.childNodes:
            # Ignore text elements
            if instruction.nodeName == "#text":
                for char in instruction.nodeValue:
                    if ord(char) == 9 or ord(char) == 10 or ord(char) == 13 or ord(char) == 32:
                        continue
                    return 32
                continue

            # Check if other attribute is instruction
            if not instruction.nodeName == "instruction":
                return 32

            # Get all attributes
            if not len(instruction.attributes.items()) == 2:
                return 32

            orderFound = False
            opcodeFound = False

            for (name,value) in instruction.attributes.items():
                if name == "order" and not orderFound:
                    orderFound = True
                if name == "opcode" and not opcodeFound:
                    opcodeFound = True
            if not orderFound or not opcodeFound:
                return 32

            arg1Found = False
            arg2Found = False
            arg3Found = False

            # Get all arguments
            for argument in instruction.childNodes:
                if argument.nodeName == "#text":
                    for char in argument.nodeValue:
                        if ord(char) == 9 or ord(char) == 10 or ord(char) == 13 or ord(char) == 32:
                            continue
                        return 32
                    continue

                if len(argument.attributes.items()) == 1:
                    if argument.attributes.items()[0][0] != "type":
                        return 32
                else:
                    return 32

                for test in argument.childNodes:
                    if test.nodeName == "#text":
                        continue
                    return 32

                if argument.nodeName == "arg1" and not arg1Found:
                    arg1Found = True
                    continue

                if argument.nodeName == "arg2" and not arg2Found:
                    arg2Found = True
                    continue

                if argument.nodeName == "arg3" and not arg3Found:
                    arg3Found = True
                    continue

                return 32

        # Load all instruction
        instructions = collection.getElementsByTagName("instruction")

        # Loop through the list of instructions
        for instruction in instructions:

            # List in which will be instruction loaded
            tmp_array = {}

            # Check if XML contain attribute "order" and "oppcode"
            if not (instruction.hasAttribute("order") and instruction.hasAttribute("opcode")):
                print("Error XML: order | opcode")
                return 31

            # Load tmp array
            tmp_array["instruction"] = instruction.getAttribute("opcode")
            tmp_array["arg1"] = []
            tmp_array["arg2"] = []
            tmp_array["arg3"] = []

            # get elements
            arg1 = instruction.getElementsByTagName('arg1')
            arg2 = instruction.getElementsByTagName('arg2')
            arg3 = instruction.getElementsByTagName('arg3')

            # Check if there are more arguments of the same type
            if arg1.length > 1 or arg2.length > 1 or arg3.length > 1:
                # if Yes error
                print("Error XML: more than 1 argument")
                return 31

            # If there is arg1
            if len(arg1) == 1:
                # Check if XML is okay
                tmp = self.CheckArgument(arg1[0])
                if not len(tmp) == 2:
                    print("Error XML: arg1 failed")
                    return 31
                # Copy
                tmp_array["arg1"] = tmp

            # If there is arg2
            if len(arg2) == 1:
                # Check if XML is okay
                tmp = self.CheckArgument(arg2[0])
                if not len(tmp) == 2:
                    print("Error XML: arg1 failed")
                    return 31
                # Copy
                tmp_array["arg2"] = tmp

            # If there is arg3
            if len(arg3) == 1:
                # Check if XML is okay
                tmp = self.CheckArgument(arg3[0])
                if not len(tmp) == 2:
                    print("Error XML: arg1 failed")
                    return 31
                # Copy
                tmp_array["arg3"] = tmp

            # Get attribute order
            order = instruction.getAttribute("order")

            # Check if order is number
            if not order.isdigit():
                print("Error XML: order not digit")
                return 31

            # Check if order is not the same number
            if order in self.program_array:
                return 32

            # Add to dictionary
            self.program_array[order] = tmp_array
        #End for

        # Check program array before returning
        return self.CheckProgramArray()
