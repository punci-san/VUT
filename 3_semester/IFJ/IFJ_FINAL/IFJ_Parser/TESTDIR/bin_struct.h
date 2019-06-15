/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
 *
 **/

#ifndef BINSTRUCT_HEADER
#define BINSTRUCT_HEADER

#include "str.h"
#include "scanner.h"

typedef enum NodeType
{
  NT_LEFT,
  NT_RIGHT,
  NT_BOTH,
  NT_ALONE
}NodeType;

typedef struct binStruct
{
  struct binNode *Top;
  struct binStack *Stack;
}binStruct;

// Stack
typedef struct binStack
{
  int size;
  int capacity;
  void **array;
}binStack;

typedef struct binNode
{
  struct binNode *prev;
  NodeType type;
  int stackIndex;
  void *Left;
  struct binItem *Middle;
  void *Right;
}binNode;

typedef struct binItem
{
  TokenType type;
  String *str;
}binItem;


struct binStruct * createBinStruct();

struct binItem * createBinItem(int type,char *str);
int addBinNode(struct binStruct *bStruct,void *it1,void *it2,void *it3,NodeType nType);


struct binNode * getFirstNode(struct binStruct *bStruct);
int setFirstItem(struct binStruct *bStruct,int index,char *str);

void freeBinNode(struct binNode *bNode);
void freeBinItem(struct binItem *bItem);
void freeBinStruct(struct binStruct *bStruct);

// TODO Vymazat tuto funkciu len na testovanie
void printNodeInfo(struct binNode *bNode);

/*
  -----------------------------------------------------------------------------
*/

int addBinStackItem(struct binStack *bStack,struct binNode *bNode);
struct binNode * peekBinStack(struct binStack *bStack, int index);

void popBinStackItem(struct binStack *bStack,int index);

void freeBinStack();


#endif
