Implementační dokumentace k 1. úloze do IPP 2018/2019
Jméno a příjmení: Matúš Škuta
Login: xskuta04

## Implementation

The project was created in object oriented programming style and everything is called from main file **parse.php**.
If some error occurred it is returned from called function and everything is freed.

## Lexical analysis

Lexical analysis was implemented by state automat, which loads characters one by one from `STDIN`.
Character comparison is done by getting ascii decimal code from char with `ord()` and comparing integers.
Because PHP does not contain function `ungetc()` I needed to find some workaround for this kind of problem.
The solution was simple, just create variable that will store the char we need to return to the stdin and
every time, before we load new char from stdin we check if the variable is not empty and rather load from variable.
The most important state is `type_determination` which determinate which state we need to go to
handle the lexem. Identified and checked lexems are returned in class **Token** which has type and string.
I am not using any regular expressions because I am implementing it in similar fashion as was done in IFJ project.

## Syntax analysis

Syntax is being checked through loop in which is every token received from lexical analyzer
thoroughly checked. If given token is instruction then the next tokens are checked for given type of
token that the instruction need.

## XML Generator

XML file is generated through simple state automat and class **SimpleXMLElement** in which I append
elements `instruction` to the root element `program`and elements `arg` to the created element
`instruction`. Instructions got attribute order which identifies the order of instruction and
oppcode which contain instruction. Arg element contain attribute type which determinate type of
argument and data of arg element are in between elements. When XML file is created its content is printed
to the stdin.
