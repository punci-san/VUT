#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stack.h"

#define INITIAL_CAPACITY 50

int isFull(struct TokenStack* stack)
{
  return stack->top == stack->capacity -1;
}

int isEmpty(struct TokenStack *stack)
{
  return stack->top == -1;
}

int sizeStack(struct TokenStack *stack)
{
  return stack->top+1;
}

struct TokenStackItem* peekStack(struct TokenStack* stack,int index)
{
  if (sizeStack(stack) > index)
  {
    return stack->array[index];
  }
  return NULL;
}

struct TokenStack* createStack()
{
  struct TokenStack* stack = (struct TokenStack *)malloc(sizeof(struct TokenStack));
  stack->capacity = INITIAL_CAPACITY;
  stack->top = -1;
  stack->array = malloc(stack->capacity * sizeof(struct TokenStackItem *));
  return stack;
}

void resizeStack(struct TokenStack *stack)
{
  // Zdvojnasob velkost stacku
  stack->capacity *= 2;
  stack->array = realloc(stack->array,stack->capacity * sizeof(struct TokenStackItem *));
}

void copyStack(struct TokenStack* fromStack,struct TokenStack* toStack)
{
  // Clearneme stack do ktoreho kopirujeme
  clearStack(toStack);

  // Vytvorime item pointer
  struct TokenStackItem *item = NULL;

  // Prejdeme vsetky itemy v fromStack a nakopirujeme ich do to stack
  for (int i = 0; i < sizeStack(fromStack); i++)
  {
    // Ziskame item z fromStacku
    item = peekStack(fromStack,i);

    // Skopirujeme ho do toStacku
    pushStack(toStack,item->type,item->ptr,item->str->str);
  }
}

void pushStack(struct TokenStack* stack,int type,void *ptr,char *key)
{
  if (isFull(stack))
  {
    resizeStack(stack);
  }

  struct TokenStackItem *item = (struct TokenStackItem *)malloc(sizeof(struct TokenStackItem));

  item->type = type;
  item->ptr = ptr;
  item->str = NULL;

  if (key != NULL)
  {
    String *tmp_str;
    initString(&tmp_str);
    copyString(key,tmp_str);
    item->str = tmp_str;
  }

  stack->array[++stack->top] = item;
}

void popIndex(struct TokenStack* stack,int index)
{
  // Ak je stack empty nerobime nic
  if (isEmpty(stack))
  {
    return;
  }
  // Ak je index na zmazanie vacsi alebo rovny nez velkost tiez nic nerobime
  if (sizeStack(stack) <= index)
  {
    return;
  }

  // Inac zmazeme na indexe item a posunieme vsetky itemy o jedno do lava
  freeString(stack->array[index]->str);
  free(stack->array[index]);
  stack->array[index] = NULL;

  // Posunieme vsetky polozky za indexom o 1
  for (int i = index; i < (sizeStack(stack) - 1); i++)
  {
    stack->array[i] = stack->array[i + 1];
  }

  // Znizime pocet
  stack->top--;
}

void popStack(struct TokenStack* stack)
{
  if (isEmpty(stack))
  {
    return;
  }

  // Uvolnime posledny item
  if (stack->array[stack->top]->str != NULL)
  {
    freeString(stack->array[stack->top]->str);
  }
  free(stack->array[stack->top]);

  stack->top--;
}

void clearStack(struct TokenStack *stack)
{
  while (!isEmpty(stack))
  {
    popStack(stack);
  }
}

void freeStack(struct TokenStack *stack)
{
  clearStack(stack);
  free(stack->array);
  free(stack);
}
