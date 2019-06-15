#!/usr/bin/python3
import sys
import re

class ProgramExecuter():

    def __init__(self, array,input):
        # Program with input
        self.program = array
        self.input = input
        # Program index
        self.index = 1;
        # When not None ending with "end" code
        self.end = None
        # Dictionary of labels "label" => index
        self.label_list = {}
        # Stack of return indexes of functions
        self.function_stack = []
        # Stack PUSH POP
        self.stack = []
        # Variable holder
        self.LF_list = []
        self.GLOBAL_FRAME = {}
        self.TEMPORALY_FRAME = None

        self.executed_instructions = 0

    def Execute(self):
        # Load labels
        if not self.LoadLabels():
            return 52

        if self.input is not sys.stdin:
            try:
                self.input = open(self.input,"r")
            except:
                return 11

        # Start executing program
        while self.index <= len(self.program):
            # Check if we want to exit sooner
            if self.end is not None:
                try:
                    close(self.input)
                except:
                    pass
                #print("Program is exiting with error code: ",self.end,file=sys.stderr)
                return self.end
            # Try to execute instruction
            if not self.ExecuteInstruction(self.program[str(self.index)]):
                try:
                    close(self.input)
                except:
                    pass
                return 32
            self.executed_instructions += 1
            # Increment instruction index
            self.index += 1

        # Check if we end with exit code or not
        if self.end is not None:
            return self.end
        return 0

    def ExecuteInstruction(self,instruction):
        # Get instruction and arguments
        inst = instruction["instruction"].upper()

        # DEFVAR
        if inst == "DEFVAR":
            # Try to define variable
            if not self.DefineVariable(instruction["arg1"][1]):
                # Redefinition of variable
                self.end = 55
            return True

        # Get arguments
        # arg => [variable,type,value]
        arg1 = self.GetVarValue(instruction["arg1"])
        arg2 = self.GetVarValue(instruction["arg2"])
        arg3 = self.GetVarValue(instruction["arg3"])

        # If variable does not exist
        if arg1 == None or arg2 == None or arg3 == None:
            print("None variable",file=sys.stderr)
            return True

        # MOVE
        if inst == "MOVE":
            # Check if variable is defined
            if arg1[0] is None:
                self.end = 56
                return True
            # Check if second variable which we are copying from is initialized
            if arg2[1] is None:
                self.end = 56
                return True

            # Set variable
            self.SetVariable(arg1[0],[arg2[1],arg2[2]])

        # If we got exit
        if inst == "EXIT":
            # Set error code
            self.end = 57
            # Check if arg1 is defined
            if arg1[2] is None:
                self.end = 56
                return True
            # check if first argument is int
            if not arg1[1] == "int":
                self.end = 53
                return True
            # Check range <0,49>
            if not (0 <= arg1[2] <= 49):
                return True
            # Set exit code
            self.end = arg1[2]
            return True

        # Pushs
        if inst == "PUSHS":
            # Check if initialized argument
            if arg1[1] is None:
                self.end = 56
                return True
            # Stack[index] => [type,value]
            self.stack.append([arg1[1],arg1[2]])
            return True

        # POPS
        if inst == "POPS":
            # Check if variable is defined
            if arg1[0] is None:
                self.end = 56
                return True
            # Check if stack is empty
            if len(self.stack) == 0:
                # Empty stack
                self.end = 56
                return True
            # Set variable
            self.SetVariable(arg1[0],self.stack.pop())
            return True

        # ADD
        if inst == "ADD" or inst == "SUB" or inst == "MUL" or inst == "IDIV":
            # Check if variable is defined
            if arg1[0] is None:
                self.end = 56
                return True
            # Check if both values are initialized
            if arg2[1] is None or arg3[1] is None:
                self.end = 56
                return True
            # Check if both are integers
            if not arg2[1] == "int" or not arg3[1] == "int":
                self.end = 53
                return True

            # ADD
            if inst == "ADD":
                # Set variable
                self.SetVariable(arg1[0],["int",(arg2[2] + arg3[2])])
                return True
            # SUB
            elif inst == "SUB":
                # Set variable
                self.SetVariable(arg1[0],["int",(arg2[2] - arg3[2])])
                return True
            # MUL
            elif inst == "MUL":
                # Set variable
                self.SetVariable(arg1[0],["int",(arg2[2] * arg3[2])])
                return True
            # IDIV
            else:
                # 
                if arg3[2] == 0:
                    self.end = 57
                    return True
                # Set variable
                self.SetVariable(arg1[0],["int",(arg2[2] // arg3[2])])
                return True

        # Label ?
        if inst == "LABEL":
            # Skip
            return True

        # Jump ?
        if inst == "JUMP":
            # Check if name exist in dictionary
            if not arg1[2] in self.label_list:
                # Does not exist
                self.end = 52
                return True
            # Get index and decrement by one because + 1 will happen
            self.index = int(self.label_list[arg1[2]]) - 1
            return True

        # Jumpifeq ?
        if inst == "JUMPIFEQ":
            # Check if label exist
            if not arg1[2] in self.label_list:
                self.end = 52
                return True

            # Check if both arg2 and arg3 are initialized
            if arg2[1] is None or arg3[1] is None:
                # Missing values in variables
                self.end = 56
                return True

            # Check if types are the same
            if not arg2[1] == arg3[1]:
                self.end = 53
                return True
            # Types are the same
            if arg2[2] == arg3[2]:
                # Execute jump
                self.index = int(self.label_list[arg1[2]]) - 1
            return True

        # Jumpifneq ?
        if inst == "JUMPIFNEQ":
            # Check if label exist
            if not arg1[2] in self.label_list:
                self.end = 52
                return True

            # Check if both arg2 and arg3 are initialized
            if arg2[1] is None or arg3[1] is None:
                # Missing values in variables
                self.end = 56
                return True

            # Check if types are the same
            if not arg2[1] == arg3[1]:
                self.end = 53
                return True
            # Types are the same
            if not arg2[2] == arg3[2]:
                # Execute jump
                self.index = int(self.label_list[arg1[2]]) - 1
            return True

        # Call
        if inst == "CALL":
            # Check if label exist
            if not arg1[2] in self.label_list:
                # Label does not exist
                self.end = 52
                return True
            # Save index from which we are calling the function
            self.function_stack.append(self.index)
            # Jump to the index of label
            self.index = int(self.label_list[arg1[2]]) - 1
            return True

        # Return
        if inst == "RETURN":
            # Check if something is on the function stack
            if len(self.function_stack) == 0:
                # Nothing on function stack
                self.end = 56
                return True
            # Pop the last value from function stack
            self.index = self.function_stack.pop()
            return True

        # Write instruction
        if inst == "WRITE":
            if arg1[1] is None:
                self.end = 56
                return True
            # Dont do anything with nil
            if not arg1[1] == "nil":
                if arg1[1] == "string":
                    string = ""
                    index = 0
                    # Loop through every character
                    while(index < len(arg1[2])):
                        # If we found \ we need to convert it into ascii value
                        if arg1[2][index] == "\\":
                            tmp_str = ""
                            # Loop through the next 3 number after \
                            for tmp in range(1,4):
                                tmp_str += arg1[2][index + tmp]
                            index += 4
                            # Convert decimal value into ascii
                            string += chr(int(tmp_str))
                            continue
                        string += arg1[2][index]
                        index += 1
                    print(string,end='')
                    return True
                print(arg1[2],end='') # flush=true
            return True

        # Read instruction
        if inst == "READ":
            # Check if variable exist
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True

            # Read from STDIN
            if self.input is sys.stdin:
                # Read input
                try:
                    inp = input()
                except EOFError:
                    inp = ""
            # Read from file
            else:
                try:
                    inp = self.input.readline()
                    inp = inp[:-1]
                except Exception as e:
                    inp = ""

            # Check if regex is true
            regex = re.match("(\\\\\d{3,}|[^\\\\\s|#])*",inp,0)
            # Nothing found
            if regex is None:
                inp = ""

            # Some bad characters might be after
            if not len(regex.group()) == len(inp):
                inp = ""

            # Type is string
            if arg2[2] == "string":
                # Set string
                self.SetVariable(arg1[0],["string",str(inp)])
                return True
            # Type is int
            elif arg2[2] == "int":
                # Set default value
                if inp == "":
                    integer = 0
                # Convert given value
                else:
                    try:
                        integer = int(inp)
                    except Exception as e:
                        integer = 0

                # Set integer from input
                self.SetVariable(arg1[0],["int",integer])
                return True
            # Type is boolean
            elif arg2[2] == "bool":
                # If input was true
                if inp.upper() == "TRUE":
                    # Set true
                    self.SetVariable(arg1[0],["bool","true"])
                # Everything else is false
                else:
                    self.SetVariable(arg1[0],["bool","false"])
                return True
            # Some other type
            else:
                # Return error
                self.end = 56
            return True

        # STRLEN
        if inst == "STRLEN":
            # Check if variable exist
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True
            # Check if second argument has been initialized
            if arg2[1] is None:
                self.end = 56
                return True
            # Check if second argument is string
            if not arg2[1] == "string":
                # Wrong type
                self.end = 53
                return True
            self.SetVariable(arg1[0],["int",int(len(arg2[2]))])

        # COCNAT
        if inst == "CONCAT":
            # Check if first is variable
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True
            # Check if both 2 and 3 arguments are initialized
            if arg2[1] is None or arg3[1] is None:
                self.end = 56
                return True

            if arg2[1] == "string" and arg3[1] == "string":
                # Make concat
                self.SetVariable(arg1[0],["string",str(arg2[2]) + str(arg3[2])])
                return True
            else:
                # Wrong arguments
                self.end = 53
                return True

        # GETCHAR
        if inst == "GETCHAR":
            # Check if first is variable
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True
            # Check if both 2 and 3 arguments are initialized
            if arg2[1] is None or arg3[1] is None:
                self.end = 56
                return True
            # Check if arg2 is string and arg3 is int
            if not arg2[1] == "string" or not arg3[1] == "int":
                self.end = 53
                return True
            # Check if arg3 is in range of arg2
            if arg3[2] >= len(arg2[2]):
                print("Index out of range",file=sys.stderr)
                self.end = 58
                return True
            # Getchar set variable
            self.SetVariable(arg1[0],["string",arg2[2][int(arg3[2])]])
            return True

        # SETCHAR
        if inst == "SETCHAR":
            # Check if first is variable
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True
            # Check if 1, 2 and 3 arguments are initialized
            if arg1[1] is None or arg2[1] is None or arg3[1] is None:
                self.end = 56
                return True
            # Check if arg2 is string and arg3 is int
            if not arg1[1] == "string" or not arg2[1] == "int" or not arg3[1] == "string":
                self.end = 53
                return True
            # Check if index arg2 is in range arg3 and if arg3 is not 0 and if arg2 is in range of arg1
            if len(arg3[2]) == 0 or arg2[2] >= len(arg1[2]):
                print("Index out of range or arg3 is zero",file=sys.stderr)
                self.end = 58
                return True
            # Copy arg1 and convert to list
            string = list(arg1[2])
            # Check if arg3 is only 1 char
            if len(arg3[2]) == 1:
                # Change char on index
                string[arg2[2]] = arg3[2]
            else:
                # Set first char on index
                string[arg2[2]] = arg3[2][0]
            string = "".join(string)
            # Getchar set variable
            self.SetVariable(arg1[0],["string",string])
            return True

        # EQ
        if inst == "EQ":
            # Check if first is variable
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True
            # Check if both are none
            if arg2[1] is None or arg3[1] is None:
                # Return error uninitialized
                self.end = 56
                return True
            # Not the same type ?
            if not arg2[1] == arg3[1]:
                if arg2[1] == "nil" or arg3[1] == "nil":
                    self.SetVariable(arg1[0],["bool","false"])
                    return True
                self.end = 53
                return True

            # String or Bool or Int or Nil ?
            if arg2[1] == "string" or arg2[1] == "bool" or arg2[1] == "int" or arg2[1] == "nil":
                if arg2[2] == arg3[2]:
                    self.SetVariable(arg1[0],["bool","true"])
                else:
                    self.SetVariable(arg1[0],["bool","false"])
                return True
            # Some other type error
            else:
                self.end = 53
                return True
        # LT
        if inst == "LT":
            # Check if first is variable
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True

            # Check if both are none
            if arg2[1] is None or arg3[1] is None:
                # Return error uninitialized
                self.end = 56
                return True

            # Not the same type ?
            if not arg2[1] == arg3[1]:
                self.end = 53
                return True

            # String ?
            if arg2[1] == "string":
                # Calculate string 1
                string1_val = 0
                for x in range(0,len(arg2[2])):
                    string1_val += ord(arg2[2][x])
                # Calculate string 2
                string2_val = 0
                for x in range(0,len(arg3[2])):
                    string2_val += ord(arg3[2][x])
                # Compare values
                if string1_val < string2_val:
                    self.SetVariable(arg1[0],["bool","true"])
                else:
                    self.SetVariable(arg1[0],["bool","false"])
            # Bool ?
            elif arg2[1] == "bool":
                if arg2[2] == "true":
                    self.SetVariable(arg1[0],["bool","false"])
                else:
                    if arg3[2] == "true":
                        self.SetVariable(arg1[0],["bool","true"])
                    else:
                        self.SetVariable(arg1[0],["bool","false"])
            # Int ?
            elif arg2[1] == "int":
                # Compare values
                if arg2[2] < arg3[2]:
                    self.SetVariable(arg1[0],["bool","true"])
                else:
                    self.SetVariable(arg1[0],["bool","false"])
            # Error
            else:
                pass

        # GT
        if inst == "GT":
            # Check if first is variable
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True

            # Check if both are none
            if arg2[1] is None or arg3[1] is None:
                # Return error uninitialized
                self.end = 56
                return True

            # Not the same type ?
            if not arg2[1] == arg3[1]:
                self.end = 53
                return True

            # String ?
            if arg2[1] == "string":
                # Calculate string 1
                string1_val = 0
                for x in range(0,len(arg2[2])):
                    string1_val += ord(arg2[2][x])
                # Calculate string 2
                string2_val = 0
                for x in range(0,len(arg3[2])):
                    string2_val += ord(arg3[2][x])
                # Compare values
                if string1_val > string2_val:
                    self.SetVariable(arg1[0],["bool","true"])
                else:
                    self.SetVariable(arg1[0],["bool","false"])
            # Bool ?
            elif arg2[1] == "bool":
                if arg2[2] == "true":
                    if arg3[2] == "false":
                        self.SetVariable(arg1[0],["bool","true"])
                    else:
                        self.SetVariable(arg1[0],["bool","false"])
                else:
                    self.SetVariable(arg1[0],["bool","false"])
            # Int ?
            elif arg2[1] == "int":
                # Compare values
                if arg2[2] > arg3[2]:
                    self.SetVariable(arg1[0],["bool","true"])
                else:
                    self.SetVariable(arg1[0],["bool","false"])
            # Error
            else:
                pass

        # AND
        if inst == "AND":
            # Check if first is variable
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True

            # Check if both are none
            if arg2[1] is None or arg3[1] is None:
                # Return error uninitialized
                self.end = 56
                return True

            # Check if the same type
            if not arg2[1] == arg3[1]:
                self.end = 53
                return True
            # Check if the type is bool
            if arg2[1] == "bool":
                # If both are true or false
                if arg2[2] == "true" and arg3[2] == "true":
                    self.SetVariable(arg1[0],["bool","true"])
                # If they are not the same
                else:
                    self.SetVariable(arg1[0],["bool","false"])
            # Error
            else:
                self.end = 53
                return True

        # NOT
        if inst == "NOT":
            # Check if first is variable
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True

            # Check if both are none
            if arg2[1] is None:
                # Return error uninitialized
                self.end = 56
                return True

            if arg2[1] == "bool":
                if arg2[2] == "true":
                    self.SetVariable(arg1[0],["bool","false"])
                else:
                    self.SetVariable(arg1[0],["bool","true"])
            else:
                self.end = 53
                return True
        # OR
        if inst == "OR":
            # Check if first is variable
            if arg1[0] is None:
                # Variable does not exist
                self.end = 54
                return True

            # Check if both are none
            if arg2[1] is None or arg3[1] is None:
                # Return error uninitialized
                self.end = 56
                return True

            # Check if the same type
            if not arg2[1] == arg3[1]:
                self.end = 53
                return True
            # Check if the type is bool
            if arg2[1] == "bool":
                # If both are true or false
                if arg2[2] == "false" and arg3[2] == "false":
                    self.SetVariable(arg1[0],["bool","false"])
                # If they are not the same
                else:
                    self.SetVariable(arg1[0],["bool","true"])
            # Error
            else:
                self.end = 53
                return True

        # Type instruction
        if inst == "TYPE":
            # Check if variable is set
            if arg1[0] is None:
                # No then error
                self.end = 56
                return True
            # Check if variable is inicialized
            if arg2[1] is None:
                self.SetVariable(arg1[0],["string",""])
                return True
            # Check if TYPE of arg2 is STRING | BOOL | INT | NIL
            self.SetVariable(arg1[0],["string",str(arg2[1])])
            return True

        # CREATEFRAME
        if inst == "CREATEFRAME":
            self.TEMPORALY_FRAME = {}
            return True

        # PUSHFRAME
        if inst == "PUSHFRAME":
            # If TF is not defined
            if self.TEMPORALY_FRAME == None:
                # TF not defined exit
                self.end = 55
                return True
            # Append temporaly frame to the LF_list and clear TF
            self.LF_list.append(self.TEMPORALY_FRAME)
            self.TEMPORALY_FRAME = None
            return True

        # POPFRAME
        if inst == "POPFRAME":
            # Check if there is LF
            if len(self.LF_list) == 0:
                # LF does not exist
                self.end = 55
                return True
            # Pop into TF
            self.TEMPORALY_FRAME = self.LF_list.pop()
            return True

        # INT2CHAR
        if inst == "INT2CHAR":
            # Check if variable is set
            if arg1[0] is None:
                # No then error
                self.end = 56
                return True
            # Check if arguent is of type int
            if not arg2[1] == "int":
                self.end = 53
                return True
            # Try to convert to char
            try:
                character = chr(int(arg2[2]))
            except Exception as e:
                self.end = 58
                return True
            # If it succeded SetVariable
            self.SetVariable(arg1[0],["string",character])
            return True

        if inst == "STRI2INT":
            # Check if variable is set
            if arg1[0] is None:
                # No then error
                self.end = 56
                return True
            # Check if arguent is of type string
            if not arg2[1] == "string":
                self.end = 53
                return True
            # Check if arguent is of type int
            if not arg3[1] == "int":
                self.end = 53
                return True

            # Check if arg3 is inside of string arg2
            if arg3[2] >= len(arg2[2]):
                print("Index out of range",file=sys.stderr)
                self.end = 58
                return True

            # Getchar
            character = arg2[2][arg3[2]]

            # Try to convert into ord
            try:
                character = ord(character)
            except Exception as e:
                self.end = 58
                return True

            # Set variable
            self.SetVariable(arg1[0],["int",int(character)])
            return True

        # BREAK
        if inst == "BREAK":
            # Print current instruction order
            print("Current line:",self.index, file=sys.stderr)

            # Print executed instructions
            print("Number of executed instructions:",self.executed_instructions, file=sys.stderr)
            # Print GF
            print("\nGlobal Frame:",file=sys.stderr)
            # Check if GF is empty
            if len(self.GLOBAL_FRAME.keys()) == 0:
                pass
            else:
                # For each variable print
                # variable => type : value
                for value in self.GLOBAL_FRAME:
                    print("GF@"+str(value),file=sys.stderr,end='')
                    if self.GLOBAL_FRAME[value] is None:
                        print("()",file=sys.stderr)
                    else:
                        print("="+str(self.GLOBAL_FRAME[value][1]) + "(" + str(self.GLOBAL_FRAME[value][0])+")",file=sys.stderr)

            print("\nLocal Frame:",file=sys.stderr)
            # Check if LF is defined
            if len(self.LF_list) == 0:
                pass
            else:
                if len(self.LF_list[-1].keys()) == 0:
                    pass
                else:
                    # For each variable print
                    # variable => type : value
                    for value in self.LF_list[-1]:
                        print("LF@"+str(value),file=sys.stderr,end='')
                        if self.LF_list[-1][value] is None:
                            print("()",file=sys.stderr)
                        else:
                            print("="+str(self.LF_list[-1][value][1]) + "(" + str(self.LF_list[-1][value][0])+")",file=sys.stderr)

            # Print what contain TF
            print("\nTemporary Frame:",file=sys.stderr)
            # Check if TF is defined
            if self.TEMPORALY_FRAME is None:
                pass
            else:
                # Check if there is atleast some variables
                if len(self.TEMPORALY_FRAME.keys()) == 0:
                    pass
                else:
                    # For each variable print
                    # variable => type : value
                    for value in self.TEMPORALY_FRAME:
                        print("TF@"+str(value),file=sys.stderr,end='')
                        if self.TEMPORALY_FRAME[value] is None:
                            print("()",file=sys.stderr)
                        else:
                            print("="+str(self.TEMPORALY_FRAME[value][1]) + "(" + str(self.TEMPORALY_FRAME[value][0]) + ")",file=sys.stderr)

            # Print whats inside stack
            print("\nStack:",file=sys.stderr)
            for value in self.stack:
                print("Stack@="+str(value[1])+"("+str(value[0]) +")",file=sys.stderr)

            return True

        # DPRINT
        if inst == "DPRINT":
            print(type(arg1[2]),file=sys.stderr)
            # Check if variable is set
            if not arg1[0] is None:
                print(arg1[0]," => ",end='',file=sys.stderr)

            # Check if variable is inicialized
            if arg1[1] is None:
                print("Uninitialized",file=sys.stderr)
            else:
                print(arg1[1]," : ",arg1[2],file=sys.stderr)

        # ROZSIRENIE
        # Clears ?
        if inst == "CLEARS":
            # Clears stack
            self.stack = []
            return True

        # ADDS ?
        if inst == "ADDS" or inst == "SUBS" or inst == "MULS" or inst == "IDIVS":
            # Check if there are atleast 2 values in stack
            if len(self.stack) < 2:
                self.end = 56
                return True

            # Get both values
            symb2 = self.stack.pop()
            symb1 = self.stack.pop()

            # Check if both are ints
            if not symb2[0] == "int" or not symb1[0] == "int":
                self.end = 53
                return True

            # ADD ?
            if inst == "ADDS":
                # Add them together
                self.stack.append(["int",int(symb1[1]) + int(symb2[1])])
            # SUBS ?
            elif inst == "SUBS":
                # Subtract one from the other
                self.stack.append(["int",int(symb1[1]) - int(symb2[1])])
            # MULS ?
            elif inst == "MULS":
                # Multiply each other
                self.stack.append(["int",int(symb1[1]) * int(symb2[1])])
            # IDIVS ?
            else:
                # Divide symb1 by symb2
                self.stack.append(["int",int(symb1[1]) // int(symb2[1])])

            return True

        # JUMPIFEQS and JUMPIFNEQS ?
        if inst == "JUMPIFEQS" or inst == "JUMPIFNEQS":
            # Check if label exist
            if not arg1[2] in self.label_list:
                self.end = 52
                return True

            # Check if there are at least 2 items
            if len(self.stack) < 2:
                self.end = 56
                return True

            # Get both variables from stack
            symb2 = self.stack.pop()
            symb1 = self.stack.pop()

            # Check if types are the same
            if not symb2[0] == symb1[0]:
                    self.end = 53
                    return True


            if inst == "JUMPIFEQS":
                # Types are the same
                if symb2[1] == symb1[1]:
                    # Execute jump
                    self.index = int(self.label_list[arg1[2]]) - 1
            else:
                # Types are the same
                if not symb2[1] == symb1[1]:
                    # Execute jump
                    self.index = int(self.label_list[arg1[2]]) - 1
            return True

        # NOTS ?
        if inst == "NOTS":
            # Atleast 1 item
            if len(self.stack) < 1:
                self.end = 56
                return True

            # Get last item
            symb = self.stack[-1]

            # Check if bool
            if symb[0] == "bool":
                self.stack.pop()
                if symb[1].upper() == "TRUE":
                    self.stack.append(["bool","false"])
                else:
                    self.stack.append(["bool","true"])
                return True
            # Wrong type
            self.end = 53
            return True

        # ANDS ?
        if inst == "ANDS":
            if len(self.stack) < 2:
                self.end = 56
                return True

            # Get symbols
            symb2 = self.stack.pop()
            symb1 = self.stack.pop()

            if symb2[0] == "bool" and symb1[0] == "bool":
                if symb2[1].upper() == symb1[1].upper() == "TRUE":
                    self.stack.append(["bool","true"])
                else:
                    self.stack.append(["bool","false"])
            else:
                self.end = 53
            return True

        # ORS ?
        if inst == "ORS":
            if len(self.stack) < 2:
                self.end = 56
                return True

            # Get symbols
            symb2 = self.stack.pop()
            symb1 = self.stack.pop()

            if symb2[0] == "bool" and symb1[0] == "bool":
                if symb2[1].upper() == "TRUE" or symb1[1].upper() == "TRUE":
                    self.stack.append(["bool","true"])
                else:
                    self.stack.append(["bool","false"])
            else:
                self.end = 53
            return True

        # GTS or LTS ?
        if inst == "GTS" or inst == "LTS":
            if len(self.stack) < 2:
                self.end = 56
                return True

            symb2 = self.stack.pop()
            symb1 = self.stack.pop()

            # Not the same type ?
            if not symb2[0] == symb1[0]:
                self.end = 53
                return True

            # String ?
            if symb1[0] == "string":
                symb2_count = 0
                symb1_count = 0
                for char in symb2[1]:
                    symb2_count += ord(char)

                for char in symb1[1]:
                    symb1_count += ord(char)

                if inst == "GTS":
                    if symb1_count > symb2_count:
                        self.stack.append(["bool","true"])
                    else:
                        self.stack.append(["bool","false"])
                else:
                    if symb1_count < symb2_count:
                        self.stack.append(["bool","true"])
                    else:
                        self.stack.append(["bool","false"])
            # Int ?
            elif symb1[0] == "int":
                if inst == "GTS":
                    if symb1[1] > symb2[1]:
                        self.stack.append(["bool","true"])
                    else:
                        self.stack.append(["bool","false"])
                else:
                    if symb1[1] < symb2[1]:
                        self.stack.append(["bool","true"])
                    else:
                        self.stack.append(["bool","false"])
            # Bool ?
            elif symb1[0] == "bool":
                if inst == "GTS":
                    if symb1[1].upper() == "TRUE" and symb2[1].upper() == "FALSE":
                        self.stack.append(["bool","true"])
                    else:
                        self.stack.append(["bool","false"])
                else:
                    if symb1[1].upper() == "FALSE" and symb2[1].upper() == "TRUE":
                        self.stack.append(["bool","true"])
                    else:
                        self.stack.append(["bool","false"])
            # Error
            else:
                self.end = 53
            return True

        # EQS ?
        if inst == "EQS":
            if len(self.stack) < 2:
                self.end = 56
                return True

            symb2 = self.stack.pop()
            symb1 = self.stack.pop()

            # Not the same type ?
            if not symb1[0] == symb2[0]:
                if symb1[0] == "nil" or symb2[0] == "nil":
                    self.stack.append(["bool","false"])
                    return True
                self.end = 53
                return True

            # String or Bool or Int or Nil ?
            if symb2[0] == "string" or symb2[0] == "bool" or symb2[0] == "int" or symb2[0] == "nil":
                if symb2[1] == symb1[1]:
                    self.stack.append(["bool","true"])
                else:
                    self.stack.append(["bool","false"])
                return True
            # Some other type error
            else:
                self.end = 53
                return True

        if inst == "INT2CHARS":
            # Check if there are atleast 1 item
            if len(self.stack) < 1:
                self.end = 56
                return True

            symb = self.stack.pop()

            # Check if arguent is of type int
            if not symb[0] == "int":
                self.end = 53
                return True
            # Try to convert to char
            try:
                character = chr(int(symb[1]))
            except Exception as e:
                self.end = 58
                return True
            # If it succeded SetVariable
            self.stack.append(["string",character])
            return True

        if inst == "STRI2INTS":

            if len(self.stack) < 2:
                self.end = 56
                return True

            symb2 = self.stack.pop()
            symb1 = self.stack.pop()

            # Check if arguent is of type string
            if not symb1[0] == "string":
                self.end = 53
                return True
            # Check if arguent is of type int
            if not symb2[0] == "int":
                self.end = 53
                return True

            # Check if arg3 is inside of string arg2
            if symb2[1] >= len(symb1[1]):
                print("Index out of range",file=sys.stderr)
                self.end = 58
                return True

            # Getchar
            character = symb1[1][symb2[1]]

            # Try to convert into ord
            try:
                character = ord(character)
            except Exception as e:
                self.end = 58
                return True

            # Set variable
            self.stack.append(["int",int(character)])
            return True


        return True

    # Load all labels and their indexes
    def LoadLabels(self):
        # Loop through every instruction in program
        for value in self.program:
            # If the instruction is LABEL
            if self.program[value]["instruction"].upper() == "LABEL":
                # Get the name of label
                index = self.program[value]["arg1"][1]
                # Check if we are trying to redefine LABEL
                if index in self.label_list:
                    print("Redefinicia labelu",file=sys.stderr)
                    # Redefining label
                    return False
                # Add new label
                self.label_list[index] = value
        # Everything went well
        return True

    # Get value of variable
    def GetVarValue(self,argument):
        # Check if there is no argument
        if len(argument) == 0:
            return [None,"",""]
        # Check if this is variable
        if argument[0].upper() == "VAR":
            # Get variable from given dictionary
            frame = argument[1].split("@")
            variable = frame[1]
            frame = frame[0]

            # Variable is in TF
            if frame == "TF":
                # Check if TF is defined
                if self.TEMPORALY_FRAME is None:
                    self.end = 55
                    return None
                # Check if variable exist in TF
                if not variable in self.TEMPORALY_FRAME:
                    self.end = 54
                    return None
                # Check if VAR is defined
                if self.TEMPORALY_FRAME[variable] is None:
                    # Then return undefined variable
                    return [argument[1],None,None]
                #  [variable,type,value]
                return [argument[1],self.TEMPORALY_FRAME[variable][0],self.TEMPORALY_FRAME[variable][1]]
            elif frame == "LF":
                # Check
                if not len(self.LF_list) > 0:
                    self.end = 55
                    return None
                if not variable in self.LF_list[-1]:
                    self.end = 54
                    return None
                # Check if var is defined
                if self.LF_list[-1][variable] is None:
                    # Return undefined variable
                    return [argument[1],None,None]
                #  [variable,type,value]
                return [argument[1],self.LF_list[-1][variable][0],self.LF_list[-1][variable][1]]
            elif frame == "GF":
                # Check if variable exist
                if not variable in self.GLOBAL_FRAME:
                    self.end = 54
                    # No it does not
                    return None
                # Check if variable is defined
                if self.GLOBAL_FRAME[variable] is None:
                    # Return undefined variable
                    return [argument[1],None,None]
                #  [variable,type,value]
                return [argument[1],self.GLOBAL_FRAME[variable][0],self.GLOBAL_FRAME[variable][1]]
            else:
                self.end = 55
                # Undefined frame
                return None;
        # Not variable
        return [None,argument[0],argument[1]]

    # Define variable based on label
    def DefineVariable(self,variable):
        # Split on @
        frame = variable.split("@")

        # We need to have list of 2 values
        if not len(frame) == 2:
            # No ? Failed
            return False

        # First is frame and second is name of variable
        variable = frame[1]
        frame = frame[0]

        # Define variable in TEMPORALY_FRAME
        if frame == "TF":
            # Check if temporaly frame exist
            if self.TEMPORALY_FRAME is None:
                return False
            # Check for redefinition
            if variable in self.TEMPORALY_FRAME:
                return False
            # Define variable in TF
            self.TEMPORALY_FRAME[variable] = None
        # Define variable in LOCAL_FRAME
        elif frame == "LF":
            # Is there atleast 1 LocalFrame ?
            if not len(self.LF_list) > 0:
                return False
            # Check for redefinition
            if variable in self.LF_list[-1]:
                return False
            # Define variable in LF
            self.LF_list[-1][variable] = None
        # Define variable in GLOBAL_FRAME
        elif frame == "GF":
            # Check for redefinition
            if variable in self.GLOBAL_FRAME:
                return False
            # Define variable in GF
            self.GLOBAL_FRAME[variable] = None
        # Some other unknown FRAME
        else:
            # Return error
            return False
        # OK
        return True

    # Set into variable value
    def SetVariable(self,variable,value):
        # Get frame and variable
        frame = variable.split("@")[0]
        variable = variable.split("@")[1]

        # Check TF
        if frame == "TF":
            # Check if frame exist
            if self.TEMPORALY_FRAME is None:
                self.end = 55
                return
            # Check if variable exist in TF
            if not variable in self.TEMPORALY_FRAME:
                self.end = 54
                return
            # Set variable
            self.TEMPORALY_FRAME[variable] = value
        # Check LF
        elif frame == "LF":
            # Check if there is some LF on the stack
            if len(self.LF_list) == 0:
                self.end = 55
                return
            # Check if variable exist
            if not variable in self.LF_list[-1]:
                self.end = 54
                return
            # Set variable
            self.LF_list[-1][variable] = value
        # Check GF
        elif frame == "GF":
            # Check if variable exist
            if not variable in self.GLOBAL_FRAME:
                self.end = 54
                return
            # Set variable
            self.GLOBAL_FRAME[variable] = value
        # 
        else:
            self.end = 54
            return
