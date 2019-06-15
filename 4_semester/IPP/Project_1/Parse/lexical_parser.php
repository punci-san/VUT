<?php

include_once 'syntax_parser.php';

class Token
{
  var $type;
  var $string;

  function __construct($_type,$_string)
  {
    $this->type = $_type;
    $this->string = $_string;
  }
}

class LexicalAnalyzator
{
  var $syntax_class = "";
  var $stdin = NULL;
  var $eof_reached = FALSE;
  var $last_char = "";

  // Open reading from stdin
  function __construct()
  {
    $this->syntax_class = new Syntax();
    $this->stdin = fopen('php://stdin', 'r');
    $this->eof_reached = false;
  }

  function __deconstruct()
  {
    fclose($this->stdin);
  }

  private function isEnd($character)
  {
    if (ord($character) == ord("\t") || ctype_space($character) || ord($character) == ord("#"))
    {
      $this->last_char = $character;
      return true;
    }
    elseif (ord($character) == ord("\n"))
    {
      $this->last_char = $character;
      return true;
    }
    return false;
  }

  private function lexicalError()
  {
    return new Token("error","lexical");
  }

  private function isIgnoreChar($character)
  {
    if (ord($character) == ord("\t") || ord($character) == ord(" "))
    {
      return true;
    }
    return false;
  }

  private function isInstruction($string)
  {
    return $this->syntax_class->isInstruction(strtoupper($string));
  }

  private function isType($string)
  {
    if ($string == "string" || $string == "int" || $string == "bool" || $string == "nil")
    {
      return true;
    }
    return false;
  }

  private function isLabel($string)
  {
    if (!($this->isSpecialChar($string[0]) || ctype_alpha($string[0])))
    {
      return false;
    }
    for ($i=1; $i < strlen($string); $i++)
    {
      if (!($this->isSpecialChar($string[$i]) || ctype_digit($string[$i]) || ctype_alpha($string[$i])))
      {
        return false;
      }
    }
    return true;
  }

  private function isSpecialChar($character)
  {
    if (ord($character) == ord("&") || ord($character) == ord("_") || ord($character) == ord("-") || ord($character) == ord("$") || ord($character) == ord("%") || ord($character) == ord("*") || ord($character) == ord("!") || ord($character) == ord("?"))
    {
      return true;
    }
    return false;
  }

  public function getToken()
  {
    $state = "default";
    $char = '';
    $special_char = "";
    $string = "";

    while(true)
    {

      // If we get
      if (feof($this->stdin))
      {
        // If we are not in default
        if ($state != 'default')
        {
          return $this->lexicalError();
        }
        return new Token("EOF","EOF");
      }

      if (strlen($this->last_char) != 0)
      {
        $char = $this->last_char;
        $this->last_char = "";
      }
      else
      {
        // getchar
        $char = fgetc($this->stdin);
      }

      switch ($state)
      {
        default:
        case 'default':
          // Is start of Comment ?
          if (ord($char) == ord('#'))
          {
            $state = 'comment';
            continue 2;
          }

          if ($this->isIgnoreChar($char))
          {
            $string = "";
            continue 2;
          }

          // Is it header ?
          if (ord($char) == ord('.'))
          {
            $string = $char;
            $state = 'header';
            continue 2;
          }

          // Is EOL ?
          if (ord($char) == ord("\n"))
          {
            return new Token("EOL",$char);
          }

          // Is character ?
          if (ctype_alpha($char))
          {
            // Append to string
            $string = $string.$char;
            $state = 'type_determination';
            continue 2;
          }

          if (ctype_digit($char))
          {
            $string = $string.$char;
            $state = 'integer';
            continue 2;
          }

          if ($this->isSpecialChar($char))
          {
            $string = $string.$char;
            $state = 'type_determination';
            continue 2;
          }

          // fgetc return FALSE if it reaches EOF
          if ($char == FALSE)
          {
            return new Token("EOF","EOF");
          }

          return $this->lexicalError();

          break;

        case 'type_determination':
          // If we found @ determinate type of variable
          if (ord($char) == ord('@'))
          {
            // Is it string ?
            if ($string == "string")
            {
              $state = 'string';
            }
            // Is it integer ?
            else if ($string == "int")
            {
              $state = 'integer';
            }
            // Is it boolean ?
            else if ($string == "bool")
            {
              $state = 'boolean';
            }
            // Is it nil ?
            else if ($string == "nil")
            {
              $state = 'nil';
            }
            // Is it some type of variable ?
            else if ($string == "GF" ||$string == "LF" || $string == "TF")
            {
              $string = $string.$char;
              $state = 'var';
              continue 2;
            }
            // It is lexical error
            else
            {
              return $this->lexicalError();
            }
            $string = "";
            continue 2;
          }
          // Je koniec ?
          if ($this->isEnd($char))
          {
            // Check if it is instruction or label name
            if ($this->isInstruction($string))
            {
              return new Token("instruction",$string);
            }
            // Check if it is
            if ($this->isType($string))
            {
              return new Token("type",$string);
            }
            // Check label
            if ($this->isLabel($string))
            {
              // Label name
              return new Token("label",$string);
            }
            return $this->lexicalError();
          }
          $string = $string.$char;
          break;

        case 'var':
          if (ctype_alpha($char) || $this->isSpecialChar($char))
          {
            if (ord($char) == ord("&"))
            {
              $char = "&amp;";
            }
            $string = $string.$char;
            $state = 'var_1';
            continue 2;
          }
          return $this->lexicalError();

          break;

        case 'var_1':
          if ($this->isEnd($char))
          {
            return new Token("var",$string);
          }
          else if (ctype_alpha($char) || ctype_digit($char) || $this->isSpecialChar($char))
          {
            $string = $string.$char;
          }
          else
          {
            return $this->lexicalError();
          }
          break;

        case 'nil':
          if (strlen($string) == 3)
          {
            if ($string == "nil" && $this->isEnd($char))
            {
              return new Token("nil",$string);
            }
            return $this->lexicalError();
          }
          $string = $string.$char;
          break;

        case 'string':
          // If # or " " return error
          if ($this->isEnd($char))
          {
            return new Token("string",$string);
          }

          // White character from 0 to 32
          if (ord($char) >= 0 && ord($char) <= ord(" "))
          {
            return $this->lexicalError();
          }
          // If its \
          if (ord($char) == ord("\\"))
          {
            $special_char = "";
            $state = 'string_1';
            continue 2;
          }
          // < ?
          if (ord($char) == ord("<"))
          {
            $char = "&lt;";
          }
          // > ?
          else if (ord($char) == ord(">"))
          {
            $char = "&gt;";
          }
          // & ?
          else if (ord($char) == ord("&"))
          {
            $char = "&amp;";
          }
          $string = $string.$char;
          break;

        case 'string_1':
          if (!ctype_digit($char))
          {
            return $this->lexicalError();
          }
          $special_char = $special_char.$char;
          $state = 'string_2';
          break;

        case 'string_2':
          if (!ctype_digit($char))
          {
            return $this->lexicalError();
          }
          $special_char = $special_char.$char;
          $state = 'string_3';
          break;

        case 'string_3':
          if (!ctype_digit($char))
          {
            return $this->lexicalError();
          }
          $special_char = $special_char.$char;

          // If its out of range
          if ($special_char < 0 || $special_char > 999)
          {
            return $this->lexicalError();
          }



          $state = 'string';
          $string = $string."\\".$special_char;
          continue 2;
          break;

        case 'integer':
          // Prvy char moze byt but cislo alebo - alebo +
          if (ord($char) == ord('-') || ord($char) == ord('+'))
          {
            $string = $char;
            $state = 'integer_1';
            continue 2;
          }
          else if (ctype_digit($char))
          {
            $string = $char;
            $state = 'integer_1';
            continue 2;
          }
          else
          {
            return $this->lexicalError();
          }
          break;

        case 'integer_1':
          // Is it digit ?
          if (ctype_digit($char))
          {
            $string = $string.$char;
            continue 2;
          }
          // Is it end ?
          else if ($this->isEnd($char))
          {
            if (strlen($string) == 1 && ($string == "-" || $string == "+"))
            {
              return $this->lexicalError();
            }
            return new Token("int",$string);
          }
          // Wrong character
          else
          {
            return $this->lexicalError();
          }
          break;

        // Bool can be either true or false
        case 'boolean':
          // Is it true ?
          if (strlen($string) == 4)
          {
            // Is true ?
            if ($string == "true")
            {
              // Is next char end ?
              if ($this->isEnd($char))
              {
                return new Token("bool",$string);
              }
            }
            // Is it fals ?
            else if ($string == "fals")
            {
              // Then append and continue;
              $string = $string.$char;
              continue 2;
            }
            // No ? Exit error
            return $this->lexicalError();
          }
          // Is it false ?
          else if (strlen($string) == 5)
          {
            // Is false ?
            if ($string == "false")
            {
              // Is next char end ?
              if ($this->isEnd($char))
              {
                return new Token("bool",$string);
              }
            }
            // No ? Exit error
            return $this->lexicalError();
          }
          else if (strlen($string) > 5)
          {
            return $this->lexicalError();
          }
          // Append char
          $string = $string.$char;
          break;

        case 'comment':
          // End of line
          if (ord($char) == ord("\n"))
          {
            return new Token("EOL",$char);
          }
          continue 2;
          break;

        case 'header':
          // did we found EOL
          if ($this->isEnd($char))
          {
            $this->eol = true;
            return new Token("header",$string);
          }
          $string = $string.$char;
        break;
      }
    }
  }
}
