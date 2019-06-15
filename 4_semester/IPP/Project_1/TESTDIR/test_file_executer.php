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

  public function TestParser($file_array)
  {

    // Create temporaly test file
    $tmpFile = fopen($this->tmpFileName,"w");

    // File creation failed
    if ($tmpFile == FALSE)
    {
      return 12;
    }

    foreach ($file_array as $key => $value)
    {
      $xmlResult = 0;
      $src = $value["src"];
      $out = $value["out"];
      $rc = $value["rc"];

      //echo "php ".$this->parserName. " < ".$src . " > ".$this->tmpFileName."\n";

      exec("php ".$this->parserName. " < ".$src . " > ".$this->tmpFileName,$output,$return);

      $rcFile = fopen($rc,"r");

      if ($rcFile == FALSE)
      {
        return 11;
      }

      $rcResult = fread($rcFile,filesize($rc));

      fclose($rcFile);

      if ($return == $rcResult && $return == 0)
      {
        exec("java -jar /pub/courses/ipp/jexamxml/jexamxml.jar ".$out." ".$this->tmpFileName,$tmp,$xmlResult);
      }

      //echo $key." : ".$return."<>".$rcResult." ".$xmlResult."\n";

      array_push($this->filesTested,array($key,($return == $rcResult && $xmlResult == 0)));
    }
    fclose($tmpFile);

    //return 0;
    return $this->GenerateHTML("Parse-only");
  }

  public function TestInterpreter($file_array)
  {
    return $this->GenerateHTML("Interpret-only");
  }

  public function TestBoth($file_array)
  {
    return $this->GenerateHTML("Full series of tests");
  }

  private function GenerateHTML($type)
  {
    $tests_output = "";
    $percetage_output = 0;
    $passed_count = 0;

    for ($i=0; $i < count($this->filesTested); $i++)
    {
      if ($this->filesTested[$i][1])
      {
        $tests_output = $tests_output."<li>
        <span class=\"first\">".$this->filesTested[$i][0]."</span>
        <span class=\"second-success\">OK</span>
        </li>";
        $passed_count++;
      }
      else
      {
        $tests_output = $tests_output."<li>
        <span class=\"first\">".$this->filesTested[$i][0]."</span>
        <span class=\"second-failed\">FAILED</span>
        </li>";
      }
    }

    $percetage_output = $passed_count / count($this->filesTested) * 100;

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
      <span class=\"test-name\">Parse only tests</span>
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

    echo $string_first;
  }
}
