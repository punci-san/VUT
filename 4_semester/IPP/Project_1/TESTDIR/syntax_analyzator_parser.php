<?php

include_once 'lexical_parser.php';

class SyntaxAnalyzator
{
  var $lexicalClass = NULL;
  var $syntaxClass = NULL;
  var $fileArray = NULL;

  function __construct()
  {
    $this->lexicalClass = new LexicalAnalyzator();
    $this->syntaxClass = new Syntax();
    $this->fileArray = array();
  }

  public function generateXML()
  {
    /// Create IPPcode19 header
    $xml = new SimpleXMLElement('<?xml version="1.0" encoding="UTF-8" ?><program/>');
    $xml->addAttribute("language","IPPcode19");

    $instruction_counter = 1;
    $inst = "";
    $argcounter = 1;

    // Start reading from first instruction
    for ($i=0; $i < count($this->fileArray); $i++)
    {
      switch ($this->fileArray[$i]->type)
      {
        // Instruction
        case 'instruction':
            $inst = $xml->addChild($this->fileArray[$i]->type);
            $inst->addAttribute("order",$instruction_counter);
            $inst->addAttribute("opcode",strtoupper($this->fileArray[$i]->string));
          break;

        case 'EOL':
          $inst = NULL;
          $instruction_counter++;
          $argcounter = 1;
        break;
        // Others are only arguments here
        default:
          $arg = $inst->addChild("arg".$argcounter,$this->fileArray[$i]->string);
          $arg->addAttribute("type",$this->fileArray[$i]->type);

          $argcounter++;
          break;
      }
    }

    $dom = dom_import_simplexml($xml)->ownerDocument;
    $dom->formatOutput = true;
    echo $dom->saveXML($dom,LIBXML_NOEMPTYTAG);
  }

  private function AppendToken($token)
  {
    array_push($this->fileArray,$token);
  }
  /*
   <symb> = string,int,bool,gf,lf,tf
  */
  private function ParseArgument($requested_syntax,$token)
  {
    $argument_type = $token->type;
    // If we require symb
    if ($requested_syntax == "symb")
    {
      // argument can be one of those
      switch ($argument_type)
      {
        case 'string':
        case 'int':
        case 'bool':
        case 'nil':
        case 'var':
          return true;
          break;

        default:
          return false;
          break;
      }
    }
    // If we require var
    else if ($requested_syntax == "var")
    {
      if ($argument_type == $requested_syntax)
      {
        return true;
      }
      return false;
    }
    // If we require label
    else if ($requested_syntax == "label")
    {
      if ($argument_type == "label" || $argument_type == "instruction")
      {
        $token->type = "label";
        return true;
      }
      return false;
    }
    // If we requre nothing that means that there needs to be EOL
    else if ($requested_syntax == "EOL")
    {
      if ($argument_type == "EOL")
      {
        return true;
      }
      return false;

    }
    // If we require type
    else if ($requested_syntax == "type")
    {
      if ($argument_type == $requested_syntax)
      {
        return true;
      }
      return false;
    }
    else
    {
      return false;
    }
    return false;
  }

  private function ParseInstruction($arg)
  {
    if ($arg->type != "instruction")
    {
      return 22;
    }
    $token = $arg;
    $syntax_inst = $this->syntaxClass->getInstructionSyntax($token->string);

    // If we didnt found instruction return error
    if ($syntax_inst == FALSE)
    {
      return 23;
    }

    $this->AppendToken($token);

    // 3 is the max number of arguments in instruction
    for ($i=0; $i < 4; $i++)
    {
      // We get the token
      $token = $this->lexicalClass->getToken();

      // Lexical error is 23 error
      if ($token->type == "error")
      {
        return 23;
      }

      // If is in
      if (array_key_exists($i,$syntax_inst))
      {
        $syntax_requirement = $syntax_inst[$i];

        // If there is EOL this needs to be last check
        if ($syntax_requirement == "EOL")
        {
          $i = 5;
        }

        if($this->ParseArgument($syntax_requirement,$token) == FALSE)
        {
          return 23;
        }
        $this->AppendToken($token);
      }
      else
      {
        return 23;
      }
    }
    return 0;
  }

  public function Parse()
  {
    $result = 0;
    // Lets test first token
    $token = $this->lexicalClass->getToken();

    // Is first header ?
    if ($token->type != "header" || strtoupper($token->string) != ".IPPCODE19")
    {
      return 21;
    }

    // Get another
    $token = $this->lexicalClass->getToken();

    // If is then is the next EOL ?
    if ($token->type != "EOL" && $token->string != "\n")
    {
      return 21;
    }

    // Get next token
    $token = $this->lexicalClass->getToken();

    // Get new tokens until you found EOF or error
    while ($token->type != "EOF" || $token->type != "error")
    {
      // If instruction
      if ($token->type == "instruction")
      {
        if (($result = $this->ParseInstruction($token)) != 0)
        {
          return $result;
        }

      }
      // if EOL
      else if ($token->type == "EOL")
      {
        // Just ignore and get new token
      }
      // Or can be EOF
      else if ($token->type == "EOF")
      {
        return 0;
      }
      // If there is something else its probably syntax error
      else
      {
        return 22;
      }

      // Get next token
      $token = $this->lexicalClass->getToken();
    }

    return 0;
  }

  public function TestLexical()
  {
    $token = $lexicalClass->getToken();

    while ($token->type != "EOF")
    {
      $token = $lexicalClass->getToken();
    }

  }
}
