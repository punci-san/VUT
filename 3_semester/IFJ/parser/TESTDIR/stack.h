#ifndef STACK_HEADER
#define STACK_HEADER

#include "scanner.h"
#include "str.h"

#define DERIVATIVE_TREE 200

typedef struct TokenStackItem
{
  TokenType type;
  String *str;
  void *ptr;
}TokenStackItem;

typedef struct TokenStack
{
  int top;
  TokenStackItem **array;
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
