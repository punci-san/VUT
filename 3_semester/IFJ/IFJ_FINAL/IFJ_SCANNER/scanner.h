#include "str.h"

#define KEYWORD_COUNT 9
#define EOL '\n'

typedef enum TokenType
{
  TT_EOL,             //    End of line \n
  TT_IDENTIFIKATOR,   //    nazov_variable5
  TT_FUN_IDENTIFIKATOR,//   Identifikator funkcie
  TT_STRING,          //    "Hello world"
  TT_INTEGER,         //    9595
  TT_FLOAT,           //    25.85
  TT_EXPONENT,        //    2.E+5 2e-8
  TT_ASSIGNMENT,      //    =
  TT_PLUS,            //    +
  TT_MINUS,           //    -
  TT_DIVISION,        //    /
  TT_MULTIPLY,        //    *
  TT_LEFT_BRACKET,    //    (
  TT_RIGHT_BRACKET,   //    )
  TT_EQUAL,           //    ==
  TT_HIGHER,          //    >
  TT_LOWER,           //    <
  TT_HIGHER_EQUAL,    //    >=
  TT_LOWER_EQUAL,     //    <=
  TT_FALSE,           //    !
  TT_FALSE_EQUAL,     //    !=
  TT_COMA,            //    ,
  TT_DEF,             //  "def", 20     20 + i
  TT_DO,              //  "do", 21
  TT_ELSE,            //  "else",22
  TT_END,             //  "end",23
  TT_IF,              //  "if",24
  TT_NOT,             //  "not",25
  TT_NIL,             //  "nil",26
  TT_THEN,            //  "then",27
  TT_WHILE            //  "while",28
}TokenType;


int initScanner(char *filename);
int getToken(String *token);
void freeScanner();
