<?php

include_once 'syntax_analyzator_parser.php';

// Check arguments
if ($argc == 2)
{
  // When user types help
  if ($argv[1] == "--help")
  {
    // Output to the stdin help message
    echo "--Usage\n";
    echo "php ".$argv[0]." < IPP_source > XML_output\n";
    exit(0);
  }
  // Argument was somehow wrongly typed so output help message to stderr
  fwrite(STDERR,"Argument not recognized. For help type \"php ".$argv[0]." --help\"\n");
  exit(10);
}
elseif ($argc > 2)
{
  // Wrong arguments output to stderr help message
  fwrite(STDERR,"Too many arguments. For help type \"php ".$argv[0]." --help\"\n");
  exit(10);
}

// Instantiate class and call function from that class
$parser = new SyntaxAnalyzator();
$result = $parser->Parse();

// Check if parsing was succesfull
if ($result != 0)
{
  exit($result);
}

// No errors ? Okay generate XML
$parser->GenerateXML();

// Exit
exit(0);
