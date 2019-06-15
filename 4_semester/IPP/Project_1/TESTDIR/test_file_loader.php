<?php

class FileLoader
{
  var $FileArray;

  function __construct()
  {
    $this->FileArray = array();
  }

  private function InsertFile($fileInfo)
  {
    $ext = $fileInfo->getExtension();

    if ($ext == "out" || $ext == "in" || $ext == "src" || $ext == "rc" || $ext == "xml")
    {
      $fileName = $fileInfo->getPath()."/".$fileInfo->getBasename(".".$fileInfo->getExtension());

      if (!array_key_exists($fileName,$this->FileArray))
      {
        $this->FileArray[$fileName] = array();
      }

      $fileinfo = new SplFileInfo($fileInfo);

      $this->FileArray[$fileName][$fileInfo->getExtension()] = $fileinfo;
    }
  }

  private function CreateFile($pathname,$extension)
  {
    $filename = $pathname.".".$extension;
    // Create file
    if (($file = fopen($filename,"w")) == FALSE)
    {
      // Failed
      return 11;
    }

    if ($extension == "rc")
    {
      fwrite($file,"0");
    }

    fclose($file);

    $fileinfo = new SplFileInfo($filename);

    $this->FileArray[$pathname][$extension] = $fileinfo;
  }

  private function CheckMissingFiles($type)
  {
    $keys = array_keys($this->FileArray);
    $result = 0;

    foreach ($keys as $key)
    {
      // Check if rc exists
      if (!array_key_exists("rc",$this->FileArray[$key]))
      {
        $result = $this->CreateFile($key,"rc");
      }
      // Check if .in exists
      if (!array_key_exists("in",$this->FileArray[$key]) && $type != "parse")
      {
        $result = $this->CreateFile($key,"in");
      }
      // Check if .out exists
      if (!array_key_exists("out",$this->FileArray[$key]))
      {
        $result = $this->CreateFile($key,"out");
      }

      // End loop with error
      if ($result != 0)
      {
        return $result;
      }
    }
    return $result;
  }

  public function LoadFiles($Testsdirectory,$recursive,$type)
  {
    if (!is_dir($Testsdirectory))
    {
      // Non existent path
      return 11;
    }

    $directories = array();

    // Push directory which we need to look in
    array_push($directories,$Testsdirectory);

    while ((count($directories) != 0))
    {
      $directory = array_pop($directories);

      // Check if we have readable right in that directory
      if (!is_readable($directory))
      {
        return 11;
      }

      $iterator = new FilesystemIterator($directory, FilesystemIterator::KEY_AS_PATHNAME);

      foreach ($iterator as $key => $fileinfo)
      {
        if (is_dir($key))
        {
          array_push($directories,$key);
          continue;
        }

        // Its file
        $this->InsertFile($fileinfo);
      }

      // If its not recursive break
      if (!$recursive)
      {
        break;
      }
    }

    // Before we return all files first we need to check if there are all of them
    return $this->CheckMissingFiles($type);
  }

  function GetLoadedFiles()
  {
    return $this->FileArray;
  }
}
