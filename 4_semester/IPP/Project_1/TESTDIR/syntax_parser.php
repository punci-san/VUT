<?php

class Syntax
{
  var $syntax_array;
  var $instructions_array;

  function __construct()
  {
    $this->instructions_array = array(
      // Basic functions
      "MOVE",
      "CREATEFRAME",
      "PUSHFRAME",
      "POPFRAME",
      "DEFVAR",
      "CALL",
      "RETURN",
      // Data zasobnik
      "PUSHS",
      "POPS",
      // Aritmetics
      "ADD",
      "SUB",
      "MUL",
      "IDIV",
      "LT",
      "GT",
      "EQ",
      "AND",
      "OR",
      "NOT",
      "INT2CHAR",
      "STRI2INT",
      // Input/Output instructions
      "READ",
      "WRITE",
      // String instructions
      "CONCAT",
      "STRLEN",
      "GETCHAR",
      "SETCHAR",
      // Type instructions
      "TYPE",
      // Control instructions
      "LABEL",
      "JUMP",
      "JUMPIFEQ",
      "JUMPIFNEQ",
      "EXIT",
      // Setting instructions
      "DPRINT",
      "BREAK"
    );

    // symb = symb or var
    $this->syntax_array = array(
      // Basic functions
      "MOVE" => array("var","symb","EOL",""),
      "CREATEFRAME" => array("EOL","","",""),
      "PUSHFRAME" => array("EOL","","",""),
      "POPFRAME" => array("EOL","","",""),
      "DEFVAR" => array("var","EOL","",""),
      "CALL" => array("label","EOL","",""),
      "RETURN" => array("EOL","","",""),
      // Data zasobnik
      "PUSHS" => array("symb","EOL","",""),
      "POPS" => array("var","EOL","",""),
      // Aritmetics
      "ADD" => array("var","symb","symb","EOL"),
      "SUB" => array("var","symb","symb","EOL"),
      "MUL" => array("var","symb","symb","EOL"),
      "IDIV" => array("var","symb","symb","EOL"),
      "LT" => array("var","symb","symb","EOL"),
      "GT" => array("var","symb","symb","EOL"),
      "EQ" => array("var","symb","symb","EOL"),
      "AND" => array("var","symb","symb","EOL"),
      "OR" => array("var","symb","symb","EOL"),
      "NOT" => array("var","symb","EOL",""),
      "INT2CHAR" => array("var","symb","EOL",""),
      "STRI2INT" => array("var","symb","symb","EOL"),
      // Input/Output instructions
      "READ" => array("var","type","EOL",""),
      "WRITE" => array("symb","EOL","",""),
      // String instructions
      "CONCAT" => array("var","symb","symb","EOL"),
      "STRLEN" => array("var","symb","EOL",""),
      "GETCHAR" => array("var","symb","symb","EOL"),
      "SETCHAR" => array("var","symb","symb","EOL"),
      // Type instructions
      "TYPE" => array("var","symb","EOL",""),
      // Control instructions
      "LABEL" => array("label","EOL","",""),
      "JUMP" => array("label","EOL","",""),
      "JUMPIFEQ" => array("label","symb","symb","EOL"),
      "JUMPIFNEQ" => array("label","symb","symb","EOL"),
      "EXIT" => array("symb","EOL","",""),
      // Setting instructions
      "DPRINT" => array("symb","EOL","",""),
      "BREAK" => array("EOL","","","")
    );
  }

  public function isInstruction($string)
  {
    for ($i=0; $i < count($this->instructions_array); $i++)
    {
      if (strtoupper($this->instructions_array[$i]) == strtoupper($string))
      {
        return true;
      }
    }
    return false;
  }

  public function getInstructionSyntax($string)
  {
    if (array_key_exists(strtoupper($string),$this->syntax_array))
    {
      return $this->syntax_array[strtoupper($string)];
    }
    return false;
  }
}
