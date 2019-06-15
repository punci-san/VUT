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

static int isSymbol(char ch,String *token)
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
          char next_char = getchar();

          // Ak za = nasleduje b je pravdepodobne ze to moze byt =begin
          if (next_char == 'b')
          {
            appendChar(ch,token);
            appendChar(next_char,token);
            state = COMMENT_BLOCK_START;
            continue;
          }
          line_counter++;
          // Ak nie tak vratime spat ukazovatel o 2
          ungetc(next_char,stdin);
        }

        // Ak to zacina # je to zaciatok riadkoveho komentara
        if (ch == '#')
        {
          line_counter++;
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

          // Pridame charakter
          //appendChar(ch,token);

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
          state = TT_INTEGER;
          continue;
        }

        // Ak je to novy riadok
        if (ch == EOL)
        {
          // Posunieme sa o jeden charakter
          ungetc(ch,stdin);
          return TT_INTEGER;
        }

        initString(&tmp);
        if (isSymbol(ch,tmp) != -LEXICAL_ERR)
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
          state = TT_FLOAT;
          continue;
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
        if (isSymbol(ch,tmp) != -LEXICAL_ERR)
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
        /*
          Exponent je definovany ako
          E[+/-]%d
        */
        if (ch == '+' || ch == '-')
        {
          if (charOccurances(token,ch) >= 1)
          {
            return -LEXICAL_ERR;
          }
          appendChar(ch,token);
          continue;
        }
        if (isdigit(ch))
        {
          appendChar(ch,token);
          continue;
        }
        if (ch == ' ')
        {
          // e nesmie byt posledne
          if ((stringLength(token) - 1) == containString(token,"e"))
          {
            return -LEXICAL_ERR;
          }
          return TT_EXPONENT;
        }

        // Ak to zacina # je to zaciatok riadkoveho komentara
        if (ch == '#')
        {
          // Ak sa rovnaju LEXICAL
          if ((stringLength(token) - 1) == containString(token,"e"))
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
          if ((stringLength(token) - 1) == containString(token,"e"))
          {
            return -LEXICAL_ERR;
          }
          ungetc(ch,stdin);
          return TT_EXPONENT;
        }

        initString(&tmp);
        if (isSymbol(ch,tmp) != LEXICAL_ERR)
        {
          ungetc(ch,stdin);
          freeString(tmp);
          // e nesmie byt posledne
          if ((stringLength(token) - 1) == containString(token,"e"))
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
      {
        // Vytvorime premennu
        char tmp_token_0 = '=';
        char tmp_token_1 = 'b';
        char tmp_token_2 = ch;

        // Skontrolujeme ci je to =begin na zaciatku
        if (line_counter != 0)
        {
          // Ak neni vratime naspat a zobereme iba =
          ungetc(tmp_token_2,stdin);
          ungetc(tmp_token_1,stdin);
          ungetc(tmp_token_0,stdin);

          // ZObereme =
          ch = getchar();

          // Clearneme token
          clearString(token);

          return isSymbol(ch,token);
        }

        appendChar(ch,token);
        ch = getchar();

        appendChar(ch,token);
        char tmp_token_3 = ch;

        ch = getchar();
        appendChar(ch,token);
        char tmp_token_4 = ch;

        ch = getchar();
        appendChar(ch,token);
        char tmp_token_5 = ch;

        ch = getchar();
        appendChar(ch,token);
        char tmp_token_6 = ch;

        // Ak sa rovnaju =begin alebo =begin/t
        if ((strcmp(token->str,"=begin ") == 0) || (strcmp(token->str,"=begin\t") == 0))
        {
          // Zmenime status a skocime na COMMENT_BLOCK_END
          clearString(token);
          state = COMMENT_BLOCK_END;
          continue;
        }
        // Inac ak je za =begin EOL
        else if (strcmp(token->str,"=begin\n") == 0)
        {
          clearString(token);
          state = COMMENT_BLOCK_END;
          // Vratime posledny EOL
          ungetc('\n',stdin);
          continue;
        }
        // Inac sa nerovna a vratime vsetky tokeny
        else
        {
          // Vratime vsetky tokeny
          ungetc(tmp_token_6,stdin);
          ungetc(tmp_token_5,stdin);
          ungetc(tmp_token_4,stdin);
          ungetc(tmp_token_3,stdin);
          ungetc(tmp_token_2,stdin);
          ungetc(tmp_token_1,stdin);
          ungetc(tmp_token_0,stdin);

          // Ziskame charakter =
          ch = getchar();

          clearString(token);

          // Vratime to ako =
          return isSymbol(ch,token);
        }

        // Zistime ci sa rovnaju
      }

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

        // Ak mame =
        if (ch == '=')
        {
          // Pridame ho do tokenu
          appendChar(ch,token);

          // Ziskame char
          ch = getchar();

          // Ak sme natrafily na EOF skorej ako na =end
          if (ch == EOF)
          {
            // Vratime chybu
            return -LEXICAL_ERR;
          }

          // Pridame ho do tokenu
          appendChar(ch,token);

          char tmp_token_0 = ch;

          // Ziskame char
          ch = getchar();

          // Ak sme natrafily na EOF skorej ako na =end
          if (ch == EOF)
          {
            // Vratime chybu
            return -LEXICAL_ERR;
          }

          // Pridame ho do tokenu
          appendChar(ch,token);

          char tmp_token_1 = ch;

          // Ziskame char
          ch = getchar();

          // Ak sme natrafily na EOF skorej ako na =end
          if (ch == EOF)
          {
            // Vratime chybu
            return -LEXICAL_ERR;
          }

          // Pridame ho do tokenu
          appendChar(ch,token);

          char tmp_token_2 = ch;

          // Ziskame char
          ch = getchar();

          // Ak sme natrafily na EOF skorej ako na =end
          if (ch == EOF)
          {
            // Vratime chybu
            return -LEXICAL_ERR;
          }

          // Pridame ho do tokenu
          appendChar(ch,token);

          char tmp_token_3 = ch;

          // Zistime ci token je =end
          if ((strcmp(token->str,"=end ") == 0) || (strcmp(token->str,"=end\t") == 0))
          {
            // Skontrolujeme ci je to =begin na zaciatku
            if (line_counter != 0)
            {
              // Ak neni vratime naspat a zobereme iba =
              ungetc(tmp_token_3,stdin);
              ungetc(tmp_token_2,stdin);
              ungetc(tmp_token_1,stdin);
              ungetc(tmp_token_0,stdin);

              // Clearneme token
              clearString(token);

              continue;
            }
            // Ak je clearneme token
            clearString(token);
            // Nastavime stav
            state = COMMENT_BLOCK_END_ENTER;
            // Skocime hore
            continue;
          }
          // Zistime ci je =end EOL
          else if (strcmp(token->str,"=end\n") == 0)
          {
            // Skontrolujeme ci je to =end\n na zaciatku
            if (line_counter != 0)
            {
              // Ak neni vratime naspat a zobereme iba =
              ungetc(tmp_token_3,stdin);
              ungetc(tmp_token_2,stdin);
              ungetc(tmp_token_1,stdin);
              ungetc(tmp_token_0,stdin);

              // Clearneme token
              clearString(token);

              continue;
            }

            // Clearneme string
            clearString(token);
            // Zmenime stav
            state = INIT;
            // Vratime naspat \n
            ungetc('\n',stdin);
            // SKocime hore
            continue;
          }
          // Inac clearujeme token a cakame na =end
          else
          {
            // Clearneme token
            clearString(token);

            // Vratime charaktery
            ungetc(tmp_token_3,stdin);
            ungetc(tmp_token_2,stdin);
            ungetc(tmp_token_1,stdin);
            ungetc(tmp_token_0,stdin);

            // Skocime hore
            continue;
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
        if (isSymbol(ch,tmp) != -LEXICAL_ERR)
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
