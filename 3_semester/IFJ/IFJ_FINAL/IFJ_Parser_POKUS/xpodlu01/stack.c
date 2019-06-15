/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Jaromír Hradil <xhradi15@stud.fit.vutbr.cz>
 *
 **/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stack.h"

#define INITIAL_CAPACITY 50
///Vrati, jestli je plny
int isFull(struct TokenStack* stack)
{
  return stack->top == stack->capacity -1;
}
///Vrati, jestli je prazdny
int isEmpty(struct TokenStack *stack)
{
  return stack->top == -1;
}
///Vrati velikost stacku
int sizeStack(struct TokenStack *stack)
{
  return stack->top+1;
}
///Vrati ukazatel na polozku v poli prvku na stacku
struct TokenStackItem* peekStack(struct TokenStack* stack,int index)
{
  if (sizeStack(stack) > index)
  {
    return stack->array[index];
  }
  return NULL;
}
///Vytvori stack
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
  // Zdvojnasobi velikost stacku
  stack->capacity *= 2;
  stack->array = realloc(stack->array,stack->capacity * sizeof(struct TokenStackItem *));
}

void copyStack(struct TokenStack* fromStack,struct TokenStack* toStack)
{
  // Clearneme stack do ktereho kopirujeme
  clearStack(toStack);

  // Vytvorime item pointer
  struct TokenStackItem *item = NULL;

  // Projdeme vsetky itemy ve fromStack a nakopirujeme je do toStack
  for (int i = 0; i < sizeStack(fromStack); i++)
  {
    // Ziskame polozku z fromStacku
    item = peekStack(fromStack,i);

    // Zkopirujeme ji do toStacku
    pushStack(toStack,item->type,item->ptr,item->str->str);
  }
}
///Vkladani prvku do pole prvku na stacku
void pushStack(struct TokenStack* stack,int type,void *ptr,char *key)
{

 /// Jestli je plny, zvetsi se
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
  // Pokud je stack prazdny, nic nedelame
  if (isEmpty(stack))
  {
    return;
  }
  // Pokud je index na odstraneni vetsi nebo roven aktualni velikosti stacku, nic nedelame
  if (sizeStack(stack) <= index)
  {
    return;
  }

  // Jinak smazeme na indexu polozku a posuneme vsechny ostatni o jedno do leva
  freeString(stack->array[index]->str);
  free(stack->array[index]);
  stack->array[index] = NULL;

  // Posuneme vsetky polozky za indexem o 1
  for (int i = index; i < (sizeStack(stack) - 1); i++)
  {
    stack->array[i] = stack->array[i + 1];
  }

  // Snizime pocet
  stack->top--;
}

void popStack(struct TokenStack* stack)
{
  if (isEmpty(stack))
  {
    return;
  }

  // Uvolnime nejsvrchnejsi polozku
  if (stack->array[stack->top]->str != NULL)
  {
    freeString(stack->array[stack->top]->str);
  }
  free(stack->array[stack->top]);

  stack->top--;
}
///Uvolnovani zdroju
void clearStack(struct TokenStack *stack)
{
  while (!isEmpty(stack))
  {
    popStack(stack);
  }
}
///Uvolnovani zdroju a celeho stacku
void freeStack(struct TokenStack *stack)
{
  clearStack(stack);
  free(stack->array);
  free(stack);
}
