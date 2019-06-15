/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
 *
 **/

#ifndef SYMTABLE_HEADER
#define SYMTABLE_HEADER

#include "str.h"

#define HS_INIT_SIZE 101

#define OK 0
#define ERR -1

#define ST_STRING 1
#define ST_INT 2
#define ST_FLOAT 3
#define ST_NIL 4
#define ST_FUN 5
/*
  Typy budu urcovat ktory z union hodnot su pouzite

  Ak bude typ ST_FUN je to typ funkcie kde val bude obsahovat pocet parametrov
*/
typedef struct Data
{
  int tokenType;  // Typ ci je to variable alebo funkcia
  // 0 => este nebola
  // 1 => uz bola a u funckii inicializacii znamena ze uz sme nasli jej end
  int inicialization;   // Bola alebo nebola este inicializovana var/fun
  int var_number;       // Bude signalizovat u funkcii pocet premennych
  void *ptr;            // Bude obsahovat pointer na LocalFrame
}Data;

typedef struct Item
{
  struct String *key;   // Nazov variable alebo funkcie
  struct Data data;
}Item;

typedef struct HashTable
{
  int size;
  int item_count;
  struct Item **items;
}HashTable;

#define PRIME_1 3
#define PRIME_2 13

struct HashTable* initHashTable(int size);

// Operacie z tabulkou
int HashTableInsert(struct HashTable *table,char *key,int type,void *ptr);
struct Item* HashTableSearch(struct HashTable *table,char *key);
int HashTableDelete(struct HashTable *table,char *key);

void freeHashTable(struct HashTable *table);

#endif
