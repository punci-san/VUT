/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Adam Richter <xricht27@stud.fit.vutbr.cz>
 *
 **/

#include "scanner.h"
#include "str.h"
#include "err_code.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

int line_counter = 0;

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
      return (24 + i); // 20 je pocet identifikatorov TokenType pred TokenType typu keywords
    }
  }
  return TT_IDENTIFIKATOR;
}

int isSymbol(char ch,String *token,int type)
{
  clearString(token);
  // Zobereme nasledujuci char
  char next_char = getchar();

  // Pridame prvy char do stringu
  appendChar(ch,token);
  switch (ch)
  {
    case '=':
      // Ak prvy charakter je = moze za nim nasledovat =
      if (next_char == '=')
      {
        // AK to neni INIT
        if (type != 0)
        {
          // Vratime
          ungetc(next_char,stdin);
        }
        appendChar(next_char,token);
        return TT_EQUAL;
      }
      // Ak nie vratime naspat charakter do bufferu
      ungetc(next_char,stdin);
      return TT_ASSIGNMENT;
    break;

    case '>':
      // Za > moze nasledovat =
      if (next_char == '=')
      {
        // AK to neni INIT
        if (type != 0)
        {
          // Vratime
          ungetc(next_char,stdin);
        }
        appendChar(next_char,token);
        return TT_HIGHER_EQUAL;
      }
      ungetc(next_char,stdin);
      return TT_HIGHER;
    break;

    case '<':
      // Za < moze znova nasledovat =
      if (next_char == '=')
      {
        // AK to neni INIT
        if (type != 0)
        {
          // Vratime
          ungetc(next_char,stdin);
        }
        appendChar(next_char,token);
        return TT_LOWER_EQUAL;
      }
      ungetc(next_char,stdin);
      return TT_LOWER;
    break;

    case '!':
      // Za ! moze nasledovat =
      if (next_char == '=')
      {
        // AK to neni INIT
        if (type != 0)
        {
          // Vratime
          ungetc(next_char,stdin);
        }
        appendChar(next_char,token);
        return TT_FALSE_EQUAL;
      }
      ungetc(next_char,stdin);
      return TT_FALSE;
    break;

    case '(':
      ungetc(next_char,stdin);
      return TT_LEFT_BRACKET;
    break;

    case ')':
      ungetc(next_char,stdin);
      return TT_RIGHT_BRACKET;
    break;

    case ',':
      ungetc(next_char,stdin);
      return TT_COMA;
    break;

    case '+':
      ungetc(next_char,stdin);
      return TT_PLUS;
    break;

    case '-':
      ungetc(next_char,stdin);
      return TT_MINUS;
    break;

    case '*':
      ungetc(next_char,stdin);
      return TT_MULTIPLY;
    break;

    case '/':
      ungetc(next_char,stdin);
      return TT_DIVISION;
    break;

    // AK sa vyskytol iny symbol tak to nepatri do nasho jazyka
    default:
      ungetc(next_char,stdin);
      return -LEXICAL_ERR;
    break;
  }
  return -LEXICAL_ERR;
}
/*
  Vygeneruje z \ a dalsieho charakteru 1 escape_charakter
*/
char generateEscapeChar(char ch)
{
  // Posledny je ch
  switch (ch)
  {
    // Ak je \n
    case 'n':
      return '\012';
    break;

    // Ak je to \t
    case 't':
      return '\t';
    break;

    // Ak je to \"
    case '"':
      return '\042';
    break;

    // Ak je to \s medzera
    case 's':
      return ' ';
    break;

    // Ak je to \ tak jedna sa o lomitko
    case '\\':
      return '\134';
    break;

    case 'x':
      return 'x';
    break;

    default:
      return 0;
    break;

  }
}

static int isHex(char ch)
{
  // Ak je to v intervale <0,9>
  if (ch >= 48 && ch <= 57)
  {
    // Vraciame cislo od 0 po 9
    return (ch - 48);
  }
  // Alebo ak je v intervale <A,F>
  else if (ch >= 'A' && ch <= 'F')
  {
    // Vraciame hodnotu od 10 po 15
    return (ch - 55);
  }
  // Alebo ak je v intervale <a,f>
  else if (ch >= 'a' && ch <= 'f')
  {
    // Vratime hodnotu od 10 po 15
    return (ch - 87);
  }
  // Ak je to v hocicom inom nie je to Hexadecimalne cislo
  else
  {
    return -1;
  }
}

int hexToChar()
{
  int hex = 0;
  char ch = getchar();
  // Posledny je ch
  // Ak je mensie nez 0 nie je to hex a vraciame chybu
  if ((hex = isHex(ch)) < 0)
  {
    return -LEXICAL_ERR;
  }

  ch = getchar();
  int tmp = 0;
  if ((tmp = isHex(ch)) < 0)
  {
    // Ak sa jedna iba o jeden charakter vratime jeden a posunieme sa o jedno dolava
    ungetc(ch,stdin);
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
    COMMENT_BLOCK_END_ENTER,
    IDENTIFIER
  }StateTypes;

  StateTypes state = INIT;
  char ch;
  String *tmp;

  int hexalNumbers = 0;

  clearString(token);

  while(1)
  {
    ch = getchar();

    // Posledny je ch
    switch (state)
    {
      default:
      case INIT:
        // Ak je koniec suboru
        if (ch == -1)
        {
          return TT_EOF;
        }

        // Ak mame end of line
        if (ch == EOL)
        {
          line_counter = 0;
          return TT_EOL;
        }

        // Medzery ignorujeme a tabulatory
        if (ch == ' ' || ch == '\t')
        {
          line_counter++;
          continue;
        }

        // Je to cislo cize to moze byt but INTEGER alebo FLOAT
        if (isdigit(ch))
        {
          line_counter++;
          appendChar(ch,token);
          state = INTEGER;
          continue;
        }

        // Ak to zacina " znaci to zaciatok STRINGU
        if (ch == '"')
        {
          line_counter++;
          state = STRING;
          continue;
        }

        // Ak to zacina _ alebo malym charakterom je to identifikator
        if (ch == '_' || islower(ch))
        {
          line_counter++;
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
          // Ak sme na zaciatku moze sa jednat o =begin skontrolujeme
          if (line_counter == 0)
          {
            // Zobereme dalsi charakter
            char next_char = getchar();

            // Ak za = nasleduje b jedna sa o =begin
            if (next_char == 'b')
            {
              state = COMMENT_BLOCK_START;
              continue;
            }
            // Inac ak nie je to chyba
            return -LEXICAL_ERR;
          }
          line_counter++;
        }

        // Ak to zacina # je to zaciatok riadkoveho komentara
        if (ch == '#')
        {
          line_counter++;
          state = COMMENT_LINE;
          continue;
        }

        // Posledna kontrola ci znak zadany je symbolom
        return isSymbol(ch,token,state);
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
          // mame \ cize zobereme dalsi char
          char next_char = getchar();
          // Ak nam vratilo naspat lomeny charakter jedna sa o chybu
          if((ch = generateEscapeChar(next_char)) == 0)
          {
            // Appendneme iba next_char a / odignorujeme
            appendChar(next_char,token);
            continue;
          }
          if (ch == 'x')
          {
            int hexal = 0;
            if ((hexal = hexToChar()) == -LEXICAL_ERR)
            {
              return -LEXICAL_ERR;
            }
            appendChar(hexal,token);
            continue;
          }

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

        // Moze to byt exponent
        if (ch == 'e' || ch == 'E')
        {
          appendChar(ch,token);
          state = EXPONENT;
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

        // Ak to zacina # je to zaciatok riadkoveho komentara
        if (ch == '#')
        {
          ungetc(ch,stdin);
          return TT_INTEGER;
        }

        // _ , <a,z>
        if (ch == '_' || islower(ch))
        {
          ungetc(ch,stdin);
          return TT_INTEGER;
        }

        // Ak je to novy riadok
        if (ch == EOL)
        {
          // Posunieme sa o jeden charakter
          ungetc(ch,stdin);
          return TT_INTEGER;
        }

        initString(&tmp);
        if (isSymbol(ch,tmp,state) != -LEXICAL_ERR)
        {
          ungetc(ch,stdin);
          freeString(tmp);
          return TT_INTEGER;
        }
        else
        {
          freeString(tmp);
          return -LEXICAL_ERR;
        }
      break;

      case FLOAT:
        // Pocet . musi byt 1
        if (charOccurances(token,'.') != 1)
        {
          return -LEXICAL_ERR;
        }

        // Kontrola ci dalsi charakter po "." je cislo
        if (isdigit(ch))
        {
          appendChar(ch,token);
          continue;
        }
        if (ch == 'e' || ch == 'E')
        {
          // nesmie byt .e
          if ((stringLength(token) - 1) == containString(token,"."))
          {
            return -LEXICAL_ERR;
          }
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
          // Ak sa rovnaju LEXICAL
          if ((stringLength(token) - 1) == containString(token,"."))
          {
            return -LEXICAL_ERR;
          }
          return TT_FLOAT;
        }

        // Ak to zacina # je to zaciatok riadkoveho komentara
        if (ch == '#')
        {
          // Ak sa rovnaju LEXICAL
          if ((stringLength(token) - 1) == containString(token,"."))
          {
            return -LEXICAL_ERR;
          }
          ungetc(ch,stdin);
          return TT_FLOAT;
        }

        // _ , <a,z>
        if (ch == '_' || islower(ch))
        {
          ungetc(ch,stdin);
          // Ak sa rovnaju LEXICAL
          if ((stringLength(token) - 1) == containString(token,"."))
          {
            return -LEXICAL_ERR;
          }
          return TT_FLOAT;
        }

        if (ch == EOL)
        {
          // Ak sa rovnaju LEXICAL
          if ((stringLength(token) - 1) == containString(token,"."))
          {
            return -LEXICAL_ERR;
          }
          ungetc(ch,stdin);
          return TT_FLOAT;
        }

        initString(&tmp);
        if (isSymbol(ch,tmp,state) != -LEXICAL_ERR)
        {
          ungetc(ch,stdin);
          freeString(tmp);
          // Ak sa rovnaju LEXICAL
          if ((stringLength(token) - 1) == containString(token,"."))
          {
            return -LEXICAL_ERR;
          }
          return TT_FLOAT;
        }
        else
        {
          freeString(tmp);
          return -LEXICAL_ERR;
        }
      break;

      case EXPONENT:

        // AK je posledne e alebo E
        if ((stringLength(token) - 1) == containString(token,"e") || (stringLength(token) - 1) == containString(token,"E"))
        {
          // Moze za nim nasledovat + alebo -
          if (ch == '+')
          {
            // Ak sme nasli + tak nesmie byt -
            if ((charOccurances(token,'+') >= 1) || (charOccurances(token,'-') != 0))
            {
              return -LEXICAL_ERR;
            }
            appendChar(ch,token);
            continue;
          }

          // Ak nasleduje -
          if (ch == '-')
          {
            // Ak sme nasli - tak nesmie byt +
            if ((charOccurances(token,'-') >= 1) || (charOccurances(token,'+') != 0))
            {
              return -LEXICAL_ERR;
            }
            appendChar(ch,token);
            continue;
          }
        }

        if (isdigit(ch))
        {
          appendChar(ch,token);
          continue;
        }
        if (ch == ' ')
        {
          // e nesmie byt posledne
          if ((stringLength(token) - 1) == containString(token,"E") || (stringLength(token) - 1) == containString(token,"e"))
          {
            return -LEXICAL_ERR;
          }
          // AK to konci + alebo -
          if ((stringLength(token) - 1) == containString(token,"+") || (stringLength(token) - 1) == containString(token,"-"))
          {
            return -LEXICAL_ERR;
          }
          return TT_EXPONENT;
        }

        // Ak sa jedna o charakter
        if (ch == '_' || islower(ch))
        {
          // e nesmie byt posledne
          if ((stringLength(token) - 1) == containString(token,"E") || (stringLength(token) - 1) == containString(token,"e"))
          {
            return -LEXICAL_ERR;
          }
          // AK to konci + alebo -
          if ((stringLength(token) - 1) == containString(token,"+") || (stringLength(token) - 1) == containString(token,"-"))
          {
            return -LEXICAL_ERR;
          }
          // Vratime charakter
          ungetc(ch,stdin);
          return TT_EXPONENT;
        }

        // Ak to zacina # je to zaciatok riadkoveho komentara
        if (ch == '#')
        {
          // Ak sa rovnaju LEXICAL
          if ((stringLength(token) - 1) == containString(token,"E") || (stringLength(token) - 1) == containString(token,"e"))
          {
            return -LEXICAL_ERR;
          }
          // AK to konci + alebo -
          if ((stringLength(token) - 1) == containString(token,"+") || (stringLength(token) - 1) == containString(token,"-"))
          {
            return -LEXICAL_ERR;
          }
          ungetc(ch,stdin);
          state = TT_EXPONENT;
          continue;
        }

        if (ch == EOL)
        {
          // e nesmie byt posledne
          if ((stringLength(token) - 1) == containString(token,"E") || (stringLength(token) - 1) == containString(token,"e"))
          {
            return -LEXICAL_ERR;
          }
          // AK to konci + alebo -
          if ((stringLength(token) - 1) == containString(token,"+") || (stringLength(token) - 1) == containString(token,"-"))
          {
            return -LEXICAL_ERR;
          }
          ungetc(ch,stdin);
          return TT_EXPONENT;
        }


        initString(&tmp);
        if (isSymbol(ch,tmp,state) != LEXICAL_ERR)
        {
          ungetc(ch,stdin);
          freeString(tmp);
          // e nesmie byt posledne
          if ((stringLength(token) - 1) == containString(token,"E") || (stringLength(token) - 1) == containString(token,"e"))
          {
            return -LEXICAL_ERR;
          }
          // AK to konci + alebo -
          if ((stringLength(token) - 1) == containString(token,"+") || (stringLength(token) - 1) == containString(token,"-"))
          {
            return -LEXICAL_ERR;
          }
          return TT_EXPONENT;
        }
        else
        {
          freeString(tmp);
          return -LEXICAL_ERR;
        }
      break;

      case COMMENT_LINE:
        line_counter++;
        // Cakame na koniec riadku vtedy koment konci
        if (ch == EOL)
        {
          state = INIT;
          ungetc(ch,stdin);
          continue;
        }
      break;

      case COMMENT_BLOCK_START:

      // e
      if (ch != 'e')
      {
        return -LEXICAL_ERR;
      }

      ch = getchar();
      // g
      if (ch != 'g')
      {
        return -LEXICAL_ERR;
      }

      ch = getchar();
      // i
      if (ch != 'i')
      {
        return -LEXICAL_ERR;
      }

      ch = getchar();
      // n
      if (ch != 'n')
      {
        return -LEXICAL_ERR;
      }

      ch = getchar();
      // TAB, Medzera, EOL
      if (!(ch == ' ' || ch == '\n' || ch == '\t'))
      {
        // AK neni vraciame LEXICAL err
        return -LEXICAL_ERR;
      }

      // Ak je EOL
      if (ch == '\n')
      {
        // Vratime naspat ch
        ungetc(ch,stdin);
      }
      state = COMMENT_BLOCK_END;
      continue;

      break;

      case COMMENT_BLOCK_END:
        // Ak sme natrafily na EOF skorej ako na =end
        if (ch == EOF)
        {
          // Vratime chybu
          return -LEXICAL_ERR;
        }


        // Ak sme natrafily na EOL
        if (ch == EOL)
        {
          // Povieme ze mame novy riadok
          line_counter = 0;
          continue;
        }

        // Ak sme na zaciatku riadku
        if (line_counter == 0)
        {
          // Skontrolujeme ci char je =
          if (ch == '=')
          {
            // Ziskame dalsie 4 charaktery
            char tmp_chr_0 = getchar();
            char tmp_chr_1 = getchar();
            char tmp_chr_2 = getchar();
            char tmp_chr_3 = getchar();

            // Nesmie nastat EOF
            if (tmp_chr_0 == EOF || tmp_chr_1 == EOF || tmp_chr_2 == EOF || tmp_chr_3 == EOF)
            {
              // Vratime chybu
              return -LEXICAL_ERR;
            }

            // Skontrolujeme ci sa jednotlive charaktery rovnaju end
            if (!(tmp_chr_0 == 'e' && tmp_chr_1 == 'n' && tmp_chr_2 == 'd'))
            {
              ungetc(tmp_chr_0,stdin);
              ungetc(tmp_chr_1,stdin);
              ungetc(tmp_chr_2,stdin);
              ungetc(tmp_chr_3,stdin);
              continue;
            }
            // Ak je to medzera alebo tab
            if (tmp_chr_3 == ' ' || tmp_chr_3 == '\t')
            {
              // Mozu za tym nasledovat charaktery ktore ignorujeme
              state = COMMENT_BLOCK_END_ENTER;
              continue;
            }
            // Ak je to EOL
            if (tmp_chr_3 == '\n')
            {
              // Zmenime stav na inicializaciu
              state = INIT;
              // Vratime naspat EOL
              ungetc('\n',stdin);
              // Skocime na zaciatok
              continue;
            }
          }
        }
        line_counter++;

      break;

      case COMMENT_BLOCK_END_ENTER:
        // Ignorujeme vsetko dokial nenajdeme enter
        if (ch == EOL)
        {
          // Zmenime stav na inicializaciu
          state = INIT;
          // Vratime naspat EOL
          ungetc('\n',stdin);
          // Skocime na zaciatok
          continue;
        }
      break;

      case IDENTIFIER:
        /*
          IDENTIFIER sa moze skladat z pismen, cislic a _ a pri funkciach moze byt
          ukoncene ? alebo !
            state = INIT;
          ak nasleduje nieco ine vratime LEXICAL_ERR
        */

        // Alebo _ <a,z> <A,Z> <0,9>
        if (ch == '_' || isalnum(ch))
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
          ungetc(ch,stdin);
          return isKeyword(token);
        }

        // Ak je to zaciatok komentaru
        if (ch == '#')
        {
          // Vratime charakter
          ungetc(ch,stdin);
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
        if (isSymbol(ch,tmp,state) != -LEXICAL_ERR)
        {
          freeString(tmp);
          ungetc(ch,stdin);
          return isKeyword(token);
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
