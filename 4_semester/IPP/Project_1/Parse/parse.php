<?php

include_once 'syntax_analyzator_parser.php';

if ($argc == 2)
{
  if ($argv[1] == "--help")
  {
    echo "--Usage\n";
    echo "php ".$argv[0]." < IPP_source > XML_output\n";
    exit(0);
  }
  echo "Argument not recognized. For help type \"php ".$argv[0]." --help\"\n";
  exit(1);
}
elseif ($argc > 2)
{
  echo "Too many arguments. For help type \"php ".$argv[0]." --help\"\n";
  exit(1);
}

$parser = new SyntaxAnalyzator();
$result = $parser->Parse();

if ($result != 0)
{
  //echo "Parse error: ".$result."\n";
  exit($result);
}

$result = $parser->GenerateXML();

if ($result != 0)
{
  exit($result);
}

exit(0);
