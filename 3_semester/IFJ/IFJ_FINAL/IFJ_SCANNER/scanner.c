#include "scanner.h"
#include "str.h"
#include "errCode.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

FILE *file;

static char *keywords[KEYWORD_COUNT] =
{
  "def",
  "do",
  "else",
  "end",
  "if",
  "not",
  "nil",
  "then",
  "while"
};


int initScanner(char *filename)
{
  if ((file = fopen(filename,"r")) == NULL)
  {
    fprintf(stderr, "Problém pri otváraní súboru.\n");
    return FILE_ERROR;
  }
  return 0;
}

void freeScanner()
{
  fclose(file);
}

/*
  Ak token ktory sme obdrzali je keyword vratime poradove cislo keywordu v
  arrai, inac vratime TT_IDENTIFIKATOR
*/
static int isKeyword(String *token)
{
  for (int i = 0; i < KEYWORD_COUNT; i++)
  {
    // Ak je to niektory z keywordov vratime jeho ID
    if (strcmp(keywords[i],token->str) == 0)
    {
      return (22 + i); // 20 je pocet identifikatorov TokenType pred TokenType typu keywords
    }
  }
  return TT_IDENTIFIKATOR;
}

static int isSymbol(char ch,String *token)
{
  clearString(token);
  // Zobereme nasledujuci char
  char next_char = fgetc(file);

  // Pridame prvy char do stringu
  appendChar(ch,token);
  switch (ch)
  {
    case '=':
      // Ak prvy charakter je = moze za nim nasledovat =
      if (next_char == '=')
      {
        appendChar(next_char,token);
        return TT_EQUAL;
      }
      // Ak nie vratime naspat charakter do bufferu
      fseek(file,-1,SEEK_CUR);
      return TT_ASSIGNMENT;
    break;

    case '>':
      // Za > moze nasledovat =
      if (next_char == '=')
      {
        appendChar(next_char,token);
        return TT_HIGHER_EQUAL;
      }
      fseek(file,-1,SEEK_CUR);
      return TT_HIGHER;
    break;

    case '<':
      // Za < moze znova nasledovat =
      if (next_char == '=')
      {
        appendChar(next_char,token);
        return TT_LOWER_EQUAL;
      }
      fseek(file,-1,SEEK_CUR);
      return TT_LOWER;
    break;

    case '!':
      // Za ! moze nasledovat =
      if (next_char == '=')
      {
        appendChar(next_char,token);
        return TT_FALSE_EQUAL;
      }
      fseek(file,-1,SEEK_CUR);
      return TT_FALSE;
    break;

    case '(':
      fseek(file,-1,SEEK_CUR);
      return TT_LEFT_BRACKET;
    break;

    case ')':
      fseek(file,-1,SEEK_CUR);
      return TT_RIGHT_BRACKET;
    break;

    case ',':
      fseek(file,-1,SEEK_CUR);
      return TT_COMA;
    break;

    case '+':
      fseek(file,-1,SEEK_CUR);
      return TT_PLUS;
    break;

    case '-':
      fseek(file,-1,SEEK_CUR);
      return TT_MINUS;
    break;

    case '*':
      fseek(file,-1,SEEK_CUR);
      return TT_MULTIPLY;
    break;

    case '/':
      fseek(file,-1,SEEK_CUR);
      return TT_DIVISION;
    break;

    // AK sa vyskytol iny symbol tak to nepatri do nasho jazyka
    default:
      return -LEXICAL_ERR;
    break;
  }
  return -LEXICAL_ERR;
}
/*
  Vygeneruje z \ a dalsieho charakteru 1 escape_charakter
*/
static char generateEscapeChar(char ch)
{
  switch (ch)
  {
    // Ak je \n
    case 'n':
      return '\n';
    break;

    // Ak je to \t
    case 't':
      return '\t';
    break;

    // Ak je to \"
    case '"':
      return '\"';
    break;

    // Ak je to \s medzera
    case 's':
      return ' ';
    break;

    case 'x':
      return 'x';
    break;

  }
}

static int isHex(char ch)
{
  // Ak je to v intervale <0,9>
  if (ch >= 48 || ch <= 57)
  {
    // Vraciame cislo od 0 po 9
    return (ch - 48);
  }
  // Alebo ak je v intervale <A,F>
  else if (ch >= 65 || ch <= 70)
  {
    // Vraciame hodnotu od 10 po 15
    return (ch - 55);
  }
  // Ak je to v hocicom inom nie je to Hexadecimalne cislo
  else
  {
    return -1;
  }
}

static int hexToChar()
{
  int hex = 0;
  char ch = fgetc(file);
  // Ak je mensie nez jedna nie je to hex a vraciame chybu
  if ((hex = isHex(ch)) < 0)
  {
    return -LEXICAL_ERR;
  }

  ch = fgetc(file);

  int tmp = 0;
  if ((tmp = isHex(ch)) < 0)
  {
    return hex;
  }

  // Obsahuje aj druhu hodnotu cize staru hodnotu vynasobime 16 aby sme posunuly o 1 desatinne miesto
  hex *= 16;

  hex += tmp;

  // Inac vraciame hexicalnu hodnotu
  return hex;
}

int getToken(String *token)
{
  typedef enum StateTypes
  {
    INIT,
    STRING,
    INTEGER,
    FLOAT,
    EXPONENT,
    COMMENT_LINE,
    COMMENT_BLOCK_START,
    COMMENT_BLOCK_END,
    IDENTIFIER
  }StateTypes;

  StateTypes state = INIT;
  char ch;
  String *tmp;
  int hexalNumbers = 0;

  clearString(token);

  while(1)
  {
    ch = fgetc(file);
    switch (state)
    {

      default:
      case INIT:
        // Ak je koniec suboru
        if (ch < 0)
        {
          return EOF;
        }

        // Ak mame end of line
        if (ch == EOL)
        {
          return TT_EOL;
        }

        // Medzery ignorujeme
        if (ch == ' ')
        {
          continue;
        }

        // Je to cislo cize to moze byt but INTEGER alebo FLOAT
        if (isdigit(ch))
        {
          appendChar(ch,token);
          state = INTEGER;
          continue;
        }

        // Ak to zacina " znaci to zaciatok STRINGU
        if (ch == '"')
        {
          state = STRING;
          continue;
        }

        // Ak to zacina _ alebo malym charakterom je to identifikator
        if (ch == '_' || islower(ch))
        {
          appendChar(ch,token);
          state = IDENTIFIER;
          continue;
        }

        /*
          Urobime kontrolu ci je ked mame = tak za tym nasleduje begin
          Hladame =begin
        */
        if (ch == '=')
        {
          char next_char = fgetc(file);

          // Ak za = nasleduje b je pravdepodobne ze to moze byt =begin
          if (next_char == 'b')
          {
            appendChar(ch,token);
            appendChar(next_char,token);
            state = COMMENT_BLOCK_START;
            continue;
          }
          // Ak nie tak vratime spat ukazovatel o 2
          fseek(file,-1,SEEK_CUR);
        }

        // Ak to zacina # je to zaciatok riadkoveho komentara
        if (ch == '#')
        {
          state = COMMENT_LINE;
          continue;
        }

        // Posledna kontrola ci znak zadany je symbolom
        return isSymbol(ch,token);
      break;

      case STRING:
        if (ch == '"')
        {
          return TT_STRING;
        }
        else if (ch == EOL)
        {
          return -LEXICAL_ERR;
        }
        else if (ch == 92) // Lomeny charakter
        {
          char next_char = fgetc(file);
          // Ak nam vratilo naspat lomeny charakter jedna sa o chybu
          if((ch = generateEscapeChar(next_char)) == 92)
          {
            return -LEXICAL_ERR;
          }
          if (ch == 'x')
          {
            char hexal;
            if ((hexal = hexToChar()) == -LEXICAL_ERR)
            {
              return -LEXICAL_ERR;
            }
            appendChar(hexal,token);
            continue;
          }

          // Pridame charakter
          appendChar(ch,token);

          // Ak je to hexadecimalne cislo
          if (ch == 'x')
          {
            // Tak dalsie 2 nasledujuce musia byt
            hexalNumbers++;
            initString(&tmp);
            appendChar(ch,tmp);
            continue;
          }
        }
        appendChar(ch,token);
      break;

      case INTEGER:
        // Kontrola ci dalsi charakter je cislo
        if (isdigit(ch))
        {
          appendChar(ch,token);
          continue;
        }
        /*
          Skontrolujeme ci zadany charakter nie je nahodou bodka
          co znaci float cize prepneme stav
        */
        if (ch == '.')
        {
          state = FLOAT;
          appendChar(ch,token);
          continue;
        }
        /*
          Ak je medzera alebo novy riadok, tak to znaci koniec ciselnej hodnoty,
          cize zmenime string na integer a vratime token
        */
        if (ch == ' ')
        {
          return TT_INTEGER;
        }

        // Ak je to novy riadok
        if (ch == EOL)
        {
          // Posunieme sa o jeden charakter
          fseek(file,-1,SEEK_CUR);
          return TT_INTEGER;
        }

        initString(&tmp);
        if (isSymbol(ch,tmp) != -LEXICAL_ERR)
        {
          fseek(file,-1,SEEK_CUR);
          freeString(tmp);
          return TT_FLOAT;
        }
        else
        {
          fseek(file,-1,SEEK_CUR);
          freeString(tmp);
          return -LEXICAL_ERR;
        }
      break;

      case FLOAT:
        // Kontrola ci dalsi charakter po "." je cislo
        if (isdigit(ch))
        {
          appendChar(ch,token);
          continue;
        }
        if (ch == 'e' || ch == 'E')
        {
          appendChar(ch,token);
          state = EXPONENT;
          continue;
        }
        /*
          Ak je medzera alebo novy riadok, tak to znaci koniec ciselnej hodnoty,
          cize vratime FLOAT identifikator
        */
        if (ch == ' ')
        {
          return TT_FLOAT;
        }

        if (ch == EOL)
        {
          fseek(file,-1,SEEK_CUR);
          return TT_FLOAT;
        }

        initString(&tmp);
        if (isSymbol(ch,tmp) != -LEXICAL_ERR)
        {
          fseek(file,-1,SEEK_CUR);
          freeString(tmp);
          return TT_FLOAT;
        }
        else
        {
          fseek(file,-1,SEEK_CUR);
          freeString(tmp);
          return -LEXICAL_ERR;
        }
      break;

      case EXPONENT:
        /*
          Exponent je definovany ako
          E[+/-]%d
        */
        if (ch == '+' || ch == '-' || isdigit(ch))
        {
          if (charOccurances(token,ch) > 1)
          {
            return -LEXICAL_ERR;
          }
          appendChar(ch,token);
          continue;
        }
        if (ch == ' ')
        {
          return TT_FLOAT;
        }

        if (ch == EOL)
        {
          fseek(file,-1,SEEK_CUR);
          return TT_FLOAT;
        }

        initString(&tmp);
        if (isSymbol(ch,tmp) != LEXICAL_ERR)
        {
          fseek(file,-1,SEEK_CUR);
          freeString(tmp);
          return TT_EXPONENT;
        }
        else
        {
          fseek(file,-1,SEEK_CUR);
          freeString(tmp);
          return -LEXICAL_ERR;
        }
      break;

      case COMMENT_LINE:
        // Cakame na koniec riadku vtedy koment konci
        if (ch == EOL)
        {
          state = INIT;
          continue;
        }
      break;

      case COMMENT_BLOCK_START:
        /*
          |=begin | = 7
          =b
          ^ Zatial mame toto co su 2 znaky a teraz musime dostat
          ostatne znaky =begin za ktorym nasleduje bude medzera alebo tabulator
        */
        // Je dlzka stringu 7 ?
        if (stringLength(token) == 7)
        {
          if (strcmp("=begin ",token->str) == 0 || strcmp("=begin\t",token->str) == 0 || strcmp("=begin\n",token->str) == 0)
          {
            // Musime posunut ukazovatel bufferu o 1 dolava lebo terajsie CH nebolo spracovane
            fseek(file,-1,SEEK_CUR);
            state = COMMENT_BLOCK_END;
            clearString(token);
            continue;
          }
          else
          {
            fseek(file,-8,SEEK_CUR);
            ch = fgetc(file);
            clearString(token);
            return isSymbol(ch,token);
          }
        }
        else
        {
          appendChar(ch,token);
        }
      break;

      case COMMENT_BLOCK_END:
        /*
          Hladame =end pred ktorym je bud medzera alebo tabulator


        */
        // Natrafily sme skorej na EOF nez na =end
        if (ch < 0)
        {
          return -LEXICAL_ERR;
        }
        if (ch == '=')
        {
          char next_char = fgetc(file);
          if (next_char == 'e')
          {
            appendChar(ch,token);
            appendChar(next_char,token);
            continue;
          }
          fseek(file,-1,SEEK_CUR);
        }
        // |=end| = 4
        if (stringLength(token) == 4)
        {
          // AK sa token zhoduje z =end
          if (strcmp(token->str,"=end") == 0)
          {
            // "=end_|" potrebujeme sa posunut o 6
            fseek(file,-6,SEEK_CUR);
            char prev_char = fgetc(file);

            /*
              Pred =end sa moze nachadzat medzera tabulator alebo novy riadok
            */
            if (prev_char == ' ' || prev_char == '\t' || prev_char == '\n')
            {
              clearString(token);
              state = INIT;
              fseek(file,5,SEEK_CUR);
              continue;
            }
            // Ak sa tam nachadza nieco ine je to LEXICAL_ERR
            else
            {
              return -LEXICAL_ERR;
            }
          }
          /*
            Ak sa string nerovna =end to znamena ze este sme nenasli koniec
            block komentaru preto vynulujeme string a hladame dalej
          */
          else
          {
            clearString(token);
            continue;
          }
        }
        else
        {
          // Kontrolujeme ci sme nasli zaciatocne =e ak hej tak az vtedy zaciname appendovat
          if (stringLength(token) > 0)
          {
            appendChar(ch,token);
          }
        }


      break;

      case IDENTIFIER:
        /*
          IDENTIFIER sa moze skladat z pismen, cislic a _ a pri funkciach moze byt
          ukoncene ? alebo !
            state = INIT;
          ak nasleduje nieco ine vratime LEXICAL_ERR
        */
        if (isalnum(ch) || ch == '_')
        {
          appendChar(ch,token);
          continue;
        }
        // medzera alebo novy riadok znaci ukoncenie identifikatoru
        if (ch == ' ')
        {
          return isKeyword(token);
        }

        if (ch == EOL)
        {
          fseek(file,-1,SEEK_CUR);
          return isKeyword(token);
        }
        // Ak ako posledny znak je ? alebo ! tak to ukoncuje identifikator
        if (ch == '?' || ch == '!')
        {
          appendChar(ch,token);
          return TT_FUN_IDENTIFIKATOR;
        }

        initString(&tmp);
        // Ak
        if (isSymbol(ch,tmp) != -LEXICAL_ERR)
        {
          freeString(tmp);
          fseek(file,-1,SEEK_CUR);
          return TT_IDENTIFIKATOR;
        }
        else
        {
          freeString(tmp);
          return -LEXICAL_ERR;
        }
      break;
    }
  }
}
