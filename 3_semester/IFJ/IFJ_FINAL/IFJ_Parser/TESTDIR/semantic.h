/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Pavel Podlužanský <xpodlu01@stud.fit.vutbr.cz>
 *
 **/

#ifndef SEMANTIC_HEADER
#define SEMANTIC_HEADER

#include "stack.h"
#include "symtable.h"

typedef enum SemanticType
{
  ST_WHILE,
  ST_IF,
  ST_DEF_FUN,
  ST_CALL_FUN,

}SemanticType;



int semCheckVar(struct HashTable *hTable,char *str);

void convertString(char *str,String *tmp_str);
void convertNil(String *tmp_str);
void convertInt(char *str,String *tmp_str);
void convertFun(char *str,String *tmp_str);
void convertFloat(char *str,String *tmp_str);

int inicializeVar(struct HashTable *hTable,char *var);
int inicializeFun(struct HashTable *hTable,char *var,int number);

void returnVar(char *var,String *str);
void returnVarName(char *var,String *str);


#endif
