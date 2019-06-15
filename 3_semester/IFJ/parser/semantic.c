#include "semantic.h"
#include "err_code.h"
#include "generate_code.h"

#include <stdlib.h>
#include <stdio.h>  // TODO Vymaz docasne na printovanie


int semCheckVar(struct HashTable *hTable,char *str)
{
  struct Item *item = NULL;
  String *tmp_str;
  initString(&tmp_str);

  // Ziskame premennu bez LF@
  returnVarName(str,tmp_str);

  // Identifikator musi byt definovany v Symtabli
  if ((item = HashTableSearch(hTable,tmp_str->str)) == NULL)
  {
    // Uvolnime string
    freeString(tmp_str);
    // Vratime error
    return SEMANTIC_PROG_ERR;
  }
  // A identifikator mus byt typu TT_IDENTIFIKATOR
  if (item->data.tokenType != TT_IDENTIFIKATOR)
  {
    // Uvolnime string
    freeString(tmp_str);
    // Vratime chybu
    return SEMANTIC_PROG_ERR;
  }

  // Ked mame item tak skontrolujeme ci je inicializovany
  if (item->data.inicialization == 0)
  {
    // Uvolnime string
    freeString(tmp_str);
    // Neinicializovana variable
    return SEMANTIC_PROG_ERR;
  }
  // Uvolnime string
  freeString(tmp_str);
  // Inac vsetko OK
  return 0;
}
/*
  ------------------------------------------------
*/

/*
  Convertneme string a specialne znaky na string typu IFJcode18
  Na typ string@hello\010
*/
void convertString(char *str,String *tmp_str)
{
  // Vycistime string pre istotu
  clearString(tmp_str);

  // Appendneme string@
  appendString("string@",tmp_str);

  // A zacneme prechadzat cely str a upravovat ho na IFJcode18 string
  for (unsigned i = 0; i < strlen(str); i++)
  {
    // Ak je charakter < a,z > tak ho appendneme
    if (('a' <= str[i] && 'z' >= str[i]) || ('A' <= str[i] && 'Z' >= str[i]))
    {
      appendChar(str[i],tmp_str);
    }
    // Ak je to cislo
    else if ('0' <= str[i] && '9' >= str[i])
    {
      appendChar(str[i],tmp_str);
    }
    // Inac je to specialny charakter a ten konvertujeme
    else
    {
      char sprintf_str[200];
      sprintf(sprintf_str,"\\%03d",(str[i] < 0) ? ((str[i] * -1) + 127) : str[i]);
      appendString(sprintf_str,tmp_str);
    }
  }
}

// Vytvorime z 85 int@85
void convertInt(char *str,String *tmp_str)
{
  // Vycistime string
  clearString(tmp_str);

  // Char array for 64 bits
  char str_int[21];

  // Convertneme string na int a naspat na string
  sprintf(str_int,"%i",atoi(str));

  // Appedneme int@
  appendString("int@",tmp_str);

  // Appendneme integer
  appendString(str_int,tmp_str);

}

void convertFun(char *str,String *tmp_str)
{
  // Vycistime string
  clearString(tmp_str);

  // Appendneme $fun
  appendString("$fun$",tmp_str);

  // Appendneme nazov funkcie
  appendString(str,tmp_str);
}

// Convertneme float a exponent ktory prezenieme cez %a
void convertFloat(char *str,String *tmp_str)
{
  // Vysitime string
  clearString(tmp_str);

  // Appendneme float@
  appendString("float@",tmp_str);

  // Float holder
  char float_str[200];

  // Convertneme string na IFJcode18 float
  sprintf(float_str,"%a",strtof(str,NULL));

  // Appendneme
  appendString(float_str,tmp_str);
}

// Convertneme float a exponent ktory prezenieme cez %a
void convertNil(String *tmp_str)
{
  // Vycistime string
  clearString(tmp_str);

  // Appedneme int@
  appendString("nil@nil",tmp_str);
}

// Inicializuje premennu v hashTabli
int inicializeVar(struct HashTable *hTable,char *var)
{
  struct Item *item = NULL;
  // String
  String *tmp_str;
  initString(&tmp_str);

  returnVarName(var,tmp_str);

  if ((item = HashTableSearch(hTable,tmp_str->str)) == NULL)
  {
    // Uvolnime String
    freeString(tmp_str);
    // A vratime chybu
    return SEMANTIC_PROG_ERR;
  }
  // Inac sme nasli tak inicializujeme
  item->data.inicialization = 1;

  // Uvolnime a vratime uspech
  freeString(tmp_str);
  return 0;
}

// Inicializuje funkciu a nastavi pocet premennych
int inicializeFun(struct HashTable *hTable,char *var,int number)
{
  // Ziskame pointer k premennej
  struct Item *item = NULL;

  // Ak item je NULL
  if ((item = HashTableSearch(hTable,var)) == NULL)
  {
    // Nastala nasa chyba
    return INTERNAL_COMP_ERR;
  }

  // Nastavime pocet premennych
  item->data.var_number = number;

  // Inicializujeme
  item->data.inicialization = 1;

  // Vratime OK
  return 0;
}

// Vrati variable name ked tam dame napr LF@a tak vrati a
void returnVarName(char *var,String *str)
{
  // Vytvorime string
  String *tmp_str;
  initString(&tmp_str);

  // Pridame do tmp_var variable
  appendString(var,tmp_str);

  // Vycistime string
  clearString(str);
  int index = containString(tmp_str,"@");

  // Ak sme nenasli nic vraciame len variable
  if (index < 0)
  {
    // Pridame do stringu
    appendString(var,str);
    // Uvolnime string
    freeString(tmp_str);
    // Dame variable a ukoncime
    return;
  }
  // Inac rozdelime string a vratime
  index++;

  // Rozdelime na indexe cize na @|
  splitString(tmp_str,str,index);

  // Uvolnime string
  freeString(tmp_str);
}

void returnVar(char *var,String *str)
{
  // Vytvorime string
  String *tmp_str;
  initString(&tmp_str);

  // Pridame do tmp_var variable
  appendString(var,tmp_str);

  // Vycistime string
  clearString(str);

  // Zistime ci var obsahuje @
  int index = containString(tmp_str,"@");

  // Ak sme nenasli nic tak potrebujeme pridat pred meno @
  if (index < 0)
  {
    // Appendneme LF@
    appendString("LF@",str);

    // Appendneme este nazov
    appendString(var,str);

    // Uvolnime string
    freeString(tmp_str);
    return;
  }
  // Inac vratime to co sme mali
  copyString(tmp_str->str,str);

  // Uvolnime string
  freeString(tmp_str);
}
