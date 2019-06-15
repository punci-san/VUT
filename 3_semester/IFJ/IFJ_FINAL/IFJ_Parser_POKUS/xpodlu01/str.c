/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Pavel Podlužanský <xpodlu01@stud.fit.vutbr.cz>
 *
 **/

#include "str.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
  initString naalocuje novu strukturu String a prida ju na str pointer
  a naalocuje pamat pre string,

  - length = mnozstvo charakterov v arrai charakterov v ->str bez '\0'
  - str = array charakterov ukoncena '\0'
  - alloc = mnozstvo alokovanych charakterov
*/
void initString(struct String **str)
{
  *str = (struct String *)malloc(sizeof(struct String));
  (*str)->length = 0;
  (*str)->alloc = 10;
  (*str)->str = malloc(sizeof(char)*10);

  for (int i = 0; i < 10; i++)
  {
    (*str)->str[i] = '\0';
  }
}

/*
  appendChar() pripevni charakter ch na koniec struktury String,
  zvysi length o 1 a ak je potrebne realocuje pamat pre string

  - ch charakter na appendnutie
  - str = pointer na String ktory menime
*/
void appendChar(char ch,struct String *str)
{
  // Musime brat v potaz novy charakter + '\0' znak
  if (str->alloc <= str->length + 1)
  {
    str->str = realloc(str->str,(str->length + 2) * sizeof(char));
    str->alloc = str->length + 2;
  }
  // Appendneme character
  str->str[str->length] = ch;
  str->length++;

  for (int i = str->length; i < str->alloc; i++)
  {
    str->str[i] = '\0';
  }
}

int charOccurances(struct String *str,char ch)
{
  int n = 0;
  for (int i = 0; i < stringLength(str); i++)
  {
    if (str->str[i] == ch)
    {
      n++;
    }
  }
  return n;
}

/*
  appendne array charakterov str_2 na koniec struktury String str_1

  - str_1 je struktura Stringu
  - str_2 je array charakterov na appendnutie
*/
void appendString(char *str_2,struct String *str_1)
{
  int str_2_len = strlen(str_2);

  for (int i = 0; i < str_2_len; i++)
  {
    appendChar(str_2[i],str_1);
  }
}

// Vrati dlzku struktury String
int stringLength(struct String *str)
{
  return str->length;
}

int containString(struct String *str,char *syntax)
{
  int syntax_len = strlen(syntax);
  int str_len = stringLength(str);
  int ok = 0;

  if (stringLength(str) < syntax_len)
  {
    return -1;
  }

  for (int i = 0; i < str_len; i++)
  {
    if (str->str[i] == syntax[0])
    {
      ok++;
      for (int j = 1; j < syntax_len; j++)
      {
        if (str->str[i+j] == syntax[j])
        {
          ok++;
        }
        else
        {
          ok = 0;
        }
      }
    }
    if (ok == syntax_len)
    {
      return i;
    }
  }
  return -1;
}

/*
  Skopiruje array charakterov str_1 do struktury String str_2

  - str_1 array charakterov ktore sa vlozia do struktury String str_2
  - str_2 struktura String ktora sa prepise na str_1
*/
void copyString(char *str_1,struct String *str_2)
{
  str_2->length = strlen(str_1);
  str_2->str = (char *)realloc(str_2->str,str_2->length+1);
  strcpy(str_2->str,str_1);
  str_2->alloc = str_2->length+1;
}

/*
  Rozdeli strukturu Stringu str na dve casti, 0 - split, split +1 - dlzka str

  - str struktura String ktorej array charakterov sa rozdeli na dve
  - splitted_string struktura String ktora bude obsahovat druhu polovicu charakterov split+1 - dlzka str
  - split Integer ktory hovori o tom kde sa array charakterov rozdeli na dve casti
*/
void splitString(struct String *str,struct String *splitted_string,int split)
{
  int str_len = stringLength(str);
  if (str_len <= 0)
  {
    return;
  }

  // Vycistime druhy string ak je tam nieco nahodou
  clearString(splitted_string);

  int i;
  for (i = split; i < str_len; i++)
  {
    // Appendujeme k jednemu
    appendChar(str->str[i],splitted_string);
    str->str[i] = '\0'; // Cistime druhy
  }
  str->length = split;
  splitted_string->length = i - split;

}

/*
  Appendne str_2 strukturu String k druhej str_1 strukture Stringu

  - str_1 struktura Stringu na ktoru budeme appendovat str_2
  - str_2 struktura Stringu ktora sa bude pridavat na koniec str_1
*/
void joinString(struct String *str_1,struct String *str_2)
{
  // Dlzku stringu zistime
  int str_2_l = stringLength(str_2);

  // appendneme str_2 k stringu str_1
  for (int i = 0; i < str_2_l; i++)
  {
    // Character po charactery pridame pokial nie sme na konci
    appendChar(str_2->str[i],str_1);
  }
}

// Vymazeme array charakterov zo str
void clearString(struct String *str)
{
  char *t = "";
  strcpy(str->str,t);
  str->length = 0;
}

/*
  Uvolnime alokovane miesto struktury String str
*/
void freeString(struct String *str)
{
  free(str->str);
  free(str);
}
