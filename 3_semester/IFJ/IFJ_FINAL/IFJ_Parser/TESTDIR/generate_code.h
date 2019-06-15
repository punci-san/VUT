/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
 *
 **/
 
#ifndef FILE_GENERATECODE
#define FILE_GENERATECODE


#include "bin_struct.h"
#include "stack.h"
#include "symtable.h"
#include "parser.h"

#define GF 0
#define LF 1

/*
  Type          PTR1    PTR2    PTR3
  DEFVAR        GF/LF   name    NULL
  MOVE          TO      FROM    NULL

  CREATEFRAME   NULL    NULL    NULL
  PUSHFRAME     NULL    NULL    NULL
  POPFRAME      NULL    NULL    NULL

  CALL          LABEL   NULL    NULL
  RETURN        NULL    NULL    NULL

  PUSHS         VAR/CONST NULL  NULL
  POPS          VAR     NULL    NULL
  CLEARS        NULL    NULL    NULL

  ADD           TO    VAR/SYMB  VAR/SYMB
  SUB           TO    VAR/SYMB  VAR/SYMB
  MUL           TO    VAR/SYMB  VAR/SYMB
  DIV           TO    VAR/SYMB  VAR/SYMB
  IDIV          TO    VAR/SYMB  VAR/SYMB

  LT            VAR   VAR/SYMB  VAR/SYMB
  GT            VAR   VAR/SYMB  VAR/SYMB
  EQ            VAR   VAR/SYMB  VAR/SYMB

  AND           VAR   VAR/SYMB  VAR/SYMB
  OR            VAR   VAR/SYMB  VAR/SYMB
  NOT           VAR   VAR/SYMB  VAR/SYMB

  INT2FLOAT     VAR   SYMB      NULL
  FLOAT2INT     VAR   SYMB      NULL

  INT2CHAR      VAR   SYMB      NULL
  STRI2INT      VAR   SYMB      SYMB

  READ          VAR   TYPE      NULL
  WRITE         SYMB/VAR

  CONCAT        VAR   SYMB/VAR  SYMB/VAR
  STRLEN
*/
typedef enum Instruction_Type
{
  /*
    DEFVAR => Vytvori variable

  */
  IT_DEFVAR,
  IT_MOVE,
  /**/
  IT_CREATEFRAME,
  IT_PUSHFRAME,
  IT_POPFRAME,
  IT_CALL,
  IT_RETURN,
  /**/
  IT_PUSHS,
  IT_POPS,
  IT_CLEARS,
  /**/
  IT_ADD,
  IT_SUB,
  IT_MUL,
  IT_DIV,
  IT_IDIV,
  /**/
  IT_ADDS,
  IT_SUBS,
  IT_MULS,
  IT_DIVS,
  IT_IDIVS,
  /**/
  IT_LT,
  IT_GT,
  IT_EQ,
  /**/
  IT_LTS,
  IT_GTS,
  IT_EQS,
  /**/
  IT_AND,
  IT_OR,
  IT_NOT,
  /**/
  IT_ANDS,
  IT_ORS,
  IT_NOTS,
  /**/
  IT_INT2FLOAT,
  IT_FLOAT2INT,
  IT_INT2CHAR,
  IT_STRI2INT,
  /**/
  IT_INT2FLOATS,
  IT_FLOAT2INTS,
  IT_INT2CHARS,
  IT_STRI2INTS,
  /**/
  IT_READ,
  IT_WRITE,
  /**/
  IT_CONCAT,
  IT_STRLEN,
  IT_GETCHAR,
  IT_SETCHAR,
  /**/
  IT_TYPE,
  /**/
  IT_LABEL,
  /**/
  IT_JUMP,
  IT_JUMPIFEQ,
  IT_JUMPIFNEQ,
  /**/
  IT_JUMPS,
  IT_JUMPIFEQS,
  IT_JUMPIFNEQS,
  /**/
  IT_EXIT,
  IT_BREAK,
  IT_DPRINT
}Instruction_Type;

typedef struct InstructionItem
{
  struct InstructionItem *prev;
  Instruction_Type type;
  String *ptr1;
  String *ptr2;
  String *ptr3;
  struct InstructionItem *next;
}InstructionItem;

typedef struct InstructionStruct
{
  struct InstructionItem *First;
  struct InstructionItem *Active;
  struct InstructionItem *Last;
}InstructionStruct;

/*
  TODO Treba sem pridat TokenStack kde:
  str =>  bude reprezentovat label
  type => bude reprezentovat typ labelu IF,WHILE,FUNCTION,programova ktoru pouzivame na obskakovanie definici funkcii

  Na tento stack sa bude neustale pushovat vtedy ked bude IF,WHILE,Definicia funkcie,

  A vzdy sa popne pri najdeni end

  Ak dostaneme pokyn na vygenerovanie EXIT tak skontrolujeme ci je stack prazdny ak nie jedna sa o SYNTAX ERROR
*/

struct InstructionStruct *createInstructionStruct();

void prependInstruction(struct InstructionStruct *iStruct,struct InstructionItem *iItem,Instruction_Type type,char *ptr1,char *ptr2,char *ptr3);
void appendInstruction(struct InstructionStruct *iStruct,Instruction_Type type,char *ptr1,char *ptr2,char *ptr3);

void freeInstructionStruct(struct InstructionStruct *iStruct);

int generateExpression(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct binStruct *binStruct,String *ret_str,int *tmp_var_counter);
int generateFunctionCall(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct TokenStack *tokenStack,int function_name_index);
int generateFunctionCallPrint(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct TokenStack *tokenStack,int function_name_index);
int generateFunctionDefinition(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct TokenStack *tokenStack);


int generateIf(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct binStruct *binStruct,int if_index,int *tmp_var_counter);
void generateElse(struct InstructionStruct *iStruct,int if_index);

int generateWhile(struct InstructionStruct *iStruct,struct HashTable *hashTable,struct binStruct *binStruct,int if_index,int *tmp_var_counter);


int generateTmpVar(struct HashTable *hTable,String *str,int *counter);
int changeToAssembly(TokenType type,char *str,String *tmp_str);


int generateCode(struct InstructionStruct *iStruct);

#endif
