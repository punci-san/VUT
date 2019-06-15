<?php


class TestExecuter
{
  var $parserName;
  var $intName;
  var $filesTested;
  var $tmpFileName;

  function __construct($parserName,$intName)
  {
    $this->parserName = $parserName;
    $this->intName = $intName;
    $this->filesTested = array();
    $this->tmpFileName = "temporaly_test_file";
  }

  // Parse-only test
  public function TestParser($file_array)
  {
    // Create temporaly test file
    $tmpFile = fopen($this->tmpFileName,"w");

    // File creation failed
    if ($tmpFile == FALSE)
    {
      return 12;
    }

    // Loop through each test and execute him
    foreach ($file_array as $key => $value)
    {
      // Get files for test
      $xmlResult = 0;
      $src = $value["src"];
      $out = $value["out"];
      $rc = $value["rc"];

      // Start script
      exec("php7.3 ".$this->parserName. " < ".$src . " > ".$this->tmpFileName,$output,$return);

      // Open rc file
      $rcFile = fopen($rc,"r");

      // Check if file opening failed
      if ($rcFile == FALSE)
      {
        return 11;
      }

      // Read file
      $rcResult = fread($rcFile,filesize($rc));

      // Close file
      fclose($rcFile);

      // When we expect no error
      if ($return == $rcResult && $return == 0)
      {
        // We need to check output
        exec("java -jar /pub/courses/ipp/jexamxml/jexamxml.jar ".$out." ".$this->tmpFileName,$tmp,$xmlResult);
      }

      // Append test to the array
      array_push($this->filesTested,array($key,($return == $rcResult && $xmlResult == 0)));
    }
    // Close tmp file and delete it
    fclose($tmpFile);
    unlink($this->tmpFileName);

    return $this->GenerateHTML("Parse-only");
  }

  // Int-only test
  public function TestInterpreter($file_array)
  {
    // Loop through each test
    foreach ($file_array as $key => $value)
    {
      // Get tested files
      $xmlResult = 0;
      $src = $value["src"];
      $in = $value["in"];
      $out = $value["out"];
      $rc = $value["rc"];

      // Execute script
      exec("python3.6 ".$this->intName. " --source=".$src . " < ".$in . " > ".$this->tmpFileName,$output,$return);

      // Open rc file
      $rcFile = fopen($rc,"r");

      // Test if opening failed
      if ($rcFile == FALSE)
      {
        return 11;
      }

      // Read result
      $rcResult = fread($rcFile,filesize($rc));

      // Close file
      fclose($rcFile);

      // If we expected and script returned 0
      if ($return == $rcResult && $return == 0)
      {
        // WE need to test outputs
        exec("diff ".$out." ".$this->tmpFileName,$tmp,$xmlResult);
      }

      // Append test to the array
      array_push($this->filesTested,array($key,($return == $rcResult && $xmlResult == 0)));
    }
    return $this->GenerateHTML("Interpret-only");
  }

  // Both tests parse and int
  public function TestBoth($file_array)
  {
    // Create temporaly test file
    $tmpFile1 = fopen($this->tmpFileName,"w");

    // File creation failed
    if ($tmpFile1 == FALSE)
    {
      return 12;
    }

    // Create temporaly test file
    $tmpFile2 = fopen($this->tmpFileName."2","w");

    // File creation failed
    if ($tmpFile2 == FALSE)
    {
      fclose($tmpFile1);
      return 12;
    }

    // Loop through every test
    foreach ($file_array as $key => $value)
    {
      // Load files needed for test
      $xmlResult = 0;
      $src = $value["src"];
      $in = $value["in"];
      $out = $value["out"];
      $rc = $value["rc"];
      $xml = NULL;

      // If xml file exist add him
      if (array_key_exists("xml",$value))
      {
        $xml = $value["xml"];
      }

      // execute parse
      exec("php7.3 ".$this->parserName. " < ".$src . " > ".$this->tmpFileName,$output,$return);

      // Load return value from test file
      $rcFile = fopen($rc,"r");

      // Check if opening file failed
      if ($rcFile == FALSE)
      {
        return 11;
      }
      // Read file
      $rcResult = fread($rcFile,filesize($rc));

      // Close fole
      fclose($rcFile);

      // If rcResult is in error code that should end in parser we check and end
      if (20 <= $rcResult && $rcResult <= 23)
      {
        array_push($this->filesTested,array($key,$return == $rcResult));
        continue;
      }

      // XML file exist compare it
      if ($xml != NULL)
      {
        // Compare xml file and file outputted with Jexamxml
        exec("java -jar /pub/courses/ipp/jexamxml/jexamxml.jar ".$xml." ".$this->tmpFileName,$tmp,$xmlResult);
      }

      // If they are not the same
      if ($xmlResult != 0)
      {
        // Add error
        array_push($this->filesTested,array($key,FALSE));
        continue;
      }

      // If they are execute them
      exec("python3.6 ".$this->intName. " --source=".$this->tmpFileName . " < ".$in." > ".$this->tmpFileName."2",$output,$return);

      // When we expect 0 and got 0 from script
      if ($return == $rcResult && $return == 0)
      {
        // Check outputs
        exec("diff ".$out." ".$this->tmpFileName."2",$tmp,$diffResult);
      }

      // Append to the tests array
      array_push($this->filesTested,array($key,($return == $rcResult && $diffResult == 0)));
    }

    // Close temporaly files and delete them
    fclose($tmpFile1);
    fclose($tmpFile2);
    unlink($this->tmpFileName);
    unlink($this->tmpFileName."2");

    return $this->GenerateHTML("Full series of tests");
  }

  // Generate HTML
  private function GenerateHTML($type)
  {
    // Variables
    $tests_output = "";
    $percetage_output = 0;
    $passed_count = 0;

    // Loop through every taken test
    for ($i=0; $i < count($this->filesTested); $i++)
    {
      // If it succeded
      if ($this->filesTested[$i][1])
      {
        // Append success HTML
        $tests_output = $tests_output."<li>
        <span class=\"first\">".$this->filesTested[$i][0]."</span>
        <span class=\"second-success\">OK</span>
        </li>";
        $passed_count++;
      }
      // If not append
      else
      {
        $tests_output = $tests_output."<li>
        <span class=\"first\">".$this->filesTested[$i][0]."</span>
        <span class=\"second-failed\">FAILED</span>
        </li>";
      }
    }

    // Calculate percentage
    if (count($this->filesTested) == 0)
    {
      $percetage_output = 0;
    }
    else
    {
      $percetage_output = $passed_count / count($this->filesTested) * 100;
    }

    // HTML string
    $string_first = "<!DOCTYPE html>
<html>
<head>
    <meta charset=\"utf-8\">
    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">
    <title>".$type."</title>
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
</head>
<style>
    body
    {
        font-family: Arial, Helvetica, sans-serif;
        background-color: #18181b;
    }
    .test-type
    {
        border-bottom: 0.1em solid white;
    }

    .test-type .test-percentage
    {
        font-size:  1em;
        color: white;
        text-align: right;
        float: right;
    }
    .test-type .test-name
    {
        color:white;
        font-size: 1.25em;
        text-align: center;
    }
    .test-statistics
    {
        width: 100%;
        text-align: right;
        padding-bottom: 0.5em;
    }

    .test-statistics span
    {
        font-size:1.25em;
        color: white;
        padding-right: 0.5em;
    }
    ul
    {
        list-style-type: none;
        padding: 0;
        border: 1px solid rgb(17, 167, 226);
    }
    ul li
    {
        padding: 8px 16px;
        border-bottom: 1px solid rgb(17, 167, 226);
    }
    ul li .first
    {
        color:white;
        padding:0;
        text-align: left;
    }
    ul li .second-success
    {
        color:#46ea4e;
        padding:0;
        float:right;
    }
    ul li .second-failed
    {
        color:#e54334;
        padding:0;
        float:right;
    }
    ul li:last-child
    {
        border-bottom: none
    }
</style>
<body>
    <div class=\"test-type\">
      <span class=\"test-name\">".$type."</span>
      <span class=\"test-percentage\">Passed: ".number_format((float)$percetage_output, 2, '.', '')." %</span>
    </div>
    <ul class=\"test-list\">
        ".$tests_output."
    </ul>
    <div class=\"test-statistics\">
    <span>Total: ".count($this->filesTested)."</span>
    <span>Passed: ".$passed_count."</span>
    <span>Failed: ".(count($this->filesTested) - $passed_count)."</span>
</div>
</body>
</html>";

    // print to the stdout HTML
    echo $string_first;
  }
}
