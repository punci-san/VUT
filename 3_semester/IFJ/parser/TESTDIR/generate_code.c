#include "generate_code.h"
#include "str.h"
#include "err_code.h"
#include "semantic.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define FILE_HEADER ".IFJcode18"

const char * const Instruction_Word[] = {
  "DEFVAR",
  "MOVE",
  /**/
  "CREATEFRAME",
  "PUSHFRAME",
  "POPFRAME",
  "CALL",
  "RETURN",
  /**/
  "PUSHS",
  "POPS",
  "CLEARS",
  /**/
  "ADD",
  "SUB",
  "MUL",
  "DIV",
  "IDIV",
  /**/
  "ADDS",
  "SUBS",
  "MULS",
  "DIVS",
  "IDIVS",
  /**/
  "LT",
  "GT",
  "EQ",
  /**/
  "LTS",
  "GTS",
  "EQS",
  /**/
  "AND",
  "OR",
  "NOT",
  /**/
  "ANDS",
  "ORS",
  "NOTS",
  /**/
  "INT2FLOAT",
  "FLOAT2INT",
  "INT2CHAR",
  "STRI2INT",
  /**/
  "INT2FLOATS",
  "FLOAT2INTS",
  "INT2CHARS",
  "STRI2INTS",
  /**/
  "READ",
  "WRITE",
  /**/
  "CONCAT",
  "STRLEN",
  "GETCHAR",
  "SETCHAR",
  /**/
  "TYPE",
  /**/
  "LABEL",
  /**/
  "JUMP",
  "JUMPIFEQ",
  "JUMPIFNEQ",
  /**/
  "JUMPS",
  "JUMPIFEQS",
  "JUMPIFNEQS",
  /**/
  "EXIT",
  "BREAK",
  "DPRINT"
};

struct InstructionStruct *createInstructionStruct()
{
  struct InstructionStruct *iStruct = malloc(sizeof(struct InstructionStruct));
  iStruct->First = NULL;
  iStruct->Active = NULL;
  iStruct->Last = NULL;
  return iStruct;
}

void prependInstruction(struct InstructionStruct *iStruct,struct InstructionItem *iItem,Instruction_Type type,char *ptr1,char *ptr2,char *ptr3)
{
  struct InstructionItem *item = malloc(sizeof(struct InstructionItem));

  // Nastavime typ
  item->type = type;

  // Vynulujeme itemy
  item->ptr1 = NULL;
  item->ptr2 = NULL;
  item->ptr3 = NULL;

  // Nastavime dalsi na NULL
  item->next = NULL;

  // Nastavime prvy item
  if (ptr1 != NULL)
  {
    String *tmp_str;
    initString(&tmp_str);
    copyString(ptr1,tmp_str);
    item->ptr1 = tmp_str;
  }
  // Nastavime druhy item
  if (ptr2 != NULL)
  {
    String *tmp_str;
    initString(&tmp_str);
    copyString(ptr2,tmp_str);
    item->ptr2 = tmp_str;
  }
  // Nastavime treti item
  if (ptr3 != NULL)
  {
    String *tmp_str;
    initString(&tmp_str);
    copyString(ptr3,tmp_str);
    item->ptr3 = tmp_str;
  }

  // Predchadzajuci item
  item->prev = iItem->prev;

  // Nasledujuci item
  item->next = iItem;

  // Pridame to pred item
  if (iItem->prev != NULL)
  {
    // Nastavime nech lavy ukazuje na nas vlozeny
    iItem->prev->next = item;
  }

  if (iStruct->First == iItem)
  {
    iStruct->First = item;
  }

  // Predchadzajuci item nech je nas item
  iItem->prev = item;
}

void appendInstruction(struct InstructionStruct *iStruct,Instruction_Type type,char *ptr1,char *ptr2,char *ptr3)
{
  struct InstructionItem *item = malloc(sizeof(struct InstructionItem));

  // Nastavime typ
  item->type = type;

  // Vynulujeme itemy
  item->ptr1 = NULL;
  item->ptr2 = NULL;
  item->ptr3 = NULL;

  // Nastavime dalsi na NULL
  item->next = NULL;

  // Nastavime prvy item
  if (ptr1 != NULL)
  {
    String *tmp_str;
    initString(&tmp_str);
    copyString(ptr1,tmp_str);
    item->ptr1 = tmp_str;
  }
  // Nastavime druhy item
  if (ptr2 != NULL)
  {
    String *tmp_str;
    initString(&tmp_str);
    copyString(ptr2,tmp_str);
    item->ptr2 = tmp_str;
  }
  // Nastavime treti item
  if (ptr3 != NULL)
  {
    String *tmp_str;
    initString(&tmp_str);
    copyString(ptr3,tmp_str);
    item->ptr3 = tmp_str;
  }

  // Ak je First prazdny tak nastavime tento na jeho miesto
  if (iStruct->First == NULL)
  {
    iStruct->First = item;
  }

  // Appendneme
  if (iStruct->Last != NULL)
  {
    iStruct->Last->next = item;
  }
  item->prev = iStruct->Last;
  iStruct->Last = item;
}

void freeInstructionItem(struct InstructionItem *item)
{
  // Uvolnime prvy string
  if (item->ptr1 != NULL)
  {
    freeString(item->ptr1);
  }
  // Uvolnime druhy
  if (item->ptr2 != NULL)
  {
    freeString(item->ptr2);
  }
  // Uvolnime treti
  if (item->ptr3 != NULL)
  {
    freeString(item->ptr3);
  }
  // Uvolnime cely item
  free(item);
}

void freeInstructionStruct(struct InstructionStruct *iStruct)
{
  // Nastavime sa na prvy item v zozname
  iStruct->Active = iStruct->First;
  // Nastavime nech tmp_item ukazuje na ten prvy item
  struct InstructionItem *tmp_item = iStruct->Active;

  // A pokial nie sme na konci tak uvolnujeme
  while (tmp_item != NULL)
  {
    // Posunieme sa na dalsi aktivny prvok
    iStruct->Active = iStruct->Active->next;
    // Uvolnime tmp
    freeInstructionItem(tmp_item);
    // Nastavime item na dalsi
    tmp_item = iStruct->Active;
  }
  // Nakoniec uvolnime celu strukturu
  free(iStruct);
}


int generateWhile(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct binStruct *binStruct,int while_index,int *tmp_var_counter)
{
  String *tmp_str;
  String *ret_str;
  initString(&tmp_str);
  initString(&ret_str);

  int errCode = 0;


  // Labely
  char label_a[200];
  char label_b[200];
  char label_c[200];

  // Labely
  sprintf(label_a,"$while$%d$next$check",while_index);
  sprintf(label_b,"$while$%d$end$loop",while_index);
  sprintf(label_c,"$while$%d$loop$continue",while_index);

  // Vygenerujeme vypocet WHILE vyrazu
  if ((errCode = generateExpression(iStruct,hashTable,binStruct,ret_str,tmp_var_counter)) != 0)
  {
    // Ukoncime s chybou
    freeString(tmp_str);
    freeString(ret_str);
    return errCode;
  }

  // Vygenerujeme temporaly variable na ulozenie typu premennej
  if ((errCode = generateTmpVar(hashTable,tmp_str,tmp_var_counter)) != 0)
  {
    // Ukoncime s chybou
    freeString(tmp_str);
    freeString(ret_str);
    return errCode;
  }

  // Zistime typ vypoctu
  appendInstruction(iStruct,IT_TYPE,tmp_str->str,ret_str->str,NULL);

  // Ak neni typ return value bool skaceme na dalsiu kontrolu
  appendInstruction(iStruct,IT_JUMPIFNEQ,label_a,tmp_str->str,"string@bool");

  // AK je tak zkontrolujeme a ak je false skaceme na ukoncenie loopu
  appendInstruction(iStruct,IT_JUMPIFEQ,label_b,ret_str->str,"bool@false");

  // Inac skaceme na pokracovanie loopu
  appendInstruction(iStruct,IT_JUMP,label_c,NULL,NULL);

  // Dalsia kontrola pre nil
  appendInstruction(iStruct,IT_LABEL,label_a,NULL,NULL);

  // Ak je typ nil skaceme na END
  appendInstruction(iStruct,IT_JUMPIFEQ,label_b,tmp_str->str,"string@nil");

  // Inac ideme na IF
  appendInstruction(iStruct,IT_LABEL,label_c,NULL,NULL);

  // Ukoncime s chybou
  freeString(tmp_str);
  freeString(ret_str);
  return 0;
}

/*
  WHILE PODMIENKA
  --------------------------------------------------------------------------------------------------------------------------------------------------------------------
  IF PODMIENKA
*/


int generateIf(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct binStruct *binStruct,int if_index,int *tmp_var_counter)
{
  char label_a[200];
  char label_b[200];
  char label_c[200];

  sprintf(label_a,"$if$else$%d$next$check",if_index);
  sprintf(label_b,"$if$else$%d$if",if_index);
  sprintf(label_c,"$if$else$%d$else",if_index);

  int errCode = 0;
  // String
  String *tmp_str;
  String *ret_str;
  initString(&tmp_str);
  initString(&ret_str);

  // Vygenerujeme vypocet
  if ((errCode = generateExpression(iStruct,hashTable,binStruct,ret_str,tmp_var_counter)) != 0)
  {
    // Ukoncime s chybou
    freeString(tmp_str);
    freeString(ret_str);
    return errCode;
  }

  // Vygenerujeme temporaly variable
  if ((errCode = generateTmpVar(hashTable,tmp_str,tmp_var_counter)) != 0)
  {
    // Ukoncime s chybou
    freeString(tmp_str);
    freeString(ret_str);
    return errCode;
  }
  // Zistime typ vypoctu
  appendInstruction(iStruct,IT_TYPE,tmp_str->str,ret_str->str,NULL);

  // Ak neni typ return value bool skaceme na dalsiu kontrolu
  appendInstruction(iStruct,IT_JUMPIFNEQ,label_a,tmp_str->str,"string@bool");

  // AK je tak zkontrolujeme a ak je false skaceme na else
  appendInstruction(iStruct,IT_JUMPIFEQ,label_c,ret_str->str,"bool@false");

  // Inac skocime na IF
  appendInstruction(iStruct,IT_JUMP,label_b,NULL,NULL);

  // Dalsia kontrola pre nil
  appendInstruction(iStruct,IT_LABEL,label_a,NULL,NULL);

  // Ak je typ nil skaceme na ELSE
  appendInstruction(iStruct,IT_JUMPIFEQ,label_c,tmp_str->str,"string@nil");

  // Inac ideme na IF
  appendInstruction(iStruct,IT_LABEL,label_b,NULL,NULL);

  // Generacia IF komplete
  freeString(tmp_str);
  freeString(ret_str);
  return 0;
}
void generateElse(struct InstructionStruct *iStruct,int if_index)
{
  char label_a[200];
  char label_b[200];

  sprintf(label_a,"$if$else$%d$else",if_index);
  sprintf(label_b,"$if$else$%d$end",if_index);

  appendInstruction(iStruct,IT_JUMP,label_b,NULL,NULL);
  appendInstruction(iStruct,IT_LABEL,label_a,NULL,NULL);

}


/*
  IF PODMIENKA
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------
  ARITMETICKE GENEROVANIE
*/


int generateExpressionDivideCode(struct binNode *node,String *str,struct HashTable *hashTable,struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Labely
  char label_a[200];
  char label_b[200];

  char label_c[200];
  char label_d[200];

  char label_e[200];
  char label_f[200];

  char label_g[200];
  char label_h[200];
  char label_i[200];

  // Inicializujeme ich
  sprintf(label_a,"$label$second$intfloat$check$divide$%d ",*tmp_var_counter);
  sprintf(label_b,"$label$second$floatint$check$divide$%d ",*tmp_var_counter);

  sprintf(label_c,"$label$first$inttofloat$divide$%d ",*tmp_var_counter);
  sprintf(label_d,"$label$second$inttofloat$divide$%d ",*tmp_var_counter);

  sprintf(label_e,"$label$div$divide$%d ",*tmp_var_counter);
  sprintf(label_f,"$label$idiv$divide$%d ",*tmp_var_counter);

  sprintf(label_g,"$label$div$continue$divide$%d ",*tmp_var_counter);
  sprintf(label_h,"$label$idiv$continue$divide$%d ",*tmp_var_counter);

  sprintf(label_i,"$label$end$divide$%d ",*tmp_var_counter);

  // Stringy
  String *tmp_str_var_a;
  String *tmp_str_var_b;

  String *tmp_str_var;

  String *tmp_str_result;
  String *tmp_str_type_a;
  String *tmp_str_type_b;

  // Inicializujeme string
  initString(&tmp_str_var_a);
  initString(&tmp_str_var_b);
  initString(&tmp_str_var);

  // Vytvorime pre lavu stranu
  if (changeToAssembly(((struct binItem *)node->Left)->type,((struct binItem *)node->Left)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre A
  generateTmpVar(hashTable,tmp_str_var_a,&(*tmp_var_counter));

  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_a->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Pre pravu
  if (changeToAssembly(((struct binItem *)node->Right)->type,((struct binItem *)node->Right)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre B
  generateTmpVar(hashTable,tmp_str_var_b,&(*tmp_var_counter));

  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_b->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Inicializujeme string
  initString(&tmp_str_result);
  initString(&tmp_str_type_a);
  initString(&tmp_str_type_b);

  // Vygenerujeme potrebne variable
  generateTmpVar(hashTable,tmp_str_result,&(*tmp_var_counter));

  // Result variable vratime
  copyString(tmp_str_result->str,str);

  // Ostatne 2 pouzijeme na kontrolu typov
  generateTmpVar(hashTable,tmp_str_type_a,&(*tmp_var_counter));
  generateTmpVar(hashTable,tmp_str_type_b,&(*tmp_var_counter));

  // Zistime typy
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_a->str,tmp_str_var_a->str,NULL);
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_b->str,tmp_str_var_b->str,NULL);

  // Vytvorime kontroly prve JUMPY
  appendInstruction(instructionList,IT_JUMPIFEQ,label_a,tmp_str_type_a->str,"string@int");
  appendInstruction(instructionList,IT_JUMPIFEQ,label_b,tmp_str_type_a->str,"string@float");

  // EXIT
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // label_a
  appendInstruction(instructionList,IT_LABEL,label_a,NULL,NULL);
  // Kontrola ci je druhy int/float
  appendInstruction(instructionList,IT_JUMPIFEQ,label_f,tmp_str_type_b->str,"string@int");
  appendInstruction(instructionList,IT_JUMPIFEQ,label_c,tmp_str_type_b->str,"string@float");
  // EXIT
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // label_b
  appendInstruction(instructionList,IT_LABEL,label_b,NULL,NULL);
  // Kontrola ci je druhy int/float
  appendInstruction(instructionList,IT_JUMPIFEQ,label_e,tmp_str_type_b->str,"string@float");
  appendInstruction(instructionList,IT_JUMPIFEQ,label_d,tmp_str_type_b->str,"string@int");
  // EXIT
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // First int2float label
  appendInstruction(instructionList,IT_LABEL,label_c,NULL,NULL);

  // INT2FLOAT prvej variable
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_a->str,tmp_str_var_a->str,NULL);

  // Skocime na nasobenie floatu cize DIV
  appendInstruction(instructionList,IT_JUMP,label_e,NULL,NULL);

  // Second int2float label
  appendInstruction(instructionList,IT_LABEL,label_d,NULL,NULL);

  // INT2FLOAT druhej variable
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_b->str,tmp_str_var_b->str,NULL);

  // Skocime na nasobenie floatu cize DIV
  appendInstruction(instructionList,IT_JUMP,label_e,NULL,NULL);

  // Label pre skok na nasobenie integerov IDIV
  appendInstruction(instructionList,IT_LABEL,label_f,NULL,NULL);

  // Jumpneme dalej ak integer neni 0
  appendInstruction(instructionList,IT_JUMPIFNEQ,label_h,tmp_str_var_b->str,"int@0");

  // Delenie 0 chyba
  appendInstruction(instructionList,IT_EXIT,"int@9",NULL,NULL);

  // Pokracovanie IDIV
  appendInstruction(instructionList,IT_LABEL,label_h,NULL,NULL);

  // IDIV
  appendInstruction(instructionList,IT_IDIV,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);

  // Skocime na koniec label_i
  appendInstruction(instructionList,IT_JUMP,label_i,NULL,NULL);

  // Label pre DIV label_e
  appendInstruction(instructionList,IT_LABEL,label_e,NULL,NULL);

  // Skaceme dalej ak neni float 0
  appendInstruction(instructionList,IT_JUMPIFNEQ,label_g,tmp_str_var_b->str,"float@0x0p+0");

  // Inac delenie 0 cize error
  appendInstruction(instructionList,IT_EXIT,"int@9",NULL,NULL);

  // Label pre pokracovanie DIV label_g
  appendInstruction(instructionList,IT_LABEL,label_g,NULL,NULL);

  // DIV A / B
  appendInstruction(instructionList,IT_DIV,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);

  // Label pre konecny JUMP
  appendInstruction(instructionList,IT_LABEL,label_i,NULL,NULL);

  // Uvolnime stringy
  freeString(tmp_str_var_a);
  freeString(tmp_str_var_b);

  freeString(tmp_str_var);

  freeString(tmp_str_result);
  freeString(tmp_str_type_a);
  freeString(tmp_str_type_b);

  // Zvysime counter variable
  (*tmp_var_counter)++;

  return 0;
}

int generateExpressionMinusMultiplyCode(struct binNode *node,String *str,struct HashTable *hashTable,struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Labely
  char label_a[200];
  char label_b[200];
  char label_c[200];
  char label_d[200];
  char label_e[200];


  // PRve jumpy
  sprintf(label_a,"$label$second$intfloat$check$minusmultiply$%d ",*tmp_var_counter);
  sprintf(label_b,"$label$second$floatint$check$minusmultiply$%d ",*tmp_var_counter);
  sprintf(label_c,"$label$submul$minusmultiply$%d ",*tmp_var_counter);
  sprintf(label_d,"$label$first$inttofloat$minusmultiply$%d ",*tmp_var_counter);
  sprintf(label_e,"$label$second$inttofloat$minusmultiply$%d ",*tmp_var_counter);


  // Stringy
  String *tmp_str_var_a;
  String *tmp_str_var_b;

  String *tmp_str_var;

  String *tmp_str_result;
  String *tmp_str_type_a;
  String *tmp_str_type_b;

  // Inicializujeme string
  initString(&tmp_str_var_a);
  initString(&tmp_str_var_b);
  initString(&tmp_str_var);

  // Vytvorime pre lavu stranu
  if (changeToAssembly(((struct binItem *)node->Left)->type,((struct binItem *)node->Left)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre A
  generateTmpVar(hashTable,tmp_str_var_a,&(*tmp_var_counter));


  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_a->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Pre pravu
  if (changeToAssembly(((struct binItem *)node->Right)->type,((struct binItem *)node->Right)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre B
  generateTmpVar(hashTable,tmp_str_var_b,&(*tmp_var_counter));

  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_b->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Inicializujeme string
  initString(&tmp_str_result);
  initString(&tmp_str_type_a);
  initString(&tmp_str_type_b);

  // Vygenerujeme potrebne variable
  generateTmpVar(hashTable,tmp_str_result,&(*tmp_var_counter));

  // Result variable vratime
  copyString(tmp_str_result->str,str);

  // Ostatne 2 pouzijeme na kontrolu typov
  generateTmpVar(hashTable,tmp_str_type_a,&(*tmp_var_counter));
  generateTmpVar(hashTable,tmp_str_type_b,&(*tmp_var_counter));

  // Zistime typy
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_a->str,tmp_str_var_a->str,NULL);
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_b->str,tmp_str_var_b->str,NULL);

  // Vytvorime kontroly prve JUMPY
  appendInstruction(instructionList,IT_JUMPIFEQ,label_a,tmp_str_type_a->str,"string@int");
  appendInstruction(instructionList,IT_JUMPIFEQ,label_b,tmp_str_type_a->str,"string@float");

  // EXIT
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // label_a
  appendInstruction(instructionList,IT_LABEL,label_a,NULL,NULL);

  // Kontrola ci je druhy int/float
  appendInstruction(instructionList,IT_JUMPIFEQ,label_c,tmp_str_type_b->str,"string@int");
  appendInstruction(instructionList,IT_JUMPIFEQ,label_d,tmp_str_type_b->str,"string@float");

  // EXIT
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // label_c
  appendInstruction(instructionList,IT_LABEL,label_b,NULL,NULL);

  // Kontrola ci je druhy float/int
  appendInstruction(instructionList,IT_JUMPIFEQ,label_c,tmp_str_type_b->str,"string@float");
  appendInstruction(instructionList,IT_JUMPIFEQ,label_e,tmp_str_type_b->str,"string@int");

  // EXIT
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // label_d
  appendInstruction(instructionList,IT_LABEL,label_d,NULL,NULL);

  // INT2FLOAT A
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_a->str,tmp_str_var_a->str,NULL);

  // Jump na add label_g
  appendInstruction(instructionList,IT_JUMP,label_c,NULL,NULL);

  // label_f
  appendInstruction(instructionList,IT_LABEL,label_e,NULL,NULL);

  // INT2FLOAT B
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_b->str,tmp_str_var_b->str,NULL);

  // label_g
  appendInstruction(instructionList,IT_LABEL,label_c,NULL,NULL);

  // Odcitanie ak je TT_MINUS
  if (node->Middle->type == TT_MINUS)
  {
    // SUB
    appendInstruction(instructionList,IT_SUB,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);
  }
  // Nasobenie ak je TT_MULTIPLY
  else
  {
    // MUL
    appendInstruction(instructionList,IT_MUL,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);
  }

  // Uvolnime string
  freeString(tmp_str_result);
  freeString(tmp_str_var_a);
  freeString(tmp_str_var_b);

  freeString(tmp_str_var);
  freeString(tmp_str_type_a);
  freeString(tmp_str_type_b);

  // Zvysime counter variable
  (*tmp_var_counter)++;

  // Ukoncime
  return 0;
}

int generateExpressionPlusCode(struct binNode *node,String *str,struct HashTable *hashTable,struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Labely
  char label_a[200];
  char label_b[200];
  char label_c[200];

  char label_d[200];
  char label_e[200];
  char label_f[200];

  char label_g[200];
  char label_h[200];

  // PRve jumpy
  sprintf(label_a,"$label$second$string$check$plus$%d ",*tmp_var_counter);
  sprintf(label_b,"$label$second$intfloat$check$plus$%d ",*tmp_var_counter);
  sprintf(label_c,"$label$second$floatint$check$plus$%d ",*tmp_var_counter);

  sprintf(label_d,"$label$concat$plus$%d ",*tmp_var_counter);
  sprintf(label_e,"$label$firsttofloat$plus$%d ",*tmp_var_counter);
  sprintf(label_f,"$label$secondtofloat$plus$%d ",*tmp_var_counter);

  sprintf(label_g,"$label$add$plus$%d ",*tmp_var_counter);
  sprintf(label_h,"$label$end$plus$%d ",*tmp_var_counter);

  // Stringy
  String *tmp_str_var_a;
  String *tmp_str_var_b;

  String *tmp_str_var;

  String *tmp_str_result;
  String *tmp_str_type_a;
  String *tmp_str_type_b;

  // Inicializujeme string
  initString(&tmp_str_var_a);
  initString(&tmp_str_var_b);
  initString(&tmp_str_var);

  // Vytvorime pre lavu stranu
  if (changeToAssembly(((struct binItem *)node->Left)->type,((struct binItem *)node->Left)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre A
  generateTmpVar(hashTable,tmp_str_var_a,&(*tmp_var_counter));


  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_a->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Pre pravu
  if (changeToAssembly(((struct binItem *)node->Right)->type,((struct binItem *)node->Right)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre B
  generateTmpVar(hashTable,tmp_str_var_b,&(*tmp_var_counter));

  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_b->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Inicializujeme string
  initString(&tmp_str_result);
  initString(&tmp_str_type_a);
  initString(&tmp_str_type_b);

  // Vygenerujeme potrebne variable
  generateTmpVar(hashTable,tmp_str_result,&(*tmp_var_counter));

  // Result variable vratime
  copyString(tmp_str_result->str,str);

  // Ostatne 2 pouzijeme na kontrolu typov
  generateTmpVar(hashTable,tmp_str_type_a,&(*tmp_var_counter));
  generateTmpVar(hashTable,tmp_str_type_b,&(*tmp_var_counter));

  // Zistime typy
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_a->str,tmp_str_var_a->str,NULL);
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_b->str,tmp_str_var_b->str,NULL);

  // Vytvorime kontroly prve JUMPY
  appendInstruction(instructionList,IT_JUMPIFEQ,label_a,tmp_str_type_a->str,"string@string");
  appendInstruction(instructionList,IT_JUMPIFEQ,label_b,tmp_str_type_a->str,"string@int");
  appendInstruction(instructionList,IT_JUMPIFEQ,label_c,tmp_str_type_a->str,"string@float");

  // EXIT
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // label_a
  appendInstruction(instructionList,IT_LABEL,label_a,NULL,NULL);

  // Jump ak je aj druhy string
  appendInstruction(instructionList,IT_JUMPIFEQ,label_d,tmp_str_type_b->str,"string@string");

  // EXIT
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // label_b
  appendInstruction(instructionList,IT_LABEL,label_b,NULL,NULL);

  // Kontrola ci je druhy int/float
  appendInstruction(instructionList,IT_JUMPIFEQ,label_g,tmp_str_type_b->str,"string@int");
  appendInstruction(instructionList,IT_JUMPIFEQ,label_e,tmp_str_type_b->str,"string@float");

  // EXIT
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // label_c
  appendInstruction(instructionList,IT_LABEL,label_c,NULL,NULL);

  // Kontrola ci je druhy float/int
  appendInstruction(instructionList,IT_JUMPIFEQ,label_g,tmp_str_type_b->str,"string@float");
  appendInstruction(instructionList,IT_JUMPIFEQ,label_f,tmp_str_type_b->str,"string@int");

  // EXIT
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // label_d
  appendInstruction(instructionList,IT_LABEL,label_d,NULL,NULL);

  // CONCAT a b
  appendInstruction(instructionList,IT_CONCAT,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);

  // Jump na koniec label_h
  appendInstruction(instructionList,IT_JUMP,label_h,NULL,NULL);

  // label_e
  appendInstruction(instructionList,IT_LABEL,label_e,NULL,NULL);

  // INT2FLOAT A
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_a->str,tmp_str_var_a->str,NULL);

  // Jump na add label_g
  appendInstruction(instructionList,IT_JUMP,label_g,NULL,NULL);

  // label_f
  appendInstruction(instructionList,IT_LABEL,label_f,NULL,NULL);

  // INT2FLOAT B
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_b->str,tmp_str_var_b->str,NULL);

  // label_g
  appendInstruction(instructionList,IT_LABEL,label_g,NULL,NULL);

  // ADD
  appendInstruction(instructionList,IT_ADD,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);

  // label_h
  appendInstruction(instructionList,IT_LABEL,label_h,NULL,NULL);

  // Uvolnime string
  freeString(tmp_str_result);
  freeString(tmp_str_var_a);
  freeString(tmp_str_var_b);

  freeString(tmp_str_var);
  freeString(tmp_str_type_a);
  freeString(tmp_str_type_b);

  // Zvysime counter variable
  (*tmp_var_counter)++;

  // Ukoncime
  return 0;
}



// Vygenerujeme IDIV alebo DIV
int generateExpressionDivide(struct binNode *node,String *str,struct HashTable *hashTable, struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Vytvorime string
  String *tmp_str;
  initString(&tmp_str);

  // Errorcode
  int errCode = 0;

  // Ak je prvy INTEGER
  if (((struct binItem*)node->Left)->type == TT_INTEGER)
  {
    // Ak je druhy INTEGER
    if (((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Ak je druhy integer generujeme var
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      // Stringy
      String *tmp_str_1;
      String *tmp_str_2;

      // Inicializujeme ich
      initString(&tmp_str_1);
      initString(&tmp_str_2);

      // Convertneme prvy integer
      convertInt(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy integer
      convertInt(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak sa rovna INT B 0 tak je to delenie 0 co je chyba
      if (strcmp(tmp_str_2->str,"int@0") == 0)
      {
        freeString(tmp_str_1);
        freeString(tmp_str_2);
        freeString(tmp_str);
        return SEMANTIC_ZERO_DIVISION;
      }

      // Teraz mozme vygenerovat CONCAT
      appendInstruction(instructionList,IT_IDIV,str->str,tmp_str_1->str,tmp_str_2->str);

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo FLOAT generujeme kod
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Ak sa jedna o TT_IDENTIFIKATOR
      if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
      {
        // Tak robime semanticku kontrolu premennej
        if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
        {
          //Uvolnime string
          freeString(tmp_str);
          // Ukoncime
          return errCode;
        }
      }
      if ((errCode = generateExpressionDivideCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy FLOAT
  else if (((struct binItem*)node->Left)->type == TT_FLOAT || ((struct binItem*)node->Left)->type == TT_EXPONENT)
  {
    // Ak je druhy FLOAT
    if (((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Ak je druhy integer generujeme var
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      // Stringy
      String *tmp_str_1;
      String *tmp_str_2;

      // Inicializujeme ich
      initString(&tmp_str_1);
      initString(&tmp_str_2);

      // Convertneme prvy integer
      convertFloat(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy integer
      convertFloat(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Skontrolujeme ci delime 0
      if (strcmp(tmp_str_2->str,"float@0x0p+0") == 0)
      {
        freeString(tmp_str);
        freeString(tmp_str_1);
        freeString(tmp_str_2);
        return SEMANTIC_ZERO_DIVISION;
      }

      // Teraz mozme vygenerovat CONCAT
      appendInstruction(instructionList,IT_DIV,str->str,tmp_str_1->str,tmp_str_2->str);

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo INTEGER generujem kod
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Je to Identifikator ?
      if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
      {
        // Urobime kontrolu identifikatora
        if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
        {
          //Uvolnime string
          freeString(tmp_str);
          // Ukoncime
          return errCode;
        }
      }
      if ((errCode = generateExpressionDivideCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy IDENTIFIKATOR
  else if (((struct binItem*)node->Left)->type == TT_IDENTIFIKATOR)
  {
    // Ak sa jedna o neho urobime semanticku kontrolu premennej
    if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Left)->str->str)) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    // Skontrolujeme ci sa jedna o TT_IDENTIFIKATOR
    if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
    {
      // Ak hej urobime semanticku kontrolu
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
    }
    if ((errCode = generateExpressionDivideCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    //Uvolnime string
    freeString(tmp_str);
    // Ukoncime
    return 0;
  }
  else
  {
    freeString(tmp_str);
    // Naslo sa tam nieco co sa nemalo NIL
    return SEMANTIC_PROG_ERR;
  }
}

// Vygenerujeme bud nasobenie alebo odcitanie
int generateExpressionMinusMultiply(struct binNode *node,String *str,struct HashTable *hashTable, struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Vytvorime string
  String *tmp_str;
  initString(&tmp_str);

  // Errorcode
  int errCode = 0;

  // Ak je prvy INTEGER
  if (((struct binItem*)node->Left)->type == TT_INTEGER)
  {
    // Ak je druhy INTEGER
    if (((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Ak je druhy integer generujeme var
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      // Stringy
      String *tmp_str_1;
      String *tmp_str_2;

      // Inicializujeme ich
      initString(&tmp_str_1);
      initString(&tmp_str_2);

      // Convertneme prvy integer
      convertInt(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy integer
      convertInt(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Teraz mozeme vygenerovat MINUS
      if (((struct binItem*)node->Middle)->type == TT_MINUS)
      {
        // Vygenerujeme MINUS
        appendInstruction(instructionList,IT_SUB,str->str,tmp_str_1->str,tmp_str_2->str);
      }
      // Inac mozeme vygenerovat MULTIPLY
      else
      {
        // Vygenerujeme MULTIPLY
        appendInstruction(instructionList,IT_MUL,str->str,tmp_str_1->str,tmp_str_2->str);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo FLOAT generujeme kod
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Ak sa jedna o TT_IDENTIFIKATOR
      if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
      {
        // Tak robime semanticku kontrolu premennej
        if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
        {
          //Uvolnime string
          freeString(tmp_str);
          // Ukoncime
          return errCode;
        }
      }
      if ((errCode = generateExpressionMinusMultiplyCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy FLOAT
  else if (((struct binItem*)node->Left)->type == TT_FLOAT || ((struct binItem*)node->Left)->type == TT_EXPONENT)
  {
    // Ak je druhy FLOAT
    if (((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Ak je druhy integer generujeme var
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      // Stringy
      String *tmp_str_1;
      String *tmp_str_2;

      // Inicializujeme ich
      initString(&tmp_str_1);
      initString(&tmp_str_2);

      // Convertneme prvy integer
      convertFloat(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy integer
      convertFloat(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak je typ TT_MINUS
      if (((struct binItem*)node->Middle)->type == TT_MINUS)
      {
        // Mozme vygenerovat SUB
        appendInstruction(instructionList,IT_SUB,str->str,tmp_str_1->str,tmp_str_2->str);
      }
      // Ak je typ TT_MULTIPLY
      else
      {
        // Generujeme MUL
        appendInstruction(instructionList,IT_MUL,str->str,tmp_str_1->str,tmp_str_2->str);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo INTEGER generujem kod
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Je to Identifikator ?
      if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
      {
        // Urobime kontrolu identifikatora
        if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
        {
          //Uvolnime string
          freeString(tmp_str);
          // Ukoncime
          return errCode;
        }
      }
      if ((errCode = generateExpressionMinusMultiplyCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy IDENTIFIKATOR
  else if (((struct binItem*)node->Left)->type == TT_IDENTIFIKATOR)
  {
    // Ak sa jedna o neho urobime semanticku kontrolu premennej
    if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Left)->str->str)) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    // Skontrolujeme ci sa jedna o TT_IDENTIFIKATOR
    if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
    {
      // Ak hej urobime semanticku kontrolu
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
    }
    if ((errCode = generateExpressionMinusMultiplyCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    //Uvolnime string
    freeString(tmp_str);
    // Ukoncime
    return 0;
  }
  else
  {
    freeString(tmp_str);
    // Naslo sa tam nieco co sa nemalo NIL
    return SEMANTIC_PROG_ERR;
  }
}

// Vygeneruje plus alebo concat alebo zavola generaciu kodu
int generateExpressionPlus(struct binNode *node,String *str,struct HashTable *hashTable, struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Vytvorime string
  String *tmp_str;
  initString(&tmp_str);

  // Errorcode
  int errCode = 0;

  // Ak je prvy STRING
  if (((struct binItem*)node->Left)->type == TT_STRING)
  {
    // Musi byt aj druhy ak nie je je to SEMANTIC_TYPE_COMP_ERR
    if (((struct binItem*)node->Right)->type == TT_STRING)
    {
      // AK je druhy string tak generujeme variable
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      String *tmp_str_1;
      String *tmp_str_2;
      initString(&tmp_str_1);
      initString(&tmp_str_2);
      // Convertneme prvy string
      convertString(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy string
      convertString(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Teraz mozme vygenerovat CONCAT
      appendInstruction(instructionList,IT_CONCAT,str->str,tmp_str_1->str,tmp_str_2->str);
      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // AK to nie je STRING moze to byt identifikator
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
    {
      // Ak sa jedna o neho urobime semanticku kontrolu premennej
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      if ((errCode = generateExpressionPlusCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy INTEGER
  else if (((struct binItem*)node->Left)->type == TT_INTEGER)
  {
    // Ak je druhy INTEGER
    if (((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Ak je druhy integer generujeme var
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      // Stringy
      String *tmp_str_1;
      String *tmp_str_2;

      // Inicializujeme ich
      initString(&tmp_str_1);
      initString(&tmp_str_2);

      // Convertneme prvy integer
      convertInt(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy integer
      convertInt(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Teraz mozme vygenerovat CONCAT
      appendInstruction(instructionList,IT_ADD,str->str,tmp_str_1->str,tmp_str_2->str);

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo FLOAT generujeme kod
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Ak sa jedna o TT_IDENTIFIKATOR
      if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
      {
        // Tak robime semanticku kontrolu premennej
        if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
        {
          //Uvolnime string
          freeString(tmp_str);
          // Ukoncime
          return errCode;
        }
      }
      if ((errCode = generateExpressionPlusCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy FLOAT
  else if (((struct binItem*)node->Left)->type == TT_FLOAT || ((struct binItem*)node->Left)->type == TT_EXPONENT)
  {
    // Ak je druhy FLOAT
    if (((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Ak je druhy integer generujeme var
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      // Stringy
      String *tmp_str_1;
      String *tmp_str_2;

      // Inicializujeme ich
      initString(&tmp_str_1);
      initString(&tmp_str_2);

      // Convertneme prvy integer
      convertFloat(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy integer
      convertFloat(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Teraz mozme vygenerovat CONCAT
      appendInstruction(instructionList,IT_ADD,str->str,tmp_str_1->str,tmp_str_2->str);

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo INTEGER generujem kod
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Je to Identifikator ?
      if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
      {
        // Urobime kontrolu identifikatora
        if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
        {
          //Uvolnime string
          freeString(tmp_str);
          // Ukoncime
          return errCode;
        }
      }
      if ((errCode = generateExpressionPlusCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy IDENTIFIKATOR
  else if (((struct binItem*)node->Left)->type == TT_IDENTIFIKATOR)
  {
    // Ak sa jedna o neho urobime semanticku kontrolu premennej
    if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Left)->str->str)) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    // Skontrolujeme ci sa jedna o TT_IDENTIFIKATOR
    if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
    {
      // Ak hej urobime semanticku kontrolu
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
    }
    if ((errCode = generateExpressionPlusCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    //Uvolnime string
    freeString(tmp_str);
    // Ukoncime
    return 0;
  }
  else
  {
    freeString(tmp_str);
    // Naslo sa tam nieco co sa nemalo NIL
    return SEMANTIC_PROG_ERR;
  }
}


/*
  ARITMETICKE generovanie

 --------------------------------------------------------------------------------------------------------------------------------------------------

 BINARNE generovanie
*/

int generateExpressionGreaterLesserEqualCode(struct binNode *node,String *str,struct HashTable *hashTable,struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Labely
  char label_a[200];
  char label_b[200];
  char label_c[200];
  char label_d[200];
  char label_e[200];
  char label_f[200];

  // Label pre kontrolu druheho stringu
  sprintf(label_a,"$label$second$string$check$greaterlesserequal$%d ",*tmp_var_counter);

  // LABEL pre kontrolu INT/FLOAT
  sprintf(label_b,"$label$second$intfloat$check$greaterlesserequal$%d ",*tmp_var_counter);

  // LABEL pre kontrolu FLOAT/INT
  sprintf(label_c,"$label$second$floatint$check$greaterlesserequal$%d ",*tmp_var_counter);

  // Labely pre konverzie
  sprintf(label_d,"$label$first$inttofloat$greaterlesserequal$%d ",*tmp_var_counter);
  sprintf(label_e,"$label$second$inttofloat$greaterlesserequal$%d ",*tmp_var_counter);

  // Labely pre LT/GT
  sprintf(label_f,"$label$gtltoreq$greaterlesser$%d ",*tmp_var_counter);

  // Stringy
  String *tmp_str_var_a;
  String *tmp_str_var_b;

  String *tmp_str_var;

  String *tmp_str_result;
  String *tmp_str_result_1;

  String *tmp_str_type_a;
  String *tmp_str_type_b;

  // Inicializujeme string
  initString(&tmp_str_var_a);
  initString(&tmp_str_var_b);
  initString(&tmp_str_var);

  // Vytvorime pre lavu stranu
  if (changeToAssembly(((struct binItem *)node->Left)->type,((struct binItem *)node->Left)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre A
  generateTmpVar(hashTable,tmp_str_var_a,&(*tmp_var_counter));

  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_a->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Pre pravu
  if (changeToAssembly(((struct binItem *)node->Right)->type,((struct binItem *)node->Right)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre B
  generateTmpVar(hashTable,tmp_str_var_b,&(*tmp_var_counter));

  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_b->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Inicializujeme string
  initString(&tmp_str_result);
  initString(&tmp_str_result_1);
  initString(&tmp_str_type_a);
  initString(&tmp_str_type_b);

  // Vygenerujeme potrebne variable
  generateTmpVar(hashTable,tmp_str_result,&(*tmp_var_counter));
  generateTmpVar(hashTable,tmp_str_result_1,&(*tmp_var_counter));

  // Result variable vratime
  copyString(tmp_str_result->str,str);

  // Ostatne 2 pouzijeme na kontrolu typov
  generateTmpVar(hashTable,tmp_str_type_a,&(*tmp_var_counter));
  generateTmpVar(hashTable,tmp_str_type_b,&(*tmp_var_counter));

  // Zistime typy
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_a->str,tmp_str_var_a->str,NULL);
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_b->str,tmp_str_var_b->str,NULL);

  // Ak prvy je STRING skaceme kontrolu druheho STRINGU
  appendInstruction(instructionList,IT_JUMPIFEQ,label_a,tmp_str_type_a->str,"string@string");

  // Ak je prvy INT skaceme na kontrolu INT/FLOAT druheho
  appendInstruction(instructionList,IT_JUMPIFEQ,label_b,tmp_str_type_a->str,"string@int");

  // Ak je prvy FLOAT skaceme na kontrolu FLOAT/INT druheho
  appendInstruction(instructionList,IT_JUMPIFEQ,label_c,tmp_str_type_a->str,"string@float");

  // Inac koncime chybou
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // Kontrola druheho STRINGU
  appendInstruction(instructionList,IT_LABEL,label_a,NULL,NULL);

  // Ak je aj druhy STRING skaceme na LT/GT
  appendInstruction(instructionList,IT_JUMPIFEQ,label_f,tmp_str_type_b->str,"string@string");

  // Inac EXIT 4
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // Kontrola INT/FLOAT
  appendInstruction(instructionList,IT_LABEL,label_b,NULL,NULL);

  // Ak je druhy INT skaceme na GT/LT
  appendInstruction(instructionList,IT_JUMPIFEQ,label_f,tmp_str_type_b->str,"string@int");

  // Ak je druhy FLOAT skaceme na INT2FLOAT prvej premennej
  appendInstruction(instructionList,IT_JUMPIFEQ,label_d,tmp_str_type_b->str,"string@float");

  // Inac EXIT 4
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // Label pre kontrolu FLOAT/INT
  appendInstruction(instructionList,IT_LABEL,label_c,NULL,NULL);

  // Ak je druhy FLOAT mozme skakat na GT/LT
  appendInstruction(instructionList,IT_JUMPIFEQ,label_f,tmp_str_type_b->str,"string@float");

  //Ak je druhy INT skaceme na INT2FLOAT druhej premennej
  appendInstruction(instructionList,IT_JUMPIFEQ,label_e,tmp_str_type_b->str,"string@int");

  // Inac EXIT 4
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // Label pre INT2FLOAT prvej premennej
  appendInstruction(instructionList,IT_LABEL,label_d,NULL,NULL);

  // INT2FLOAT prvej premennej
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_a->str,tmp_str_var_a->str,NULL);

  // JUMP na GT/LT
  appendInstruction(instructionList,IT_JUMP,label_f,NULL,NULL);

  // Label pre INT2FLOAT druhej premennej
  appendInstruction(instructionList,IT_LABEL,label_e,NULL,NULL);

  // INT2FLOAT druhej premennej
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_b->str,tmp_str_var_b->str,NULL);

  // Label pre vypocet GT/LT
  appendInstruction(instructionList,IT_LABEL,label_f,NULL,NULL);

  // Ak je <=
  if (node->Middle->type == TT_LOWER_EQUAL)
  {
    // Appendneme instrukciu na porovnanie A < B a ulozime ju do resutl_1
    appendInstruction(instructionList,IT_LT,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);

    // Appendneme instrukciu na porovnanie A == B a ulozime ju do result
    appendInstruction(instructionList,IT_EQ,tmp_str_result_1->str,tmp_str_var_a->str,tmp_str_var_b->str);

    // Urobime OR instrukcii
    appendInstruction(instructionList,IT_OR,tmp_str_result->str,tmp_str_result->str,tmp_str_result_1->str);
  }
  // Ak je >
  else
  {
    // Appendneme instrukciu na porovnanie A < B a ulozime ju do resutl_1
    appendInstruction(instructionList,IT_GT,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);

    // Appendneme instrukciu na porovnanie A == B a ulozime ju do result
    appendInstruction(instructionList,IT_EQ,tmp_str_result_1->str,tmp_str_var_a->str,tmp_str_var_b->str);

    // Urobime OR instrukcii
    appendInstruction(instructionList,IT_OR,tmp_str_result->str,tmp_str_result->str,tmp_str_result_1->str);
  }

  // Uvolnime string
  freeString(tmp_str_result);
  freeString(tmp_str_result_1);
  freeString(tmp_str_var_a);
  freeString(tmp_str_var_b);

  freeString(tmp_str_var);
  freeString(tmp_str_type_a);
  freeString(tmp_str_type_b);

  // Zvysime counter variable
  (*tmp_var_counter)++;

  // Ukoncime
  return 0;
}

int generateExpressionGreaterLesserCode(struct binNode *node,String *str,struct HashTable *hashTable,struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Labely
  char label_a[200];
  char label_b[200];
  char label_c[200];
  char label_d[200];
  char label_e[200];
  char label_f[200];

  // Label pre kontrolu druheho stringu
  sprintf(label_a,"$label$second$string$check$greaterlesser$%d ",*tmp_var_counter);

  // LABEL pre kontrolu INT/FLOAT
  sprintf(label_b,"$label$second$intfloat$check$greaterlesser$%d ",*tmp_var_counter);

  // LABEL pre kontrolu FLOAT/INT
  sprintf(label_c,"$label$second$floatint$check$greaterlesser$%d ",*tmp_var_counter);

  // Labely pre konverzie
  sprintf(label_d,"$label$first$inttofloat$greaterlesser$%d ",*tmp_var_counter);
  sprintf(label_e,"$label$second$inttofloat$greaterlesser$%d ",*tmp_var_counter);

  // Labely pre LT/GT
  sprintf(label_f,"$label$gtlt$greaterlesser$%d ",*tmp_var_counter);

  // Stringy
  String *tmp_str_var_a;
  String *tmp_str_var_b;

  String *tmp_str_var;

  String *tmp_str_result;
  String *tmp_str_type_a;
  String *tmp_str_type_b;

  // Inicializujeme string
  initString(&tmp_str_var_a);
  initString(&tmp_str_var_b);
  initString(&tmp_str_var);

  // Vytvorime pre lavu stranu
  if (changeToAssembly(((struct binItem *)node->Left)->type,((struct binItem *)node->Left)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre A
  generateTmpVar(hashTable,tmp_str_var_a,&(*tmp_var_counter));

  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_a->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Pre pravu
  if (changeToAssembly(((struct binItem *)node->Right)->type,((struct binItem *)node->Right)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre B
  generateTmpVar(hashTable,tmp_str_var_b,&(*tmp_var_counter));

  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_b->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Inicializujeme string
  initString(&tmp_str_result);
  initString(&tmp_str_type_a);
  initString(&tmp_str_type_b);

  // Vygenerujeme potrebne variable
  generateTmpVar(hashTable,tmp_str_result,&(*tmp_var_counter));

  // Result variable vratime
  copyString(tmp_str_result->str,str);

  // Ostatne 2 pouzijeme na kontrolu typov
  generateTmpVar(hashTable,tmp_str_type_a,&(*tmp_var_counter));
  generateTmpVar(hashTable,tmp_str_type_b,&(*tmp_var_counter));

  // Zistime typy
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_a->str,tmp_str_var_a->str,NULL);
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_b->str,tmp_str_var_b->str,NULL);

  // Ak prvy je STRING skaceme kontrolu druheho STRINGU
  appendInstruction(instructionList,IT_JUMPIFEQ,label_a,tmp_str_type_a->str,"string@string");

  // Ak je prvy INT skaceme na kontrolu INT/FLOAT druheho
  appendInstruction(instructionList,IT_JUMPIFEQ,label_b,tmp_str_type_a->str,"string@int");

  // Ak je prvy FLOAT skaceme na kontrolu FLOAT/INT druheho
  appendInstruction(instructionList,IT_JUMPIFEQ,label_c,tmp_str_type_a->str,"string@float");

  // Inac koncime chybou
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // Kontrola druheho STRINGU
  appendInstruction(instructionList,IT_LABEL,label_a,NULL,NULL);

  // Ak je aj druhy STRING skaceme na LT/GT
  appendInstruction(instructionList,IT_JUMPIFEQ,label_f,tmp_str_type_b->str,"string@string");

  // Inac EXIT 4
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // Kontrola INT/FLOAT
  appendInstruction(instructionList,IT_LABEL,label_b,NULL,NULL);

  // Ak je druhy INT skaceme na GT/LT
  appendInstruction(instructionList,IT_JUMPIFEQ,label_f,tmp_str_type_b->str,"string@int");

  // Ak je druhy FLOAT skaceme na INT2FLOAT prvej premennej
  appendInstruction(instructionList,IT_JUMPIFEQ,label_d,tmp_str_type_b->str,"string@float");

  // Inac EXIT 4
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // Label pre kontrolu FLOAT/INT
  appendInstruction(instructionList,IT_LABEL,label_c,NULL,NULL);

  // Ak je druhy FLOAT mozme skakat na GT/LT
  appendInstruction(instructionList,IT_JUMPIFEQ,label_f,tmp_str_type_b->str,"string@float");

  //Ak je druhy INT skaceme na INT2FLOAT druhej premennej
  appendInstruction(instructionList,IT_JUMPIFEQ,label_e,tmp_str_type_b->str,"string@int");

  // Inac EXIT 4
  appendInstruction(instructionList,IT_EXIT,"int@4",NULL,NULL);

  // Label pre INT2FLOAT prvej premennej
  appendInstruction(instructionList,IT_LABEL,label_d,NULL,NULL);

  // INT2FLOAT prvej premennej
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_a->str,tmp_str_var_a->str,NULL);

  // JUMP na GT/LT
  appendInstruction(instructionList,IT_JUMP,label_f,NULL,NULL);

  // Label pre INT2FLOAT druhej premennej
  appendInstruction(instructionList,IT_LABEL,label_e,NULL,NULL);

  // INT2FLOAT druhej premennej
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_b->str,tmp_str_var_b->str,NULL);

  // Label pre vypocet GT/LT
  appendInstruction(instructionList,IT_LABEL,label_f,NULL,NULL);

  // Ak je <
  if (node->Middle->type == TT_LOWER)
  {
    // Appendneme instrukciu na porovnanie A < B
    appendInstruction(instructionList,IT_LT,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);
  }
  // Ak je >
  else
  {
    // Appendneme instrukciu na porovnanie A > B
    appendInstruction(instructionList,IT_GT,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);
  }

  // Uvolnime string
  freeString(tmp_str_result);
  freeString(tmp_str_var_a);
  freeString(tmp_str_var_b);

  freeString(tmp_str_var);
  freeString(tmp_str_type_a);
  freeString(tmp_str_type_b);

  // Zvysime counter variable
  (*tmp_var_counter)++;

  // Ukoncime
  return 0;
}

int generateExpressionBothEqualCode(struct binNode *node,String *str,struct HashTable *hashTable,struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Labely
  char label_a[200];
  char label_b[200];
  char label_c[200];
  char label_d[200];
  char label_e[200];
  char label_f[200];
  char label_g[200];
  char label_h[200];
  char label_i[200];

  // Labely dalsich kontrol
  sprintf(label_a,"$label$next$nil$check$bothequal$%d ",*tmp_var_counter);
  sprintf(label_b,"$label$next$bool$check$bothequal$%d ",*tmp_var_counter);
  sprintf(label_c,"$label$next$intfloat$check$bothequal$%d ",*tmp_var_counter);
  sprintf(label_d,"$label$next$floatint$check$bothequal$%d ",*tmp_var_counter);

  // Labely pre konverzie
  sprintf(label_e,"$label$first$inttofloat$bothequal$%d ",*tmp_var_counter);
  sprintf(label_f,"$label$second$inttofloat$bothequal$%d ",*tmp_var_counter);

  // Labely pre EQ
  sprintf(label_g,"$label$eqnot$bothequal$%d ",*tmp_var_counter);

  // Label pre MOVE LF@result bool@false
  sprintf(label_h,"$label$false$bothequal$%d ",*tmp_var_counter);
  sprintf(label_i,"$label$end$bothequal$%d ",*tmp_var_counter);


  // Stringy
  String *tmp_str_var_a;
  String *tmp_str_var_b;

  String *tmp_str_var;

  String *tmp_str_result;
  String *tmp_str_type_a;
  String *tmp_str_type_b;

  // Inicializujeme string
  initString(&tmp_str_var_a);
  initString(&tmp_str_var_b);
  initString(&tmp_str_var);


  // Vytvorime pre lavu stranu
  if (changeToAssembly(((struct binItem *)node->Left)->type,((struct binItem *)node->Left)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre A
  generateTmpVar(hashTable,tmp_str_var_a,&(*tmp_var_counter));


  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_a->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Pre pravu
  if (changeToAssembly(((struct binItem *)node->Right)->type,((struct binItem *)node->Right)->str->str,tmp_str_var) != 0)
  {
    freeString(tmp_str_var);
    freeString(tmp_str_var_a);
    freeString(tmp_str_var_b);
    return SEMANTIC_PROG_ERR;
  }

  // Vygenerujeme variable pre B
  generateTmpVar(hashTable,tmp_str_var_b,&(*tmp_var_counter));

  // Vygenerujeme move
  appendInstruction(instructionList,IT_MOVE,tmp_str_var_b->str,tmp_str_var->str,NULL);

  // Vycistime tmp_str_var
  clearString(tmp_str_var);

  // Inicializujeme string
  initString(&tmp_str_result);
  initString(&tmp_str_type_a);
  initString(&tmp_str_type_b);

  // Vygenerujeme potrebne variable
  generateTmpVar(hashTable,tmp_str_result,&(*tmp_var_counter));

  // Result variable vratime
  copyString(tmp_str_result->str,str);

  // Ostatne 2 pouzijeme na kontrolu typov
  generateTmpVar(hashTable,tmp_str_type_a,&(*tmp_var_counter));
  generateTmpVar(hashTable,tmp_str_type_b,&(*tmp_var_counter));

  // Zistime typy
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_a->str,tmp_str_var_a->str,NULL);
  appendInstruction(instructionList,IT_TYPE,tmp_str_type_b->str,tmp_str_var_b->str,NULL);

  // Ak prvy item neni string skaceme na dalsiu kontrolu
  appendInstruction(instructionList,IT_JUMPIFNEQ,label_a,tmp_str_type_a->str,"string@string");

  // Ak je tak kontrolujeme druhy ci je STRING ak je skaceme na EQ
  appendInstruction(instructionList,IT_JUMPIFEQ,label_g,tmp_str_type_b->str,"string@string");

  // Ak nie skaceme na FALSE
  appendInstruction(instructionList,IT_JUMP,label_h,NULL,NULL);

  // label_a
  appendInstruction(instructionList,IT_LABEL,label_a,NULL,NULL);

  // Ak prvy nie je typu NIL tak skaceme na dalsiu kontrolu
  appendInstruction(instructionList,IT_JUMPIFNEQ,label_b,tmp_str_type_a->str,"string@nil");

  // Ak je tak skontrolujeme ci je druhy NIL ak je tak skaceme na EQ
  appendInstruction(instructionList,IT_JUMPIFEQ,label_g,tmp_str_type_b->str,"string@nil");

  // Ak nie skaceme na FALSE
  appendInstruction(instructionList,IT_JUMP,label_h,NULL,NULL);

  // Skok pre kontrolu BOOL/BOOL
  appendInstruction(instructionList,IT_LABEL,label_b,NULL,NULL);

  // Ak prvy nie je typu BOOL tak skaceme na dalsiu kontrolu
  appendInstruction(instructionList,IT_JUMPIFNEQ,label_c,tmp_str_type_a->str,"string@bool");

  // Ak je tak skontrolujeme ci je druhy BOOL ak je tak skaceme na EQ
  appendInstruction(instructionList,IT_JUMPIFEQ,label_g,tmp_str_type_b->str,"string@bool");

  // Ak nie skaceme na FALSE
  appendInstruction(instructionList,IT_JUMP,label_h,NULL,NULL);

  // LABEL pre kontrolu INT/FLOAT
  appendInstruction(instructionList,IT_LABEL,label_c,NULL,NULL);

  // Ak prvy nie je INT skaceme na dalsiu kontrolu
  appendInstruction(instructionList,IT_JUMPIFNEQ,label_d,tmp_str_type_a->str,"string@int");

  // Ak je druhy INT skaceme na EQ
  appendInstruction(instructionList,IT_JUMPIFEQ,label_g,tmp_str_type_b->str,"string@int");

  // Ak je druhy FLOAT skaceme na INT2FLOAT prvej premennej
  appendInstruction(instructionList,IT_JUMPIFEQ,label_e,tmp_str_type_b->str,"string@float");

  // Ak nie skaceme na FALSE
  appendInstruction(instructionList,IT_JUMP,label_h,NULL,NULL);

  // label_d
  appendInstruction(instructionList,IT_LABEL,label_d,NULL,NULL);

  // Prvy je na 100% FLOAT lebo vsetky ostatne kontroly neplatily a sme tu

  // Ak je druhy FLOAT skaceme na EQ
  appendInstruction(instructionList,IT_JUMPIFEQ,label_g,tmp_str_type_b->str,"string@float");

  // Ak je druhy INT skaceme na INT2FLOAT druhej premennej
  appendInstruction(instructionList,IT_JUMPIFEQ,label_f,tmp_str_type_b->str,"string@int");

  // Ak nie skaceme na FALSE
  appendInstruction(instructionList,IT_JUMP,label_h,NULL,NULL);


  // Label pre INT2FLOAT pre prvu premennu
  appendInstruction(instructionList,IT_LABEL,label_e,NULL,NULL);

  // INT2FLOAT prvej premennej
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_a->str,tmp_str_var_a->str,NULL);

  // Jump na EQ label_f
  appendInstruction(instructionList,IT_JUMP,label_g,NULL,NULL);

  // Label pre INT2FLOAT druhej premennej
  appendInstruction(instructionList,IT_LABEL,label_f,NULL,NULL);

  // INT2FLOAT druhej premennej
  appendInstruction(instructionList,IT_INT2FLOAT,tmp_str_var_b->str,tmp_str_var_b->str,NULL);

  // label_f EQ
  appendInstruction(instructionList,IT_LABEL,label_g,NULL,NULL);

  // Prikaz EQ
  appendInstruction(instructionList,IT_EQ,tmp_str_result->str,tmp_str_var_a->str,tmp_str_var_b->str);

  // Jump na koniec
  appendInstruction(instructionList,IT_JUMP,label_i,NULL,NULL);

  // Label pre FALSE label_h
  appendInstruction(instructionList,IT_LABEL,label_h,NULL,NULL);

  // Move FALSE na result
  appendInstruction(instructionList,IT_MOVE,tmp_str_result->str,"bool@false",NULL);

  // Koniec
  appendInstruction(instructionList,IT_LABEL,label_i,NULL,NULL);

  // Negacia ak mame !=
  if (node->Middle->type == TT_FALSE_EQUAL)
  {
    // NOT
    appendInstruction(instructionList,IT_NOT,tmp_str_result->str,tmp_str_result->str,NULL);
  }

  // Uvolnime string
  freeString(tmp_str_result);
  freeString(tmp_str_var_a);
  freeString(tmp_str_var_b);

  freeString(tmp_str_var);
  freeString(tmp_str_type_a);
  freeString(tmp_str_type_b);

  // Zvysime counter variable
  (*tmp_var_counter)++;

  // Ukoncime
  return 0;
}

int generateExpressionGreaterLesserEqual(struct binNode *node,String *str,struct HashTable *hashTable, struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Vytvorime string
  String *tmp_str;
  initString(&tmp_str);

  // Errorcode
  int errCode = 0;

  // Ak je prvy STRING
  if (((struct binItem*)node->Left)->type == TT_STRING)
  {
    // Musi byt aj druhy ak nie je je to SEMANTIC_TYPE_COMP_ERR
    if (((struct binItem*)node->Right)->type == TT_STRING)
    {
      // Variables
      String *tmp_str_1;  // Lava premenna
      String *tmp_str_2;  // Prava premenna
      String *tmp_str_3;  // tmp result
      initString(&tmp_str_1);
      initString(&tmp_str_2);
      initString(&tmp_str_3);

      // AK je druhy string tak generujeme variable
      generateTmpVar(hashTable,str,&(*tmp_var_counter));
      generateTmpVar(hashTable,tmp_str_3,&(*tmp_var_counter));

      // Convertneme prvy string
      convertString(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy string
      convertString(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak sa jedna o <=
      if (((struct binItem*)node->Middle)->type == TT_LOWER_EQUAL)
      {
        // Vygenerujeme LT instrukciu
        appendInstruction(instructionList,IT_LT,str->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme EQ instrukciu
        appendInstruction(instructionList,IT_EQ,tmp_str_3->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme instrukciu OR a ulozime do str
        appendInstruction(instructionList,IT_OR,str->str,str->str,tmp_str_3->str);
      }
      // Inac generujeme instrukciu pre >=
      else
      {
        // Vygenerujeme GT instrukciu
        appendInstruction(instructionList,IT_GT,str->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme EQ instrukciu
        appendInstruction(instructionList,IT_EQ,tmp_str_3->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme instrukciu OR a ulozime do str
        appendInstruction(instructionList,IT_OR,str->str,str->str,tmp_str_3->str);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str_3);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // AK to nie je STRING moze to byt identifikator
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
    {
      // Ak sa jedna o neho urobime semanticku kontrolu premennej
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      if ((errCode = generateExpressionGreaterLesserEqualCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy INTEGER
  else if (((struct binItem*)node->Left)->type == TT_INTEGER)
  {
    // Ak je druhy INTEGER
    if (((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Variables
      String *tmp_str_1;  // Lava premenna
      String *tmp_str_2;  // Prava premenna
      String *tmp_str_3;  // tmp result
      initString(&tmp_str_1);
      initString(&tmp_str_2);
      initString(&tmp_str_3);

      // AK je druhy int tak generujeme variable
      generateTmpVar(hashTable,str,&(*tmp_var_counter));
      generateTmpVar(hashTable,tmp_str_3,&(*tmp_var_counter));

      // Convertneme prvy int
      convertInt(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy int
      convertInt(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak sa jedna o <=
      if (((struct binItem*)node->Middle)->type == TT_LOWER_EQUAL)
      {
        // Vygenerujeme LT instrukciu
        appendInstruction(instructionList,IT_LT,str->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme EQ instrukciu
        appendInstruction(instructionList,IT_EQ,tmp_str_3->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme instrukciu OR a ulozime do str
        appendInstruction(instructionList,IT_OR,str->str,str->str,tmp_str_3->str);
      }
      // Inac generujeme instrukciu pre >
      else
      {
        // Vygenerujeme GT instrukciu
        appendInstruction(instructionList,IT_GT,str->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme EQ instrukciu
        appendInstruction(instructionList,IT_EQ,tmp_str_3->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme instrukciu OR a ulozime do str
        appendInstruction(instructionList,IT_OR,str->str,str->str,tmp_str_3->str);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str_3);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo FLOAT generujeme kod
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Urobime semanticku kontrolu pravej premennej
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      if ((errCode = generateExpressionGreaterLesserEqualCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy FLOAT
  else if (((struct binItem*)node->Left)->type == TT_FLOAT || ((struct binItem*)node->Left)->type == TT_EXPONENT)
  {
    // Ak je druhy FLOAT
    if (((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Variables
      String *tmp_str_1;  // Lava premenna
      String *tmp_str_2;  // Prava premenna
      String *tmp_str_3;  // tmp result
      initString(&tmp_str_1);
      initString(&tmp_str_2);
      initString(&tmp_str_3);

      // AK je druhy float tak generujeme variable
      generateTmpVar(hashTable,str,&(*tmp_var_counter));
      generateTmpVar(hashTable,tmp_str_3,&(*tmp_var_counter));

      // Convertneme prvy float
      convertFloat(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy float
      convertFloat(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak sa jedna o <=
      if (((struct binItem*)node->Middle)->type == TT_LOWER_EQUAL)
      {
        // Vygenerujeme LT instrukciu
        appendInstruction(instructionList,IT_LT,str->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme EQ instrukciu
        appendInstruction(instructionList,IT_EQ,tmp_str_3->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme instrukciu OR a ulozime do str
        appendInstruction(instructionList,IT_OR,str->str,str->str,tmp_str_3->str);
      }
      // Inac generujeme instrukciu pre >
      else
      {
        // Vygenerujeme GT instrukciu
        appendInstruction(instructionList,IT_GT,str->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme EQ instrukciu
        appendInstruction(instructionList,IT_EQ,tmp_str_3->str,tmp_str_1->str,tmp_str_2->str);

        // Vygenerujeme instrukciu OR a ulozime do str
        appendInstruction(instructionList,IT_OR,str->str,str->str,tmp_str_3->str);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str_3);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo INTEGER generujem kod
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Urobime kontrolu identifikatora
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      if ((errCode = generateExpressionGreaterLesserEqualCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy IDENTIFIKATOR
  else if (((struct binItem*)node->Left)->type == TT_IDENTIFIKATOR)
  {
    // Ak sa jedna o neho urobime semanticku kontrolu premennej
    if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Left)->str->str)) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    // Skontrolujeme ci sa jedna o TT_IDENTIFIKATOR
    if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
    {
      // Ak hej urobime semanticku kontrolu
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
    }
    if ((errCode = generateExpressionGreaterLesserEqualCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    //Uvolnime string
    freeString(tmp_str);
    // Ukoncime
    return 0;
  }
  // Vsetko ostatne co sa moze vyskytnut je chyba
  else
  {
    freeString(tmp_str);
    // Naslo sa tam nieco co sa nemalo NIL
    return SEMANTIC_PROG_ERR;
  }
}

int generateExpressionGreaterLesser(struct binNode *node,String *str,struct HashTable *hashTable, struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Vytvorime string
  String *tmp_str;
  initString(&tmp_str);

  // Errorcode
  int errCode = 0;

  // Ak je prvy STRING
  if (((struct binItem*)node->Left)->type == TT_STRING)
  {
    // Musi byt aj druhy ak nie je je to SEMANTIC_TYPE_COMP_ERR
    if (((struct binItem*)node->Right)->type == TT_STRING)
    {
      // AK je druhy string tak generujeme variable
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      String *tmp_str_1;
      String *tmp_str_2;
      initString(&tmp_str_1);
      initString(&tmp_str_2);
      // Convertneme prvy string
      convertString(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy string
      convertString(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak sa jedna o <
      if (((struct binItem*)node->Middle)->type == TT_LOWER)
      {
        // Vygenerujeme LT instrukciu
        appendInstruction(instructionList,IT_LT,str->str,tmp_str_1->str,tmp_str_2->str);
      }
      // Inac generujeme instrukciu pre >
      else
      {
        // Vygenerujeme instrukciu GT
        appendInstruction(instructionList,IT_GT,str->str,tmp_str_1->str,tmp_str_2->str);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // AK to nie je STRING moze to byt identifikator
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
    {
      // Ak sa jedna o neho urobime semanticku kontrolu premennej
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      if ((errCode = generateExpressionGreaterLesserCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy INTEGER
  else if (((struct binItem*)node->Left)->type == TT_INTEGER)
  {
    // Ak je druhy INTEGER
    if (((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Ak je druhy integer generujeme var
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      // Stringy
      String *tmp_str_1;
      String *tmp_str_2;

      // Inicializujeme ich
      initString(&tmp_str_1);
      initString(&tmp_str_2);

      // Convertneme prvy integer
      convertInt(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy integer
      convertInt(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak sa jedna o <
      if (((struct binItem*)node->Middle)->type == TT_LOWER)
      {
        // Vygenerujeme LT instrukciu
        appendInstruction(instructionList,IT_LT,str->str,tmp_str_1->str,tmp_str_2->str);
      }
      // Inac generujeme pre instrukciu >
      else
      {
        // Vygenerujeme instrukciu GT
        appendInstruction(instructionList,IT_GT,str->str,tmp_str_1->str,tmp_str_2->str);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo FLOAT generujeme kod
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Urobime semanticku kontrolu pravej premennej
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      if ((errCode = generateExpressionGreaterLesserCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy FLOAT
  else if (((struct binItem*)node->Left)->type == TT_FLOAT || ((struct binItem*)node->Left)->type == TT_EXPONENT)
  {
    // Ak je druhy FLOAT
    if (((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Ak je druhy integer generujeme var
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      // Stringy
      String *tmp_str_1;
      String *tmp_str_2;

      // Inicializujeme ich
      initString(&tmp_str_1);
      initString(&tmp_str_2);

      // Convertneme prvy integer
      convertFloat(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy integer
      convertFloat(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak sa jedna o <
      if (((struct binItem*)node->Middle)->type == TT_LOWER)
      {
        // Vygenerujeme LT instrukciu
        appendInstruction(instructionList,IT_LT,str->str,tmp_str_1->str,tmp_str_2->str);
      }
      // Inac generujeme pre instrukciu >
      else
      {
        // Vygenerujeme instrukciu GT
        appendInstruction(instructionList,IT_GT,str->str,tmp_str_1->str,tmp_str_2->str);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo INTEGER generujem kod
    else if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Urobime kontrolu identifikatora
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      if ((errCode = generateExpressionGreaterLesserCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy IDENTIFIKATOR
  else if (((struct binItem*)node->Left)->type == TT_IDENTIFIKATOR)
  {
    // Ak sa jedna o neho urobime semanticku kontrolu premennej
    if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Left)->str->str)) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    // Skontrolujeme ci sa jedna o TT_IDENTIFIKATOR
    if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
    {
      // Ak hej urobime semanticku kontrolu
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
    }
    if ((errCode = generateExpressionGreaterLesserCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    //Uvolnime string
    freeString(tmp_str);
    // Ukoncime
    return 0;
  }
  // Vsetko ostatne co sa moze vyskytnut je chyba
  else
  {
    freeString(tmp_str);
    // Naslo sa tam nieco co sa nemalo NIL
    return SEMANTIC_PROG_ERR;
  }
}

int generateExpressionBothEqual(struct binNode *node,String *str,struct HashTable *hashTable, struct InstructionStruct *instructionList,int *tmp_var_counter)
{
  // Vytvorime string
  String *tmp_str;
  initString(&tmp_str);

  // Errorcode
  int errCode = 0;

  // Ak je prvy STRING
  if (((struct binItem*)node->Left)->type == TT_STRING)
  {
    // Musi byt aj druhy ak nie je je to SEMANTIC_TYPE_COMP_ERR
    if (((struct binItem*)node->Right)->type == TT_STRING)
    {
      // AK je druhy string tak generujeme variable
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      String *tmp_str_1;
      String *tmp_str_2;
      initString(&tmp_str_1);
      initString(&tmp_str_2);
      // Convertneme prvy string
      convertString(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy string
      convertString(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak sa jedna o EQUAL
      if (((struct binItem*)node->Middle)->type == TT_EQUAL)
      {
        // Vygenerujeme EQ instrukciu
        appendInstruction(instructionList,IT_EQ,str->str,tmp_str_1->str,tmp_str_2->str);
      }
      // Inac generujeme EQ ktoru potom znegujeme
      else
      {
        // Vygenerujeme instrukciu EQ
        appendInstruction(instructionList,IT_EQ,str->str,tmp_str_1->str,tmp_str_2->str);

        // A znegujeme ju
        appendInstruction(instructionList,IT_NOT,str->str,str->str,NULL);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // AK to nie je STRING moze to byt identifikator
    else if (((struct binItem*)node->Right)->type == TT_NIL || ((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_INTEGER || ((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Ak to je premenna musime urobit kontrolu
      if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
      {
        // Ak sa jedna o neho urobime semanticku kontrolu premennej
        if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
        {
          //Uvolnime string
          freeString(tmp_str);
          // Ukoncime
          return errCode;
        }
      }
      // Vygenerujeme kod
      if ((errCode = generateExpressionBothEqualCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy NIL
  else if (((struct binItem*)node->Left)->type == TT_NIL)
  {
    // Musi byt aj druhy ak nie je je to SEMANTIC_TYPE_COMP_ERR
    if (((struct binItem*)node->Right)->type == TT_NIL)
    {
      // AK je druhy string tak generujeme variable
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      String *tmp_str_1;
      String *tmp_str_2;
      initString(&tmp_str_1);
      initString(&tmp_str_2);
      // Convertneme prvy string
      convertNil(tmp_str_1);

      // Convertneme druhy string
      convertNil(tmp_str_2);

      // Ak sa jedna o EQUAL
      if (((struct binItem*)node->Middle)->type == TT_EQUAL)
      {
        // Vygenerujeme EQ instrukciu
        appendInstruction(instructionList,IT_EQ,str->str,tmp_str_1->str,tmp_str_2->str);
      }
      // Inac generujeme EQ ktoru potom znegujeme
      else
      {
        // Vygenerujeme instrukciu EQ
        appendInstruction(instructionList,IT_EQ,str->str,tmp_str_1->str,tmp_str_2->str);

        // A znegujeme ju
        appendInstruction(instructionList,IT_NOT,str->str,str->str,NULL);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // AK to nie je STRING moze to byt identifikator
    else if (((struct binItem*)node->Right)->type == TT_STRING || ((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_INTEGER || ((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Skontrolujeme ci to je variable
      if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
      {
        // Staci aby premenna bola definovana
        if (HashTableSearch(hashTable,((struct binItem*)node->Right)->str->str) == NULL)
        {
          //Uvolnime string
          freeString(tmp_str);
          // Ukoncime semantickou chybou
          return SEMANTIC_PROG_ERR;
        }
      }
      // Inac generujeme
      if ((errCode = generateExpressionBothEqualCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy INTEGER
  else if (((struct binItem*)node->Left)->type == TT_INTEGER)
  {
    // Ak je druhy INTEGER
    if (((struct binItem*)node->Right)->type == TT_INTEGER)
    {
      // Ak je druhy integer generujeme var
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      // Stringy
      String *tmp_str_1;
      String *tmp_str_2;

      // Inicializujeme ich
      initString(&tmp_str_1);
      initString(&tmp_str_2);

      // Convertneme prvy integer
      convertInt(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy integer
      convertInt(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak sa jedna o EQUAL
      if (((struct binItem*)node->Middle)->type == TT_EQUAL)
      {
        // Vygenerujeme EQ instrukciu
        appendInstruction(instructionList,IT_EQ,str->str,tmp_str_1->str,tmp_str_2->str);
      }
      // Inac generujeme EQ ktoru potom znegujeme
      else
      {
        // Vygenerujeme instrukciu EQ
        appendInstruction(instructionList,IT_EQ,str->str,tmp_str_1->str,tmp_str_2->str);

        // A znegujeme ju
        appendInstruction(instructionList,IT_NOT,str->str,str->str,NULL);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo FLOAT generujeme kod
    else if (((struct binItem*)node->Right)->type == TT_STRING || ((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_NIL || ((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Ak sa jedna o variable tak robime semanticku kontrolu
      if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
      {
        // Urobime semanticku kontrolu pravej premennej
        if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
        {
          //Uvolnime string
          freeString(tmp_str);
          // Ukoncime
          return errCode;
        }
      }
      if ((errCode = generateExpressionBothEqualCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy FLOAT
  else if (((struct binItem*)node->Left)->type == TT_FLOAT || ((struct binItem*)node->Left)->type == TT_EXPONENT)
  {
    // Ak je druhy FLOAT
    if (((struct binItem*)node->Right)->type == TT_FLOAT || ((struct binItem*)node->Right)->type == TT_EXPONENT)
    {
      // Ak je druhy integer generujeme var
      generateTmpVar(hashTable,str,&(*tmp_var_counter));

      // Stringy
      String *tmp_str_1;
      String *tmp_str_2;

      // Inicializujeme ich
      initString(&tmp_str_1);
      initString(&tmp_str_2);

      // Convertneme prvy integer
      convertFloat(((struct binItem*)node->Left)->str->str,tmp_str_1);

      // Convertneme druhy integer
      convertFloat(((struct binItem*)node->Right)->str->str,tmp_str_2);

      // Ak sa jedna o EQUAL
      if (((struct binItem*)node->Middle)->type == TT_EQUAL)
      {
        // Vygenerujeme EQ instrukciu
        appendInstruction(instructionList,IT_EQ,str->str,tmp_str_1->str,tmp_str_2->str);
      }
      // Inac generujeme EQ ktoru potom znegujeme
      else
      {
        // Vygenerujeme instrukciu EQ
        appendInstruction(instructionList,IT_EQ,str->str,tmp_str_1->str,tmp_str_2->str);

        // A znegujeme ju
        appendInstruction(instructionList,IT_NOT,str->str,str->str,NULL);
      }

      // Uvolnime stringy
      freeString(tmp_str_1);
      freeString(tmp_str_2);
      freeString(tmp_str);
      // Vratime OK
      return 0;
    }
    // Ak je druhy IDENTIFIKATOR alebo INTEGER generujem kod
    else if (((struct binItem*)node->Right)->type == TT_INTEGER || ((struct binItem*)node->Right)->type == TT_STRING || ((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR || ((struct binItem*)node->Right)->type == TT_NIL)
    {
      // Ak je to premenna robime semanticku kontrolu
      if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
      {
        // Urobime kontrolu identifikatora
        if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
        {
          //Uvolnime string
          freeString(tmp_str);
          // Ukoncime
          return errCode;
        }
      }
      if ((errCode = generateExpressionBothEqualCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return 0;
    }
    // Inac chyba
    else
    {
      freeString(tmp_str);
      // Vratime error
      return SEMANTIC_TYPE_COMP_ERR;
    }
  }
  // Ak je prvy IDENTIFIKATOR
  else if (((struct binItem*)node->Left)->type == TT_IDENTIFIKATOR)
  {
    // Ak sa jedna o neho urobime semanticku kontrolu premennej
    if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Left)->str->str)) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    // Skontrolujeme ci sa jedna o TT_IDENTIFIKATOR
    if (((struct binItem*)node->Right)->type == TT_IDENTIFIKATOR)
    {
      // Ak hej urobime semanticku kontrolu
      if ((errCode = semCheckVar(hashTable,((struct binItem*)node->Right)->str->str)) != 0)
      {
        //Uvolnime string
        freeString(tmp_str);
        // Ukoncime
        return errCode;
      }
    }
    if ((errCode = generateExpressionBothEqualCode(node,&(*str),hashTable,instructionList,&(*tmp_var_counter))) != 0)
    {
      //Uvolnime string
      freeString(tmp_str);
      // Ukoncime
      return errCode;
    }
    //Uvolnime string
    freeString(tmp_str);
    // Ukoncime
    return 0;
  }
  // Vsetko ostatne co sa moze vyskytnut je chyba
  else
  {
    freeString(tmp_str);
    // Naslo sa tam nieco co sa nemalo NIL
    return SEMANTIC_PROG_ERR;
  }
}
/*

 --------------------------------------------------------------------------------------------------------------------------------------------------

*/

int generateExpression(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct binStruct *binStruct,String *ret_str,int *tmp_var_counter)
{
  struct binNode *node = getFirstNode(binStruct);
  int errCode = 0;
  String *tmp_str;
  initString(&tmp_str);

  // Loopujeme dokial node nie je NULL
  while (node != NULL)
  {
    // Musi byt posledny inac error
    if (node->type != NT_ALONE)
    {
      return INTERNAL_COMP_ERR;
    }
    // Ak je prazdny stredny item jedna sa o vyraz jednej premennej
    if (node->Middle == NULL)
    {
      if (node->Left != NULL)
      {
        if (isConstant(((struct binItem *)node->Left)->type) || ((struct binItem *)node->Left)->type == TT_IDENTIFIKATOR)
        {
          // Ak je premenna identifikator
          if (((struct binItem *)node->Left)->type == TT_IDENTIFIKATOR)
          {
            // zkontrolujeme ci je definovana
            if (HashTableSearch(hashTable,((struct binItem *)node->Left)->str->str) == NULL)
            {
              // Vratime semantiku
              freeString(tmp_str);
              return SEMANTIC_PROG_ERR;
            }
          }

          // Zmenime premennu alebo konstantu do assembly
          if ((errCode = changeToAssembly(((struct binItem *)node->Left)->type,((struct binItem *)node->Left)->str->str,ret_str)) != 0)
          {
            //Uvolnime string
            freeString(tmp_str);
            return errCode;
          }

          //Uvolnime string
          freeString(tmp_str);
          return 0;
        }
        else
        {
          freeString(tmp_str);
          return SYNTAX_ERR;
        }
      }
      freeString(tmp_str);
      return SYNTAX_ERR;
    }

    // Ked sme dosli na assignment koncime
    if (node->Middle->type == TT_ASSIGNMENT)
    {
      // Priradenie musi to byt posledny NODE
      if (node->prev != NULL)
      {
        // Uvolnime string
        freeString(tmp_str);
        // AK neni vraciame error
        return SEMANTIC_OTHER_ERR;
      }
      // Vygenerujeme instrukciu move
      if (node->Left != NULL && node->Right != NULL)
      {
        // Ziskame nazov variable
        returnVarName(((struct binItem *)node->Left)->str->str,tmp_str);

        // Zkopirujeme do navratovej hodnoty
        copyString(tmp_str->str,ret_str);

        // Vytvorime ju ak neexistuje ak existuje nic nerobi
        if ((errCode = createVarDef(tmp_str->str)) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }

        // vytvorime string
        String *tmp_str_1;
        String *tmp_str_2;
        initString(&tmp_str_1);
        initString(&tmp_str_2);

        // ZIskame IFJcode18 var A
        returnVar(tmp_str->str,tmp_str_1);

        // Zmenime VAR,FLOAT,INT,STRING na assembler tvar
        changeToAssembly(((struct binItem *)node->Right)->type,((struct binItem *)node->Right)->str->str,tmp_str_2);



        // AK je Pravy item premenna kontrolujeme ci existuje
        if (((struct binItem *)node->Right)->type == TT_IDENTIFIKATOR)
        {
          // Ak sa nerovnaju robime semanticku kontrolu
          if (strcmp(((struct binItem *)node->Left)->str->str,((struct binItem *)node->Right)->str->str) != 0)
          {
            // Potrebujeme ziskat meno premennej
            returnVarName(tmp_str_2->str,tmp_str);

            // Skontrolujeme ci existje a ci je inicializovana
            if((errCode = semCheckVar(hashTable,tmp_str->str)) != 0)
            {
              // Uvolnime stringy
              freeString(tmp_str);
              freeString(tmp_str_1);
              freeString(tmp_str_2);
              return errCode;
            }

            // Ziskame naspat meno variable
            returnVar(tmp_str->str,tmp_str_2);
          }
        }

        // Vygenerujeme move instrukciu
        appendInstruction(iStruct,IT_MOVE,tmp_str_1->str,tmp_str_2->str,NULL);

        // Inicializujeme variable A ak nie je inicializovana na hodnotu NIL
        if (((struct binItem *)node->Right)->type != TT_NIL)
        {
          // Inicializujeme variable
          inicializeVar(hashTable,tmp_str_1->str);
        }

        // Zistime nazov lavej premennej a dame ho do tmp_str_2 ktory je uz nepotrebny
        returnVarName(tmp_str_1->str,tmp_str_2);

        // Uvolnime String
        freeString(tmp_str_2);
        freeString(tmp_str_1);
        freeString(tmp_str);
        return 0;
      }
      return INTERNAL_COMP_ERR;
    }
    // Okay mozme generovat
    switch (node->Middle->type)
    {
      case TT_PLUS:
        // Pokusime sa vygenerovat scitanie
        if ((errCode = generateExpressionPlus(node,tmp_str,hashTable,iStruct,&(*tmp_var_counter))) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }

        // Ak je NODE posledny koncime a generujeme retval ak je potrebne
        if (node->prev == NULL)
        {
          copyString(tmp_str->str,ret_str);
          freeString(tmp_str);
          return 0;
        }

        // Inac prebehlo vsetko v poriadku tak znizime
        if ((errCode = setFirstItem(binStruct,node->stackIndex,tmp_str->str)) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }
      break;
      case TT_MINUS:
      case TT_MULTIPLY:
        // Pokusime sa vygenerovat scitanie
        if ((errCode = generateExpressionMinusMultiply(node,tmp_str,hashTable,iStruct,&(*tmp_var_counter))) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }

        // Ak je NODE posledny koncime a generujeme retval ak je potrebne
        if (node->prev == NULL)
        {
          copyString(tmp_str->str,ret_str);
          freeString(tmp_str);
          return 0;
        }

        // Inac prebehlo vsetko v poriadku tak znizime
        if ((errCode = setFirstItem(binStruct,node->stackIndex,tmp_str->str)) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }
      break;
      case TT_DIVISION:
        // Pokusime sa vygenerovat scitanie
        if ((errCode = generateExpressionDivide(node,tmp_str,hashTable,iStruct,&(*tmp_var_counter))) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }

        // Ak je NODE posledny koncime a generujeme retval ak je potrebne
        if (node->prev == NULL)
        {
          copyString(tmp_str->str,ret_str);
          freeString(tmp_str);
          return 0;
        }

        // Inac prebehlo vsetko v poriadku tak znizime
        if ((errCode = setFirstItem(binStruct,node->stackIndex,tmp_str->str)) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }
      break;

      // BINARNE OPERATORY
      // >  <
      case TT_LOWER:
      case TT_HIGHER:
        // Pokusime sa vygenerovat scitanie
        if ((errCode = generateExpressionGreaterLesser(node,tmp_str,hashTable,iStruct,&(*tmp_var_counter))) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }

        // Ak je NODE posledny koncime a generujeme retval ak je potrebne
        if (node->prev == NULL)
        {
          copyString(tmp_str->str,ret_str);
          freeString(tmp_str);
          return 0;
        }

        // Inac prebehlo vsetko v poriadku tak znizime
        if ((errCode = setFirstItem(binStruct,node->stackIndex,tmp_str->str)) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }
      break;

      // ==  !=
      case TT_EQUAL:
      case TT_FALSE_EQUAL:
        // Pokusime sa vygenerovat scitanie
        if ((errCode = generateExpressionBothEqual(node,tmp_str,hashTable,iStruct,&(*tmp_var_counter))) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }


        // Ak je NODE posledny koncime a generujeme retval ak je potrebne
        if (node->prev == NULL)
        {
          copyString(tmp_str->str,ret_str);
          freeString(tmp_str);
          return 0;
        }

        // Inac prebehlo vsetko v poriadku tak znizime
        if ((errCode = setFirstItem(binStruct,node->stackIndex,tmp_str->str)) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }
      break;

      // <=  >=
      case TT_LOWER_EQUAL:
      case TT_HIGHER_EQUAL:
        // Pokusime sa vygenerovat scitanie
        if ((errCode = generateExpressionGreaterLesserEqual(node,tmp_str,hashTable,iStruct,&(*tmp_var_counter))) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }

        // Ak je NODE posledny koncime a generujeme retval ak je potrebne
        if (node->prev == NULL)
        {
          copyString(tmp_str->str,ret_str);
          freeString(tmp_str);
          return 0;
        }

        // Inac prebehlo vsetko v poriadku tak znizime
        if ((errCode = setFirstItem(binStruct,node->stackIndex,tmp_str->str)) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          // Vratime error
          return errCode;
        }
      break;

      default:
        // Uvolnime string
        freeString(tmp_str);
        // Chyba INTERNAL
        return INTERNAL_COMP_ERR;
      break;
    }
    // Ziskame dalsi node
    node = getFirstNode(binStruct);
  }
  return 0;
}

/*
  A = 5
  MOVE instrukciu vygenerujem na ten sposob, ze ak je potrebne vytvorit var A
  tak sa vygeneruje ale var B uz musi existovat, no nemusi byt inicializovana
*/
/*int generateMove(struct InstructionStruct *iStruct,struct HashTable *hashTable,char *var,char *value,TokenType type)
{
  // String
  String *tmp_str;
  // Inicializujeme string
  initString(&tmp_str);
  // Vycistime string
  clearString(tmp_str);

  // Nakopirujeme do stringu variable
  appendString(var,tmp_str);

  // Vytvorime string1
  String *tmp_str1;
  initString(&tmp_str1);

  // Splitneme ju na @
  int index = containString(tmp_str,"@");

  // Ak sme nenasli nic treba vytvorit variable
  if (index < 0)
  {
    // Vytvorime string
    if (createVarDef(tmp_str->str) != 0)
    {
      // Uvolnime stringy
      freeString(tmp_str);
      freeString(tmp_str1);
      // Vratime chybu
      return INTERNAL_COMP_ERR;
    }

    // Clearneme string
    clearString(tmp_str);

    // Vytvorime LF@
    appendString("LF@",tmp_str);

    // Apendneme k tomu variable name
    appendString(tmp_str->str,tmp_str);

    // Splitneme string
    index = containString(tmp_str,"@");
  }

  // Zvysime index aby sa nachadzal na @
  index++;

  // Ziskame nazov variable
  splitString(tmp_str,tmp_str1,index);

  // Treba najst tu vytvorenu variable a inicializovat ju
  struct Item *item = NULL;

  // Vyhladame tu variable
  item = HashTableSearch(hashTable,tmp_str1->str);

  // Ak sa nevytvorilo tak je chyba
  if (item == NULL)
  {
    // Uvolnime stringy
    freeString(tmp_str);
    freeString(tmp_str1);

    // Vraciame chybu
    return INTERNAL_COMP_ERR;
  }

  // Joineme LF@ var
  joinString(tmp_str,tmp_str1);

  // Appendneme instrukciu
  appendInstruction(iStruct,IT_MOVE,tmp_str->str,item->key->str,NULL);

  // Inac inicializujeme
  item->data.inicialization = 1;

  // Uvolnime stringy
  freeString(tmp_str);
  freeString(tmp_str1);

  return 0;
}
*/
void generateReservedFunctionCall(struct InstructionStruct *iStruct)
{
  // Vytvorime obskok
  appendInstruction(iStruct,IT_JUMP,"$$main",NULL,NULL);

  // INPUTS
  appendInstruction(iStruct,IT_LABEL,"$fun$inputs",NULL,NULL);
  appendInstruction(iStruct,IT_PUSHFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@%retval",NULL,NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@%retval","nil@nil",NULL);
  appendInstruction(iStruct,IT_READ,"LF@%retval","string",NULL);
  appendInstruction(iStruct,IT_POPFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_RETURN,NULL,NULL,NULL);

  // INPUTI
  appendInstruction(iStruct,IT_LABEL,"$fun$inputi",NULL,NULL);
  appendInstruction(iStruct,IT_PUSHFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@%retval",NULL,NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@%retval","nil@nil",NULL);
  appendInstruction(iStruct,IT_READ,"LF@%retval","int",NULL);
  appendInstruction(iStruct,IT_POPFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_RETURN,NULL,NULL,NULL);

  // INPUTF
  appendInstruction(iStruct,IT_LABEL,"$fun$inputf",NULL,NULL);
  appendInstruction(iStruct,IT_PUSHFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@%retval",NULL,NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@%retval","nil@nil",NULL);
  appendInstruction(iStruct,IT_READ,"LF@%retval","float",NULL);
  appendInstruction(iStruct,IT_POPFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_RETURN,NULL,NULL,NULL);

  // PRINT
  appendInstruction(iStruct,IT_LABEL,"$fun$print",NULL,NULL);
  appendInstruction(iStruct,IT_PUSHFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@%retval",NULL,NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@%retval","nil@nil",NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@param1",NULL,NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@param1","LF@%1",NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@param2",NULL,NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@param2","nil@nil",NULL);
  appendInstruction(iStruct,IT_LABEL,"$fun$print$loop",NULL,NULL);
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$print$end","LF@param1","int@0");
  appendInstruction(iStruct,IT_POPS,"LF@param2",NULL,NULL);
  appendInstruction(iStruct,IT_WRITE,"LF@param2",NULL,NULL);
  appendInstruction(iStruct,IT_SUB,"LF@param1","LF@param1","int@1");
  appendInstruction(iStruct,IT_JUMP,"$fun$print$loop",NULL,NULL);
  appendInstruction(iStruct,IT_LABEL,"$fun$print$end",NULL,NULL);
  appendInstruction(iStruct,IT_CLEARS,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_POPFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_RETURN,NULL,NULL,NULL);

  // LENGTH
  appendInstruction(iStruct,IT_LABEL,"$fun$length",NULL,NULL);
  appendInstruction(iStruct,IT_PUSHFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@%retval",NULL,NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@%retval","nil@nil",NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@param1",NULL,NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@param1","LF@%1",NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@param2",NULL,NULL);
  appendInstruction(iStruct,IT_TYPE,"LF@param2","LF@param1",NULL);
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$length$strlen","LF@param2","string@string");
  appendInstruction(iStruct,IT_EXIT,"int@4",NULL,NULL);
  appendInstruction(iStruct,IT_LABEL,"$fun$length$strlen",NULL,NULL);
  appendInstruction(iStruct,IT_STRLEN,"LF@%retval","LF@param1",NULL);
  appendInstruction(iStruct,IT_POPFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_RETURN,NULL,NULL,NULL);

  // SUBSTR
  appendInstruction(iStruct,IT_LABEL,"$fun$substr",NULL,NULL);
  appendInstruction(iStruct,IT_PUSHFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@%retval",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@param1",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@param2",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@param3",NULL,NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@%retval","string@",NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@param1","LF@%1",NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@param2","LF@%2",NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@param3","LF@%3",NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@$type$param1",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@$type$param2",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@$type$param3",NULL,NULL);

  appendInstruction(iStruct,IT_TYPE,"LF@$type$param1","LF@param1",NULL);

  appendInstruction(iStruct,IT_LABEL,"$fun$substr$again$check",NULL,NULL);

  appendInstruction(iStruct,IT_TYPE,"LF@$type$param2","LF@param2",NULL);
  appendInstruction(iStruct,IT_TYPE,"LF@$type$param3","LF@param3",NULL);


  appendInstruction(iStruct,IT_JUMPIFNEQ,"$fun$substr$error","LF@$type$param1","string@string");
  appendInstruction(iStruct,IT_JUMPIFNEQ,"$fun$substr$error","LF@$type$param2","string@int");
  appendInstruction(iStruct,IT_JUMPIFNEQ,"$fun$substr$error","LF@$type$param3","string@int");

  appendInstruction(iStruct,IT_JUMP,"$fun$substr$next$check",NULL,NULL);
  appendInstruction(iStruct,IT_LABEL,"$fun$substr$error",NULL,NULL);

  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$substr$floattoint$first","LF@$type$param2","string@float");
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$substr$floattoint$second","LF@$type$param3","string@float");
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$substr$next$check","LF@$type$param3","string@int");

  appendInstruction(iStruct,IT_EXIT,"int@4",NULL,NULL);

  appendInstruction(iStruct,IT_LABEL,"$fun$substr$floattoint$first",NULL,NULL);
  appendInstruction(iStruct,IT_FLOAT2INT,"LF@param2","LF@param2",NULL);
  appendInstruction(iStruct,IT_JUMP,"$fun$substr$again$check",NULL,NULL);

  appendInstruction(iStruct,IT_LABEL,"$fun$substr$floattoint$second",NULL,NULL);
  appendInstruction(iStruct,IT_FLOAT2INT,"LF@param3","LF@param3",NULL);





  appendInstruction(iStruct,IT_LABEL,"$fun$substr$next$check",NULL,NULL);

  appendInstruction(iStruct,IT_DEFVAR,"LF@$fun$substr$tmp0",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@$fun$substr$tmp1",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@$fun$substr$tmp2",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@$fun$substr$tmp3",NULL,NULL);
  appendInstruction(iStruct,IT_STRLEN,"LF@$fun$substr$tmp2","LF@param1",NULL);
  appendInstruction(iStruct,IT_LT,"LF@$fun$substr$tmp0","LF@param2","int@0");
  appendInstruction(iStruct,IT_GT,"LF@$fun$substr$tmp1","LF@param2","LF@$fun$substr$tmp2");
  appendInstruction(iStruct,IT_OR,"LF@$fun$substr$tmp3","LF@$fun$substr$tmp0","LF@$fun$substr$tmp1");
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$substr$return$nil","LF@$fun$substr$tmp3","bool@true");
  appendInstruction(iStruct,IT_ADD,"LF@param3","LF@param2","LF@param3");
  appendInstruction(iStruct,IT_GT,"LF@$fun$substr$tmp0","LF@param3","LF@$fun$substr$tmp2");
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$substr$loop","LF@$fun$substr$tmp0","bool@false");
  appendInstruction(iStruct,IT_MOVE,"LF@param3","LF@$fun$substr$tmp2",NULL);
  appendInstruction(iStruct,IT_LABEL,"$fun$substr$loop",NULL,NULL);
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$substr$return","LF@param2","LF@param3");
  appendInstruction(iStruct,IT_GETCHAR,"LF@$fun$substr$tmp0","LF@param1","LF@param2");
  appendInstruction(iStruct,IT_CONCAT,"LF@%retval","LF@%retval","LF@$fun$substr$tmp0");
  appendInstruction(iStruct,IT_ADD,"LF@param2","LF@param2","int@1");
  appendInstruction(iStruct,IT_JUMP,"$fun$substr$loop",NULL,NULL);
  appendInstruction(iStruct,IT_LABEL,"$fun$substr$return$nil",NULL,NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@%retval","nil@nil",NULL);
  appendInstruction(iStruct,IT_LABEL,"$fun$substr$return",NULL,NULL);
  appendInstruction(iStruct,IT_POPFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_RETURN,NULL,NULL,NULL);

  // ORD

  appendInstruction(iStruct,IT_LABEL,"$fun$ord",NULL,NULL);
  appendInstruction(iStruct,IT_PUSHFRAME,NULL,NULL,NULL);

  appendInstruction(iStruct,IT_DEFVAR,"LF@%retval",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@param1",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@param2",NULL,NULL);

  appendInstruction(iStruct,IT_MOVE,"LF@%retval","nil@nil",NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@param1","LF@%1",NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@param2","LF@%2",NULL);

  appendInstruction(iStruct,IT_DEFVAR,"LF@$type$param1",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@$type$param2",NULL,NULL);

  appendInstruction(iStruct,IT_TYPE,"LF@$type$param1","LF@param1",NULL);
  appendInstruction(iStruct,IT_TYPE,"LF@$type$param2","LF@param2",NULL);

  appendInstruction(iStruct,IT_JUMPIFNEQ,"$fun$ord$error","LF@$type$param1","string@string");
  appendInstruction(iStruct,IT_JUMPIFNEQ,"$fun$ord$error","LF@$type$param2","string@int");

  appendInstruction(iStruct,IT_JUMP,"$fun$ord$next$check",NULL,NULL);

  appendInstruction(iStruct,IT_LABEL,"$fun$ord$error",NULL,NULL);
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$ord$floattoint","LF@$type$param2","string@float");
  appendInstruction(iStruct,IT_EXIT,"int@4",NULL,NULL);

  appendInstruction(iStruct,IT_LABEL,"$fun$ord$floattoint",NULL,NULL);
  appendInstruction(iStruct,IT_FLOAT2INT,"LF@param2","LF@param2",NULL);


  appendInstruction(iStruct,IT_LABEL,"$fun$ord$next$check",NULL,NULL);

  appendInstruction(iStruct,IT_DEFVAR,"LF@$fun$ord$tmp0",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@$fun$ord$tmp1",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@$fun$ord$tmp2",NULL,NULL);

  appendInstruction(iStruct,IT_STRLEN,"LF@$fun$ord$tmp2","LF@param1",NULL);
  appendInstruction(iStruct,IT_SUB,"LF@$fun$ord$tmp2","LF@$fun$ord$tmp2","int@1");

  appendInstruction(iStruct,IT_LT,"LF@$fun$ord$tmp0","LF@param2","int@0");
  appendInstruction(iStruct,IT_GT,"LF@$fun$ord$tmp1","LF@param2","LF@$fun$ord$tmp2");
  appendInstruction(iStruct,IT_OR,"LF@$fun$ord$tmp0","LF@$fun$ord$tmp0","LF@$fun$ord$tmp1");
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$ord$return$nil","LF@$fun$ord$tmp0","bool@true");

  appendInstruction(iStruct,IT_GETCHAR,"LF@%retval","LF@param1","LF@param2");
  appendInstruction(iStruct,IT_STRI2INT,"LF@%retval","LF@%retval","int@0");
  appendInstruction(iStruct,IT_LABEL,"$fun$ord$return$nil",NULL,NULL);
  appendInstruction(iStruct,IT_POPFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_RETURN,NULL,NULL,NULL);

  // CHR

  appendInstruction(iStruct,IT_LABEL,"$fun$chr",NULL,NULL);
  appendInstruction(iStruct,IT_PUSHFRAME,NULL,NULL,NULL);

  appendInstruction(iStruct,IT_DEFVAR,"LF@%retval",NULL,NULL);
  appendInstruction(iStruct,IT_DEFVAR,"LF@param1",NULL,NULL);

  appendInstruction(iStruct,IT_MOVE,"LF@%retval","nil@nil",NULL);
  appendInstruction(iStruct,IT_MOVE,"LF@param1","LF@%1",NULL);

  appendInstruction(iStruct,IT_DEFVAR,"LF@$type$param1",NULL,NULL);
  appendInstruction(iStruct,IT_TYPE,"LF@$type$param1","LF@param1",NULL);

  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$chr$continue","LF@$type$param1","string@int");
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$chr$floattoint$continue","LF@$type$param1","string@float");
  appendInstruction(iStruct,IT_EXIT,"int@4",NULL,NULL);

  appendInstruction(iStruct,IT_LABEL,"$fun$chr$floattoint$continue",NULL,NULL);
  appendInstruction(iStruct,IT_FLOAT2INT,"LF@param1","LF@param1",NULL);

  appendInstruction(iStruct,IT_LABEL,"$fun$chr$continue",NULL,NULL);

  appendInstruction(iStruct,IT_DEFVAR,"LF@$fun$chr$tmp$0",NULL,NULL);
  appendInstruction(iStruct,IT_GT,"LF@$fun$chr$tmp$0","LF@param1","int@-1"); // i > 0
  appendInstruction(iStruct,IT_LT,"LF@$type$param1","LF@param1","int@256"); // i < 256
  appendInstruction(iStruct,IT_AND,"LF@$fun$chr$tmp$0","LF@$fun$chr$tmp$0","LF@$type$param1");  // <0,255>
  appendInstruction(iStruct,IT_JUMPIFEQ,"$fun$chr$continue$continue","LF@$fun$chr$tmp$0","bool@true");

  appendInstruction(iStruct,IT_EXIT,"int@4",NULL,NULL);

  appendInstruction(iStruct,IT_LABEL,"$fun$chr$continue$continue",NULL,NULL);

  appendInstruction(iStruct,IT_INT2CHAR,"LF@%retval","LF@param1",NULL);

  appendInstruction(iStruct,IT_POPFRAME,NULL,NULL,NULL);
  appendInstruction(iStruct,IT_RETURN,NULL,NULL,NULL);

  // obskok

  appendInstruction(iStruct,IT_LABEL,"$$main",NULL,NULL);
}

int generateFunctionCall(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct TokenStack *tokenStack,int function_name_index)
{
  TokenStackItem *item = NULL;
  TokenStackItem *fun_item = NULL;

  int var_counter = 1;
  char temp_var_name[200];
  int errCode = 0;

  String *tmp_str;
  String *ret_str;
  initString(&tmp_str);
  initString(&ret_str);

  // Ak je nazov funckie nie je na 0
  if (function_name_index > 0)
  {
    // Ak sme nenasli nic chyba
    if ((item = peekStack(tokenStack,0)) == NULL)
    {
      // Vratime chybu
      freeString(ret_str);
      freeString(tmp_str);
      return INTERNAL_COMP_ERR;
    }

    // Vytvorime ju ak neexistuje ak existuje nic nerobi
    if ((errCode = createVarDef(item->str->str)) != 0)
    {
      // Uvolnime a ukoncime
      freeString(ret_str);
      freeString(tmp_str);
      return errCode;
    }

    // po tom ako ju vygenerujem transformneme ju do assembleru
    changeToAssembly(TT_IDENTIFIKATOR,item->str->str,ret_str);
  }

  // Ziskame funkciu
  if ((fun_item = peekStack(tokenStack,function_name_index)) == NULL)
  {
    //
    freeString(tmp_str);
    freeString(ret_str);
    return INTERNAL_COMP_ERR;
  }

  // Vytvorime frame
  appendInstruction(iStruct,IT_CREATEFRAME,NULL,NULL,NULL);

  // Skontrolujeme kolko premennych je potrebnych
  for (int i = (function_name_index + 1); i < (sizeStack(tokenStack)); i++)
  {
    // Zacneme ziskavat itemy
    if ((item = peekStack(tokenStack,i)) == NULL)
    {
      // Ak je chyba tak
      freeString(tmp_str);
      freeString(ret_str);
      return errCode;
    }

    // Ak input do funkcie premenna
    if (item->type == TT_IDENTIFIKATOR)
    {
      // Urobime semanticku kontrolu premennej
      if ((errCode = semCheckVar(hashTable,item->str->str)) != 0)
      {
        // Ak vrati chybu koncime
        freeString(ret_str);
        freeString(tmp_str);
        return errCode;
      }
    }
    // Ziskany item prevedieme do assembleru
    if ((errCode = changeToAssembly(item->type,item->str->str,tmp_str)) != 0)
    {
      // AK nastala chyba vraciame error
      freeString(ret_str);
      freeString(tmp_str);
      return errCode;
    }

    // Zmenime var_count na string
    sprintf(temp_var_name,"TF@%%%d",var_counter);

    // Zacneme generovat temporaly variable
    appendInstruction(iStruct,IT_DEFVAR,temp_var_name,NULL,NULL);
    appendInstruction(iStruct,IT_MOVE,temp_var_name,tmp_str->str,NULL);

    var_counter++;
  }

  // Pridame funkciu do function call
  appendFunctionCall(fun_item->str->str,(var_counter - 1));

  // Vygenerujeme nazov funkcie
  if ((errCode = changeToAssembly(TT_FUN_RESERVED,fun_item->str->str,tmp_str)) != 0)
  {
    // AK nastala chyba vraciame error
    freeString(tmp_str);
    freeString(ret_str);
    return errCode;
  }

  // Callneme funkciu
  appendInstruction(iStruct,IT_CALL,tmp_str->str,NULL,NULL);


  // Ak je dlzka stringu vacsia nez 0 treba vygenerovat move
  if (stringLength(ret_str) > 0)
  {
    // Generujeme move
    appendInstruction(iStruct,IT_MOVE,ret_str->str,"TF@%retval",NULL);

    // Vtedy inicializujeme premennu
    inicializeVar(hashTable,ret_str->str);
  }

  // Vytvorime retval ak sme nahodou vo funkcii
  createRetval("TF@%retval",TT_FUN_RETVAL);

  // Uvolnime a ukoncime
  freeString(tmp_str);
  freeString(ret_str);
  return 0;
}

int generateFunctionCallPrint(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct TokenStack *tokenStack,int function_name_index)
{
  TokenStackItem *item = NULL;
  TokenStackItem *fun_item = NULL;
  int errCode = 0;
  String *tmp_str;
  String *ret_str;
  initString(&tmp_str);
  initString(&ret_str);

  // Ak je nazov funckie nie je na 0
  if (function_name_index > 0)
  {
    // Ak sme nenasli nic chyba
    if ((item = peekStack(tokenStack,0)) == NULL)
    {
      // Vratime chybu
      freeString(ret_str);
      freeString(tmp_str);
      return INTERNAL_COMP_ERR;
    }

    // Vytvorime ju ak neexistuje ak existuje nic nerobi
    if ((errCode = createVarDef(tmp_str->str)) != 0)
    {
      // Uvolnime a ukoncime
      freeString(ret_str);
      freeString(tmp_str);
      return errCode;
    }

    // po tom ako ju vygenerujem transformneme ju do assembleru
    changeToAssembly(TT_IDENTIFIKATOR,item->str->str,ret_str);
  }

  // Ak sme nenasli nic chyba
  if ((fun_item = peekStack(tokenStack,function_name_index)) == NULL)
  {
    freeString(tmp_str);
    freeString(ret_str);
    return INTERNAL_COMP_ERR;
  }

  // Ak sa jedna o funkciu print je to specialne volanie
  if (strcmp(fun_item->str->str,"print") == 0)
  {
    int var_number = 0;

    // Prejdeme vsetky premenne
    for (int i = (sizeStack(tokenStack) - 1); i > function_name_index; i--)
    {
      // Ziskame item
      if ((item = peekStack(tokenStack,i)) == NULL)
      {
        freeString(ret_str);
        freeString(tmp_str);
        return INTERNAL_COMP_ERR;
      }

      // Ak input do funkcie premenna
      if (item->type == TT_IDENTIFIKATOR)
      {
        // Urobime semanticku kontrolu premennej
        if ((errCode = semCheckVar(hashTable,item->str->str)) != 0)
        {
          // Ak vrati chybu koncime
          freeString(ret_str);
          freeString(tmp_str);
          return errCode;
        }
      }


      // Ziskany item prevedieme do assembleru
      if ((errCode = changeToAssembly(item->type,item->str->str,tmp_str)) != 0)
      {
        // AK nastala chyba vraciame error
        freeString(ret_str);
        freeString(tmp_str);
        return errCode;
      }

      // Zvysime counter premennych
      var_number++;

      // Kazdu premennu napushujeme do stacku
      appendInstruction(iStruct,IT_PUSHS,tmp_str->str,NULL,NULL);
    }

    // Vytvorime frame
    appendInstruction(iStruct,IT_CREATEFRAME,NULL,NULL,NULL);
    appendInstruction(iStruct,IT_DEFVAR,"TF@%1",NULL,NULL);

    char tmp[200];

    sprintf(tmp,"%d",var_number);

    // Prepiseme integer do assembleru
    if ((errCode = changeToAssembly(TT_INTEGER,tmp,tmp_str)) != 0)
    {
      // AK nastala chyba vraciame error
      freeString(tmp_str);
      freeString(ret_str);
      return errCode;
    }

    // Movneme to prveho parametru pocet premennych v stacku
    appendInstruction(iStruct,IT_MOVE,"TF@%1",tmp_str->str,NULL);

    // Vygenerujeme nazov funkcie
    if ((errCode = changeToAssembly(TT_FUN_RESERVED,fun_item->str->str,tmp_str)) != 0)
    {
      // AK nastala chyba vraciame error
      freeString(tmp_str);
      freeString(ret_str);
      return errCode;
    }

    // Callneme funkciu
    appendInstruction(iStruct,IT_CALL,tmp_str->str,NULL,NULL);

    // Ak je dlzka stringu vacsia nez 0 treba vygenerovat move
    if (stringLength(ret_str) > 0)
    {
      // Generujeme move
      appendInstruction(iStruct,IT_MOVE,ret_str->str,"TF@%retval",NULL);
    }

    // Uvolnime a ukoncime
    freeString(ret_str);
    freeString(tmp_str);
    return 0;
  }
  // Ak to neni print funkcia je zle volanie nas error
  else
  {
    // Koncie a vratime error
    freeString(ret_str);
    freeString(tmp_str);
    return INTERNAL_COMP_ERR;
  }
}

int generateFunctionDefinition(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct TokenStack *tokenStack)
{
  String *tmp_str;
  initString(&tmp_str);

  TokenStackItem *item = NULL;

  // Ziskame nazov funkcie
  if ((item = peekStack(tokenStack,0)) == NULL)
  {
    // Ak nastala chyba uvolnime a vratime
    freeString(tmp_str);
    return INTERNAL_COMP_ERR;
  }

  // Vygenerujeme nazov labelu pre skok
  appendString("$fun$",tmp_str);

  // Appendneme nazov funkcie
  appendString(item->str->str,tmp_str);

  // Vygenerujeme LABEL pre skok
  appendInstruction(iStruct,IT_LABEL,tmp_str->str,NULL,NULL);

  // Appendneme push
  appendInstruction(iStruct,IT_PUSHFRAME,NULL,NULL,NULL);

  // Definujeme retval
  appendInstruction(iStruct,IT_DEFVAR,"LF@%retval",NULL,NULL);

  // Inicializujeme ju na NIL
  appendInstruction(iStruct,IT_MOVE,"LF@%retval","nil@nil",NULL);

  char var_count[200];

  // Prejdeme vsetky premenne a vytvorime ich a pridame ich do hashtablu
  for (int i = 1; i < sizeStack(tokenStack); i++)
  {
    // Ziskame item
    item = peekStack(tokenStack,i);

    // Ak je item null je to chyba
    if (item == NULL)
    {
      // Uvolnime a vratime
      freeString(tmp_str);
      return INTERNAL_COMP_ERR;
    }

    // Zmenime do assembleru
    changeToAssembly(TT_IDENTIFIKATOR,item->str->str,tmp_str);

    // Vytvorime definiciu
    appendInstruction(iStruct,IT_DEFVAR,tmp_str->str,NULL,NULL);

    // Vytvorime MOVE
    sprintf(var_count,"LF@%%%d",i);

    // Vytvorime MOVE
    appendInstruction(iStruct,IT_MOVE,tmp_str->str,var_count,NULL);

    // Inicializujeme variable
    inicializeVar(hashTable,tmp_str->str);
  }

  // Uvolnime a ukoncime OK
  freeString(tmp_str);
  return 0;
}

// Vygenerujeme tmp variable ktoru vratime v stringu
int generateTmpVar(struct HashTable *hTable,String *str,int *counter)
{
  int errCode = 0;
  // Vycistime str
  clearString(str);

  char tmp_str[200];

  // Dame int na string
  sprintf(tmp_str,"$tmp%d",*counter);


  // Vytvorime
  if ((errCode = createVarDef(tmp_str)) != 0)
  {
    return errCode;
  }


  // Vlozime do hashtable
  HashTableInsert(hTable,tmp_str,TT_IDENTIFIKATOR,NULL);


  // Inicializujeme ju
  inicializeVar(hTable,tmp_str);

  // Appendneme do stringu
  appendString("LF@",str);
  appendString(tmp_str,str);

  (*counter)++;

  return errCode;
}

int generateCode(struct InstructionStruct *iStruct)
{
  // Ak je list instrukcii prazdny negenerujeme nic
  if (iStruct->First == NULL)
  {
    return 0;
  }
  // Vygenerujeme header filu .IFJcode18
  printf("%s\n", FILE_HEADER);
  printf("CREATEFRAME\n");
  printf("PUSHFRAME\n");

  // Vygenerujeme vstavane funkcie
  generateReservedFunctionCall(iStruct);

  // Posunieme sa na zaciatok instruction listu
  iStruct->Active = iStruct->First;

  // Zacneme prechadzat pokial nenarazime na koniec
  while (iStruct->Active != NULL)
  {
    printf("%s", Instruction_Word[iStruct->Active->type]);
    if (iStruct->Active->ptr1 != NULL)
    {
      printf("\t%s", iStruct->Active->ptr1->str);
    }
    if (iStruct->Active->ptr2 != NULL)
    {
      printf("\t%s", iStruct->Active->ptr2->str);
    }
    if (iStruct->Active->ptr3 != NULL)
    {
      printf("\t%s", iStruct->Active->ptr3->str);
    }
    printf("\n");
    iStruct->Active = iStruct->Active->next;
  }
  // Popneme na konci frame
  printf("POPFRAME\n");

  // Vratime 0
  return 0;
}

int changeToAssembly(TokenType type,char *str,String *tmp_str)
{
  // Ak to je String konvertneme na STRING
  if (type == TT_STRING)
  {
    convertString(str,tmp_str);
  }
  // AK to je INT konvertneme na INT
  else if (type == TT_INTEGER)
  {
    convertInt(str,tmp_str);
  }
  // Ak to je FLOAT alebo EXPONENT konvertneme na FLOAT
  else if (type == TT_FLOAT || type == TT_EXPONENT)
  {
    convertFloat(str,tmp_str);
  }
  // Ak je to VARIABLE
  else if (type == TT_IDENTIFIKATOR)
  {
    // Tak skontrolujeme ci existuje
    returnVar(str,tmp_str);
  }
  // Alebo hodnota nil
  else if (type == TT_NIL)
  {
    convertNil(tmp_str);
  }
  // AK je to volanie funkcie
  else if (type == TT_FUN_RESERVED || type == TT_FUN_IDENTIFIKATOR)
  {
    convertFun(str,tmp_str);
  }
  else
  {
    return SEMANTIC_PROG_ERR;
  }
  return 0;
}
