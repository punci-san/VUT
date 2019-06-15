/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
 *
 **/

#include "bin_struct.h"

#include <stdio.h>
#include <stdlib.h>

// Zaciatocna velkost stacku
#define INITIAL_STACK_SIZE 10


void freeBinNode(struct binNode *bNode);
struct binStack * createBinStack();


// Vytvori binStruct
struct binStruct * createBinStruct()
{
  // Allokujeme strukturu
  struct binStruct *bStruct = (struct binStruct *)malloc(sizeof(struct binStruct));

  // Inicializujeme hodnoty struktury
  bStruct->Top = NULL;
  bStruct->Stack = createBinStack();  // Vytvorime stack ktory je pripevneny na strukturu

  // Vratime strukturu
  return bStruct;
}

// Vytvori Node
struct binNode * createBinNode(struct binItem *it1,struct binItem *it2,struct binItem *it3,NodeType type)
{
  // Vytvorime Node
  struct binNode *bNode = (struct binNode *)malloc(sizeof(struct binNode));

  // Pridame prvy item
  bNode->Left = (struct binItem *)it1;

  // Pridame druhy item
  bNode->Middle = (struct binItem *)it2;

  // Pridame treti item
  bNode->Right = (struct binItem *)it3;

  // Inicializujeme pointer na predchadzajuci Node na NULL
  bNode->prev = NULL;

  // Nastavime typ
  bNode->type = type;

  // Vratime Node
  return bNode;
}

// Vytvorime binItem z typu a stringu
struct binItem * createBinItem(int type,char *str)
{
  // Allokujeme strukturu binItem
  struct binItem *bItem = (struct binItem *)malloc(sizeof(struct binItem));

  // Vytvorime string
  String *tmp_str;
  initString(&tmp_str);
  copyString(str,tmp_str);

  // Priradime itemu
  bItem->str = tmp_str;
  bItem->type = type;

  // Vratime ten item
  return bItem;
}

// Pridame binNode a ak sa nieco pokazilo
int addBinNode(struct binStruct *bStruct,void *it1,void *it2,void *it3,NodeType nType)
{
  // Node pointer
  struct binNode *bNode = NULL;

  // Ak mame pridat na obe znamena to ze vytvorime Node iba zo symbolom
  if (nType == NT_BOTH)
  {
    // Vytvorime node
    bNode = createBinNode(NULL,it2,NULL,nType);

    // Nastavime nech nody ukazuju naspat na nas
    ((struct binNode *)it1)->prev = bNode;
    ((struct binNode *)it3)->prev = bNode;


    // Pridame nody na spravne miesta
    bNode->Left = (struct binNode *)it1;
    bNode->Right = (struct binNode *)it3;
  }
  //
  else if (nType == NT_LEFT)
  {
    // Vytvorime node
    bNode = createBinNode(NULL,it2,it3,nType);

    // Nastavime ukazovatel naspat
    ((struct binNode *)it1)->prev = bNode;

    // Pridame nody na spravne miesta
    bNode->Left = (struct binNode *)it1;
  }
  else if (nType == NT_RIGHT)
  {
    // Vytvorime node
    bNode = createBinNode(it1,it2,NULL,nType);

    // Nastavime ukazovatel naspat
    ((struct binNode *)it3)->prev = bNode;

    // Pridame nody na spravne miesta
    bNode->Right = (struct binNode *)it3;
  }
  else if (nType == NT_ALONE)
  {
    // Vytvorime node
    bNode = createBinNode(it1,it2,it3,nType);
  }
  else
  {
    return -1;
  }

  // Ak nastala chyba vratime error
  if (bNode == NULL)
  {
    return -1;
  }

  // Pripojime na prev top tento item
  bStruct->Top = bNode;


  // Pridame do stacku
  return addBinStackItem(bStruct->Stack,bNode);
}

/*
  Ziskame prvy item ktory je potrebny na zacatie vypoctu
  Co znamena ze bude ten najnizsi
*/
struct binNode * getFirstNode(struct binStruct *bStruct)
{
  for (int i = 0; i < bStruct->Stack->size; i++)
  {
    if (bStruct->Stack->array[i] == NULL)
    {
      continue;
    }
    return bStruct->Stack->array[i];
  }
  return NULL;
}

int setFirstItem(struct binStruct *bStruct,int index,char *str)
{
  if (index > bStruct->Stack->size)
  {
    return -1;
  }

  // Pridame sem node na ktorom je pripevneny node ktory akuratne prepisujeme
  struct binNode *curr_node = (struct binNode *)bStruct->Stack->array[index];
  struct binNode *prev_node = (struct binNode *)curr_node->prev;

  // Vzdy to musi byt posledny node ktory je zostaveny len z itemov
  if (curr_node->type != NT_ALONE)
  {
    return -1;
  }
  // Vytvorime novy item
  struct binItem *bItem = createBinItem(TT_IDENTIFIKATOR,str);

  // Ak je to typ kde su na oboch stranach nody
  if (prev_node->type == NT_BOTH)
  {

    // Skontrolujeme obe strany nodu na ktorej strane je nodu ktory prepisujeme na item

    // Lavy node
    if (((struct binNode*)prev_node->Left)->stackIndex == index)
    {
      // uvolnime stary item
      freeBinNode(curr_node);


      // Zmenime typ Nodu na Pravy
      prev_node->type = NT_RIGHT;

      // V stacku dame pointer na NULL
      bStruct->Stack->array[index] = NULL;

      // pripevnime na pravu stranu item
      prev_node->Left = bItem;
    }
    // Pravy node
    else if (((struct binNode*)curr_node->Right)->stackIndex == index)
    {
      // uvolnime stary item
      freeBinNode(curr_node);

      // Zmenime typ Nodu na Lavy
      prev_node->type = NT_LEFT;

      // V stacku dame pointer na NULL
      bStruct->Stack->array[index] = NULL;

      // pripevnime na pravu stranu item
      prev_node->Right = bItem;
    }
    // Ak nieco ine je to chyba ktora nemela nastat
    else
    {
      return -1;
    }
  }
  // Node je iba na pravej strane
  else if (prev_node->type == NT_RIGHT)
  {
    // uvolnime stary item
    freeBinNode(curr_node);

    // Zmenime typ Nodu na Alone
    prev_node->type = NT_ALONE;

    // V stacku dame pointer na NULL
    bStruct->Stack->array[index] = NULL;

    // pripevnime na pravu stranu item
    prev_node->Right = bItem;
  }
  // Node je iba na lavej strane
  else if (prev_node->type == NT_LEFT)
  {
    // uvolnime stary item
    freeBinNode(curr_node);

    // Zmenime typ Nodu na Alone
    prev_node->type = NT_ALONE;

    // V stacku dame pointer na NULL
    bStruct->Stack->array[index] = NULL;

    // pripevnime na lavu stranu item
    prev_node->Left = bItem;
  }

  return 0;
}


void freeBinNode(struct binNode *bNode)
{
  // Ak je bNode prazdny nerobime nic
  if (bNode == NULL)
  {
    // Vrat sa
    return;
  }
  // Obsahuje len itemy
  if (bNode->type == NT_ALONE)
  {
    // Uvolnime vsetky itemy
    if (bNode->Left != NULL)
    {
      freeBinItem((struct binItem *)bNode->Left);
    }
    if (bNode->Middle != NULL)
    {
      freeBinItem((struct binItem *)bNode->Middle);
    }
    if (bNode->Right != NULL)
    {
      freeBinItem((struct binItem *)bNode->Right);
    }
  }
  // Vlavo je Node
  else if (bNode->type == NT_LEFT)
  {
    bNode->Left = NULL;

    // Uvolnime itemy
    freeBinItem((struct binItem *)bNode->Middle);
    freeBinItem((struct binItem *)bNode->Right);
  }
  // Vpravo je Node
  else if (bNode->type == NT_RIGHT)
  {
    // Uvolnime itemy
    freeBinItem((struct binItem *)bNode->Left);
    freeBinItem((struct binItem *)bNode->Middle);

    bNode->Right = NULL;
  }
  // Vlavo aj vpravo su Nody
  else if (bNode->type == NT_BOTH)
  {
    // Uvolnime lavy Node
    bNode->Left = NULL;

    // Uvolnime item
    freeBinItem((struct binItem *)bNode->Middle);

    bNode->Right = NULL;
  }
  free(bNode);
}

void freeBinItem(struct binItem *bItem)
{
  if (bItem->str != NULL)
  {
    freeString(bItem->str);
  }
  free(bItem);
}

void freeBinStruct(struct binStruct *bStruct)
{
  if (bStruct != NULL)
  {
    freeBinStack(bStruct->Stack);
    // Uvolnime strukturu
    free(bStruct);
  }
}

/*
  --------------------------------------------------------------------------------------------------
*/

struct binStack * createBinStack()
{
  struct binStack *bStack = (struct binStack *)malloc(sizeof(struct binStack));

  bStack->size = 0;
  bStack->capacity = INITIAL_STACK_SIZE;
  bStack->array = malloc(INITIAL_STACK_SIZE * sizeof(void *));

  return bStack;
}

int resizeBinStack(struct binStack *bStack)
{
  // Zdvojnasobime velkost stacku
  void ** arr = malloc(bStack->capacity * 2 * sizeof(void *));

  // Nastala chyba pri mallocu ?
  if (arr == NULL)
  {
    return -1;
  }

  // Zvacsime capacitu
  bStack->capacity *= 2;

  // Prekopirujeme stary stack na novy
  for (int i = 0; i < bStack->size; i++)
  {
    arr[i] = bStack->array[i];
  }

  // Switchneme stacky
  //free(bStack->array);
  bStack->array = realloc(bStack->array,bStack->capacity * sizeof(void *));

  // Prekopirujeme stary stack na novy
  for (int i = 0; i < bStack->size; i++)
  {
    bStack->array[i] = arr[i];
  }

  free(arr);
  // Uvolnime stary stack
  //free(arr);

  return 0;
}

int addBinStackItem(struct binStack *bStack,struct binNode *bNode)
{
  if (bStack->size >= bStack->capacity)
  {
    if(resizeBinStack(bStack) != 0)
    {
      return -1;
    }
  }
  bNode->stackIndex = bStack->size;
  bStack->array[bStack->size] = (struct binNode *)bNode;
  bStack->size++;
  return bNode->stackIndex;
}

struct binNode * peekBinStack(struct binStack *bStack, int index)
{
  return (index > bStack->size) ? NULL : bStack->array[index];
}

void popBinStackItem(struct binStack *bStack,int index)
{
  // Ak sme dostaly zly index vynechame
  if (index > bStack->size)
  {
    return;
  }

  // Uvolnime item na indexe
  freeBinNode(bStack->array[index]);

  // Nastavime item na indexe na NULL
  bStack->array[index] = NULL;
}

void freeBinStack(struct binStack *bStack)
{
  for (int i = 0; i < bStack->size; i++)
  {
    if ((struct binNode *)bStack->array[i] == NULL)
    {
      continue;
    }
    freeBinNode((struct binNode *)bStack->array[i]);
  }

  free(bStack->array);
  free(bStack);
}
