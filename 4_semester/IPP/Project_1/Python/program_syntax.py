import re
import sys

class SyntaxCheck():

    def __init__(self,program):
        self.program = program
        self.syntax_list = {}
        self.syntax_list["MOVE"] = ["var","symb",""]
        self.syntax_list["CREATEFRAME"] = ["","",""]
        self.syntax_list["PUSHFRAME"] = ["","",""]
        self.syntax_list["POPFRAME"] = ["","",""]
        self.syntax_list["DEFVAR"] = ["var","",""]
        self.syntax_list["CALL"] = ["label","",""]
        self.syntax_list["RETURN"] = ["","",""]
        self.syntax_list["PUSHS"] = ["symb","",""]
        self.syntax_list["POPS"] = ["var","",""]
        self.syntax_list["ADD"] = ["var","symb","symb"]
        self.syntax_list["SUB"] = ["var","symb","symb"]
        self.syntax_list["MUL"] = ["var","symb","symb"]
        self.syntax_list["IDIV"] = ["var","symb","symb"]
        self.syntax_list["LT"] = ["var","symb","symb"]
        self.syntax_list["GT"] = ["var","symb","symb"]
        self.syntax_list["EQ"] = ["var","symb","symb"]
        self.syntax_list["AND"] = ["var","symb","symb"]
        self.syntax_list["OR"] = ["var","symb","symb"]
        self.syntax_list["NOT"] = ["var","symb",""]
        self.syntax_list["INT2CHAR"] = ["var","symb",""]
        self.syntax_list["STRI2INT"] = ["var","symb","symb"]
        self.syntax_list["READ"] = ["var","type",""]
        self.syntax_list["WRITE"] = ["symb","",""]
        self.syntax_list["CONCAT"] = ["var","symb","symb"]
        self.syntax_list["STRLEN"] = ["var","symb",""]
        self.syntax_list["GETCHAR"] = ["var","symb","symb"]
        self.syntax_list["SETCHAR"] = ["var","symb","symb"]
        self.syntax_list["TYPE"] = ["var","symb",""]
        self.syntax_list["LABEL"] = ["label","",""]
        self.syntax_list["JUMP"] = ["label","",""]
        self.syntax_list["JUMPIFEQ"] = ["label","symb","symb"]
        self.syntax_list["JUMPIFNEQ"] = ["label","symb","symb"]
        self.syntax_list["EXIT"] = ["symb","",""]
        self.syntax_list["DPRINT"] = ["symb","",""]
        self.syntax_list["BREAK"] = ["","",""]
        # ROZSIRENIE
        self.syntax_list["CLEARS"] = ["","",""]
        self.syntax_list["ADDS"] = ["","",""]
        self.syntax_list["SUBS"] = ["","",""]
        self.syntax_list["MULS"] = ["","",""]
        self.syntax_list["IDIVS"] = ["","",""]
        self.syntax_list["LTS"] = ["","",""]
        self.syntax_list["GTS"] = ["","",""]
        self.syntax_list["EQS"] = ["","",""]
        self.syntax_list["ANDS"] = ["","",""]
        self.syntax_list["ORS"] = ["","",""]
        self.syntax_list["NOTS"] = ["","",""]
        self.syntax_list["INT2CHARS"] = ["","",""]
        self.syntax_list["STRI2INTS"] = ["","",""]
        self.syntax_list["JUMPIFEQS"] = ["label","",""]
        self.syntax_list["JUMPIFNEQS"] = ["label","",""]

    def Syntax(self):
        i = 1
        # Loop through every instruction in dictionary
        while i <= len(self.program):
            # Get current instruction
            instruction = self.program[str(i)]

            # Check if given instruction exist
            if not (instruction["instruction"]).upper() in self.syntax_list:
                return 32

            # Check if instruction is okay
            if not self.CheckInstruction(instruction):
                return 32
            # Increment pointer
            i += 1
        return 0

    def CheckInstruction(self,instruction):
        # Get syntax for given instruction
        syntax = self.syntax_list[(instruction["instruction"]).upper()]

        # Check for first argument
        if not self.CheckArgument(instruction["arg1"],syntax[0]):
            return False

        # Check for second argument
        if not self.CheckArgument(instruction["arg2"],syntax[1]):
            return False

        # Check for third argument
        if not self.CheckArgument(instruction["arg3"],syntax[2]):
            return False

        # If nothing failed its okay
        return True

    def CheckArgument(self,argument,syntax_arg):
        # If we have emtpy syntax we need to have argument with nothing
        if not syntax_arg:
            # Is argument emty ?
            if not len(argument) == 0:
                # NO ? Error
                return False
            return True

        # If we dont have argument and we need it
        if len(argument) == 0:
            return False
        # Symb ?
        if syntax_arg == "symb":
            # Is it constant or var ? ?
            if not (argument[0] == "string" or argument[0] == "int" or argument[0] == "bool" or argument[0] == "nil" or argument[0] == "var"):
                return False
            # Check lexical and return
            return self.CheckLexical(argument)

        # Variable ?
        if syntax_arg == "var":
            # Argument can be only variable
            if not argument[0] == "var":
                return False
            # Check lexical and return
            return self.CheckLexical(argument)

        # Type ?
        if syntax_arg == "type":

            # Argument can only be type
            if not argument[0] == "type":
                return False
            if argument[1] == "nil":
                return False
            # Check lexical and return
            return self.CheckLexical(argument)

        # Label
        if syntax_arg == "label":
            # Can only be label
            if not argument[0] == "label":
                return False;
            # Check lexical and return
            return self.CheckLexical(argument)

        return False

    def CheckLexical(self,arg):
        # Int
        if arg[0] == "int":
            regex = re.match("^(((\+|-)[0-9])|([0-9]))([0-9])*",arg[1],0);
            # Nothing found
            if regex is None:
                return False

            # Some bad characters might be after
            if not len(regex.group()) == len(arg[1]):
                return False

            # Convert to int
            arg[1] = int(arg[1])
            return True

        # Bool
        if arg[0] == "bool":
            # Can only be true or false
            if not (arg[1] == "true" or arg[1] == "false"):
                return False
            return True

        # Nil
        if arg[0] == "nil":
            # Nil can be only nil
            if not arg[1] == "nil":
                return False
            return True

        # String
        if arg[0] == "string":
            # Check if string matches regex
            regex = re.match("(\\\\\d{3,}|[^\\\\\s|#])*",arg[1],0)
            # Nothing found
            if regex is None:
                print("STRING error",sys.stderr)
                return False

            # Some bad characters might be after
            if not len(regex.group()) == len(arg[1]):
                return False

            string = ""
            index = 0
            # Loop through every character
            while(index < len(arg[1])):
                # If we found \ we need to convert it into ascii value
                if arg[1][index] == "\\":
                    tmp_str = ""
                    # Loop through the next 3 number after \
                    for tmp in range(1,4):
                        tmp_str += arg[1][index + tmp]
                    index += 4
                    # Convert decimal value into ascii
                    string += chr(int(tmp_str))
                    continue
                string += arg[1][index]
                index += 1

            arg[1] = string
            return True

        # Var (GF@|LF@|TF@)(_|-|\$|%|\*|!|\?|[a-zA-Z])[a-zA-Z]*
        if arg[0] == "var":
            # Check if var matches regex
            regex = re.match("(GF@|LF@|TF@)(_|-|\$|%|\*|!|\?|[a-zA-Z])([a-zA-Z0-9]|_|-|\$|%|\*|!|\?)*",arg[1],0)
            # Nothing found
            if regex is None:
                return False

            # Some bad characters might be after
            if not len(regex.group()) == len(arg[1]):
                return False

            return True

        # Label (_|-|\$|%|\*|!|\?|[a-zA-Z])[a-zA-Z0-9]*
        if arg[0] == "label":
            # Check if label matches regex
            regex = re.match("(_|-|\$|%|\*|!|\?|[a-zA-Z])([a-zA-Z0-9]|_|-|\$|%|\*|!|\?)*",arg[1],0)
            # Nothing found
            if regex is None:
                return False
            # Is there some garbage behind matched chars ?
            if not len(regex.group()) == len(arg[1]):
                return False

            return True

        return True
