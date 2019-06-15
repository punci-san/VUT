<?php

include 'test_argument_checker.php';

// Create class argument checker
$argumentChecker = new ArgumentChecker($argv,$argc);

// Execute arguments and call functions associated with given arguments
$result = $argumentChecker->ExecuteArguments($argc);

// End program
exit($result);
