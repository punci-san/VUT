/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Jaromír Hradil <xhradi15@stud.fit.vutbr.cz>
 *
 **/

#ifndef STACK_HEADER
#define STACK_HEADER

#include "scanner.h"
#include "str.h"
///Identifikator derivacniho stromu
#define DERIVATIVE_TREE 200
///Prvek ukladany na stack
typedef struct TokenStackItem
{
  TokenType type;
  String *str;
  void *ptr;
}TokenStackItem;
///Struktura stacku
typedef struct TokenStack
{
  int top;
  TokenStackItem **array; ///Pole prvku zasobniku
  int capacity;
}TokenStack;

struct TokenStack* createStack();

int sizeStack(struct TokenStack* stack);
struct TokenStackItem* peekStack(struct TokenStack* stack,int index);

void copyStack(struct TokenStack* fromStack,struct TokenStack* toStack);

void pushStack(struct TokenStack* stack,int type,void *ptr,char *key);
void popStack(struct TokenStack* stack);
void popIndex(struct TokenStack* stack,int index);

void clearStack(TokenStack *stack);
void freeStack(TokenStack *stack);

#endif
