<?php

class FileLoader
{
  var $FileArray;

  function __construct()
  {
    $this->FileArray = array();
  }

  // Insert file into array
  private function InsertFile($fileInfo)
  {
    // Get extension of file
    $ext = $fileInfo->getExtension();

    // Check if its one of accepted extensions
    if ($ext == "out" || $ext == "in" || $ext == "src" || $ext == "rc" || $ext == "xml")
    {
      // Generate path to file
      $fileName = $fileInfo->getPath()."/".$fileInfo->getBasename(".".$fileInfo->getExtension());

      // IS array created ?
      if (!array_key_exists($fileName,$this->FileArray))
      {
        // No then create
        $this->FileArray[$fileName] = array();
      }

      // Save instance of class
      $fileinfo = new SplFileInfo($fileInfo);
      $this->FileArray[$fileName][$fileInfo->getExtension()] = $fileinfo;
    }
    // No ? Then skip
  }

  // Create missing files
  private function CreateFile($pathname,$extension)
  {
    $filename = $pathname.".".$extension;
    // Create file
    if (($file = fopen($filename,"w")) == FALSE)
    {
      // Failed
      return 12;
    }

    // If extension is rc we need to write 0 into it
    if ($extension == "rc")
    {
      // Did write failed ?
      if (fwrite($file,"0") == 0)
      {
        fclose($file);
        return 12;
      }
    }

    // Close writting file
    fclose($file);

    // Append file class into array
    $fileinfo = new SplFileInfo($filename);
    $this->FileArray[$pathname][$extension] = $fileinfo;
  }

  // Check and create missing files
  private function CheckMissingFiles($type)
  {
    // Ket all files
    $keys = array_keys($this->FileArray);
    $result = 0;

    // Loop through each test
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

      // Return all items and folders in directory
      $iterator = new FilesystemIterator($directory, FilesystemIterator::KEY_AS_PATHNAME);

      // LOop through each returned item
      foreach ($iterator as $key => $fileinfo)
      {
        // Check if its directory
        if (is_dir($key))
        {
          // Append it and jump up
          array_push($directories,$key);
          continue;
        }

        // Its file insert it into array
        $this->InsertFile($fileinfo);
      }

      // If recursive is not set end with first
      if (!$recursive)
      {
        break;
      }
    }

    // Before we return all files first we need to check if there are all of them
    return $this->CheckMissingFiles($type);
  }

  // Return all loaded files
  function GetLoadedFiles()
  {
    return $this->FileArray;
  }
}
