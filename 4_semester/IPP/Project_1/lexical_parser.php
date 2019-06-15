<?php

include_once 'syntax_parser.php';

// Class to hold identified tokens
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

// Class for lexical state automat
class LexicalAnalyzator
{
  // Variables
  var $syntax_class = "";
  var $stdin = NULL;
  var $eof_reached = FALSE;
  var $last_char = "";

  // Set needed variables
  function __construct()
  {
    $this->syntax_class = new Syntax();
    $this->eof_reached = false;
  }

  // Function will check if we encountered ending char
  private function isEnd($character)
  {
    if (ord($character) == ord("\t") || ctype_space($character) || ord($character) == ord("#") || ord($character) == FALSE || ord($character) == ord("\n"))
    {
      $this->last_char = $character;
      return true;
    }
    return false;
  }

  // Return lexical error
  private function lexicalError()
  {
    return new Token("error","lexical");
  }

  // Function to check if we need to skip some chars
  private function isIgnoreChar($character)
  {
    if (ord($character) == ord("\t") || ord($character) == ord(" "))
    {
      return true;
    }
    return false;
  }

  // Check if given string is one of instructions
  private function isInstruction($string)
  {
    return $this->syntax_class->isInstruction(strtoupper($string));
  }

  // Check if given string is type in READ instructions
  private function isType($string)
  {
    if ($string == "string" || $string == "int" || $string == "bool")
    {
      return true;
    }
    return false;
  }

  // Check if label is correctly typed
  private function isLabel($string)
  {
    // Check if first char is special_char or alpha character
    if (!($this->isSpecialChar($string[0]) || ctype_alpha($string[0])))
    {
      return false;
    }
    // Then loop through every character in string and check if it contains only alphanumeric characters and special chars
    for ($i=1; $i < strlen($string); $i++)
    {
      if (!($this->isSpecialChar($string[$i]) || ctype_digit($string[$i]) || ctype_alpha($string[$i])))
      {
        return false;
      }
    }
    return true;
  }

  // Check if given character is one of the special chars
  private function isSpecialChar($character)
  {
    if (ord($character) == ord("&") || ord($character) == ord("_") || ord($character) == ord("-") || ord($character) == ord("$") || ord($character) == ord("%") || ord($character) == ord("*") || ord($character) == ord("!") || ord($character) == ord("?"))
    {
      return true;
    }
    return false;
  }

  // Function that will return one token at a time
  public function getToken()
  {
    // Variables
    $state = "default";
    $char = '';
    $special_char = "";
    $string = "";

    // Endless loop
    while(true)
    {
      // Did we return something back ?
      if (strlen($this->last_char) != 0)
      {
        // If yes load that char and clear it
        $char = $this->last_char;
        $this->last_char = "";
      }
      // NO ?
      else
      {
        // Getchar from stdin
        $char = fgetc(STDIN);
      }

      // state automat
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

          // Is skip char ?
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

          // Is special char ?
          if ($this->isSpecialChar($char))
          {
            $string = $string.$char;
            $state = 'type_determination';
            continue 2;
          }

          // fgetc return \0 if reaches EOF
          if (ord($char) == FALSE)
          {
            // Reached EOF already ?
            if ($this->eof_reached)
            {
              // Yes then return EOF
              return new Token("EOF","EOF");
            }
            // No ? then return EOL before returning EOF
            $this->eof_reached = true;
            return new Token("EOL","EOL");
          }

          // Everything else is bad return lexical error
          return $this->lexicalError();

          break;

        // Case to determinate which state is next
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
            // No ? Then it is lexical error
            else
            {
              return $this->lexicalError();
            }
            $string = "";
            continue 2;
          }
          // Did we reached end char ?
          if ($this->isEnd($char))
          {
            // Check if it is instruction
            if ($this->isInstruction($string))
            {
              return new Token("instruction",$string);
            }
            // Check if it is type
            if ($this->isType($string))
            {
              return new Token("type",$string);
            }
            // Check if it is label
            if ($this->isLabel($string))
            {
              // Label name
              return new Token("label",$string);
            }
            // Nothing ? Then error
            return $this->lexicalError();
          }
          // Append char
          $string = $string.$char;
          break;

        // Variable [GF|LF|TF]
        case 'var':
          // First char in variable can be special or alpha character
          if (ctype_alpha($char) || $this->isSpecialChar($char))
          {
            $string = $string.$char;
            $state = 'var_1';
            continue 2;
          }
          // SOmething else ? Lexical error
          return $this->lexicalError();

          break;

        // Others variable characters
        case 'var_1':
          // Did we reached end char ?
          if ($this->isEnd($char))
          {
            // Before we return variable we need to escape special chars
            $string = htmlspecialchars($string, ENT_XML1 | ENT_QUOTES, 'UTF-8');
            return new Token("var",$string);
          }
          // Chars can only be alpha numeric and special chars
          else if (ctype_alpha($char) || ctype_digit($char) || $this->isSpecialChar($char))
          {
            $string = $string.$char;
          }
          // If there is something else
          else
          {
            // Return lexical error
            return $this->lexicalError();
          }
          break;

        // nil constant
        case 'nil':
          // When we reached string of length 3 chars
          if (strlen($string) == 3)
          {
            // We check if string equal to "nil" and 4th character is end char
            if ($string == "nil" && $this->isEnd($char))
            {
              // Return nil constant
              return new Token("nil",$string);
            }
            // Not equal or not end char lexical error
            return $this->lexicalError();
          }
          // If we somehow reached more than 3 characters
          else if (strlen($string) > 3)
          {
            // Return lexical error instantly
            return $this->lexicalError();
          }
          // If we have lesser than 3 chars and
          else
          {
            // We reached end char
            if ($this->isEnd($char))
            {
              // Return lexical error
              return $this->lexicalError();
            }
          }
          $string = $string.$char;
          break;

        case 'string':
          // Did we reached end character ?
          if ($this->isEnd($char))
          {
            // Before we return string we need to escape special chars
            $string = htmlspecialchars($string, ENT_XML1 | ENT_QUOTES, 'UTF-8');
            return new Token("string",$string);
          }
          // White character from 0 to 32
          if (ord($char) >= 0 && ord($char) <= ord(" "))
          {
            // They cant be here
            return $this->lexicalError();
          }
          // If its \
          if (ord($char) == ord("\\"))
          {
            // Change state
            $special_char = "";
            $state = 'string_1';
            continue 2;
          }
          $string = $string.$char;
          break;

        case 'string_1':
          // After \ we are expecting 3 numbers
          if (!ctype_digit($char))
          {
            // Not digit error
            return $this->lexicalError();
          }
          $special_char = $special_char.$char;
          $state = 'string_2';
          break;

        case 'string_2':
          // After \ we are expecting 3 numbers
          if (!ctype_digit($char))
          {
            // Not digit error
            return $this->lexicalError();
          }
          $special_char = $special_char.$char;
          $state = 'string_3';
          break;

        case 'string_3':
          // After \ we are expecting 3 numbers
          if (!ctype_digit($char))
          {
            // Not digit error
            return $this->lexicalError();
          }
          $special_char = $special_char.$char;

          // If its out of range
          if ($special_char < 0 || $special_char > 999)
          {
            // Error
            return $this->lexicalError();
          }
          // Switch back to the string state
          $state = 'string';
          $string = $string."\\".$special_char;
          continue 2;
          break;

        // We need to have atleast 1 char in integer
        case 'integer':
          // First char may be +,-
          if (ord($char) == ord('-') || ord($char) == ord('+'))
          {
            $string = $char;
            $state = 'integer_1';
            continue 2;
          }
          // Or digit
          else if (ctype_digit($char))
          {
            $string = $char;
            $state = 'integer_1';
            continue 2;
          }
          // Everything else is error
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
            // If the integer got only + or -
            if (strlen($string) == 1 && ($string == "-" || $string == "+"))
            {
              // Its lexical error
              return $this->lexicalError();
            }
            // Return int
            return new Token("int",$string);
          }
          // Wrong character
          else
          {
            // Return error
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
          // If we somehow managed to append more than 5 characters
          else if (strlen($string) > 5)
          {
            // return lexical error
            return $this->lexicalError();
          }
          else
          {
            // Did we reached end char sooner than "true" or "false " ?
            if ($this->isEnd($char))
            {
              // Return lexical error
              return $this->lexicalError();
            }
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
          // Did we encounter EOF ?
          else if (ord($char) == FALSE)
          {
            // return EOL and set next EOF to be final
            $this->eof_reached = TRUE;
            return new Token("EOL","EOL");
          }
          continue 2;
          break;


        case 'header':
          // Did we found end char ?
          if ($this->isEnd($char))
          {
            // Return header
            return new Token("header",$string);
          }
          // apend string
          $string = $string.$char;
        break;
      }
    }
  }
}
