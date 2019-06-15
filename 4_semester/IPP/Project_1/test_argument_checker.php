<?php

include 'test_file_loader.php';
include 'test_file_executer.php';

class ArgumentChecker
{
  var $argument_array = NULL;
  var $programName = "";


  function __construct($argv,$argc)
  {
    // Create array
    $this->argument_array = array();

    // Load array with arguments
    for ($i=1; $i < $argc; $i++)
    {
      array_push($this->argument_array,$argv[$i]);
    }

    $this->programName = $argv[0];
  }

  private function CallGivenFunctions($parseName,$interpretName,$testDirectory,$recursive,$parseOnly,$interpretOnly)
  {
    // If both are true
    if ($parseOnly && $interpretOnly)
    {
      return 10;
    }

    // If we want parse tests and interpret name was given error
    if ($parseOnly && $interpretName != "")
    {
      return 10;
    }

    // If we want only interpret tests and parser name was given
    if ($interpretOnly && $parseName != "")
    {
      return 10;
    }

    if ($testDirectory == "")
    {
      return 10;
    }

    if ($interpretName == "")
    {
      $interpretName = "interpret.py";
    }

    if ($parseName == "")
    {
      $parseName = "parse.php";
    }

    $fileLoader = new FileLoader($testDirectory,$recursive);

    $test_executer = new TestExecuter($parseName,$interpretName);

    if ($parseOnly)
    {
      if (!file_exists($parseName))
      {
        return 11;
      }

      if (!is_readable($parseName))
      {
        return 11;
      }

      $fileLoader->LoadFiles($testDirectory,$recursive,"parse");
      $loadedFiles = $fileLoader->GetLoadedFiles();

      //echo "Calling parser tests\n";
      $result = $test_executer->TestParser($loadedFiles);
      return 0;
    }
    else if ($interpretOnly)
    {
      if (!file_exists($interpretName))
      {
        return 11;
      }

      if (!is_readable($interpretName))
      {
        return 11;
      }

      $fileLoader->LoadFiles($testDirectory,$recursive,"int");
      $loadedFiles = $fileLoader->GetLoadedFiles();

      //echo "Calling interpret tests\n";
      $result = $test_executer->TestInterpreter($loadedFiles);
      return 0;
    }
    else
    {
      if (!file_exists($parseName))
      {
        return 11;
      }

      if (!is_readable($parseName))
      {
        return 11;
      }

      if (!file_exists($interpretName))
      {
        return 11;
      }

      if (!is_readable($interpretName))
      {
        return 11;
      }
      $fileLoader->LoadFiles($testDirectory,$recursive,"both");
      $loadedFiles = $fileLoader->GetLoadedFiles();

      //echo "Testing both parser and interpreter\n";
      $result = $test_executer->TestBoth($loadedFiles);
      return 0;
    }
  }

  public function PrintHelp($failed)
  {
    if ($failed)
    {
      fwrite(STDERR, "Usage: ".$this->programName." [arguments]\n");
      fwrite(STDERR, "\t--help: To see this help message.\n");
      fwrite(STDERR, "\t--directory=path: Directory containing the tests. If not specified current directory is chosen.\n");
      fwrite(STDERR, "\t--recursive: Look throug every folder in the directory.\n");
      fwrite(STDERR, "\t--parse-script=file: Name of the parser script. If not specified \"parse.php\" is chosen.\n");
      fwrite(STDERR, "\t--int-script=file: Name of the interpret script. If not specified \"interpret.py\" is chosen.\n");
      fwrite(STDERR, "\t--parse-only: Specify to only test parser script\n");
      fwrite(STDERR, "\t--int-only: Specify to only test interpret script\n");
      return;
    }
    echo "Usage: ".$this->programName." [arguments]\n";
    echo "\t--help: To see this help message.\n";
    echo "\t--directory=path: Directory containing the tests. If not specified current directory is chosen.\n";
    echo "\t--recursive: Look throug every folder in the directory.\n";
    echo "\t--parse-script=file: Name of the parser script. If not specified \"parse.php\" is chosen.\n";
    echo "\t--int-script=file: Name of the interpret script. If not specified \"interpret.py\" is chosen.\n";
    echo "\t--parse-only: Specify to only test parser script\n";
    echo "\t--int-only: Specify to only test interpret script\n";

  }

  public function ExecuteArguments($argc)
  {
    $parseName = "";
    $interpretName = "";
    $testDirectory = "./";
    $recursive = FALSE;
    $parseOnly = FALSE;
    $interpretOnly = FALSE;

    // Help ?
    if (in_array("--help",$this->argument_array))
    {

      // Check if there is only --help
      if ($argc != 2)
      {
        // Print stderr help
        $this->PrintHelp(true);

        // Return error in arguments
        return 10;
      }

      // Print stdin help
      $this->PrintHelp(false);

      // Return OK
      return 0;
    }

    if ($argc == 1)
    {
      // Print stderr help
      $this->PrintHelp(true);

      // Return error in arguments
      return 10;
    }

    // Check directory, parseName and interpretName
    for ($i=0; $i < count($this->argument_array); $i++)
    {
      $tmp = NULL;
      // Directory
      if (strncmp("--directory=",$this->argument_array[$i],12) == 0)
      {
        // Split the string
        $tmp = explode("=",$this->argument_array[$i]);

        // Check if there only 2 items
        if (count($tmp) != 2)
        {
          // Print stderr help
          $this->PrintHelp(true);

          // No error
          return 10;
        }
        $testDirectory = $tmp[1];
      }
      // Parse script ?
      else if (strncmp("--parse-script=",$this->argument_array[$i],15) == 0)
      {
        // Split the string
        $tmp = explode("=",$this->argument_array[$i]);

        // Check if there only 2 items
        if (count($tmp) != 2)
        {
          // Print stderr help
          $this->PrintHelp(true);
          // No error
          return 10;
        }

        $parseName = $tmp[1];
      }
      // Interpret script ?
      else if (strncmp("--int-script=",$this->argument_array[$i],13) == 0)
      {
        // Split the string
        $tmp = explode("=",$this->argument_array[$i]);

        // Check if there only 2 items
        if (count($tmp) != 2)
        {
          // Print stderr help
          $this->PrintHelp(true);
          // No error
          return 10;
        }
        $interpretName = $tmp[1];
      }
      // Check if parse-only
      else if (in_array("--parse-only",$this->argument_array) && !$parseOnly)
      {
        $parseOnly = true;
      }
      // Check if integer-only
      else if (in_array("--int-only",$this->argument_array) && !$interpretOnly)
      {
        $interpretOnly = true;
      }
      // Check if recursive
      else if (in_array("--recursive",$this->argument_array) && !$recursive)
      {
        $recursive = true;
      }
      // Some error
      else
      {
        // Print stderr help
        $this->PrintHelp(true);

        return 10;
      }
    }

    return $this->CallGivenFunctions($parseName,$interpretName,$testDirectory,$recursive,$parseOnly,$interpretOnly);
  }
}
