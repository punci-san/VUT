#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "str.h"
#include "symtable.h"
#include "prime.h"

void freeItem(struct Item *item);
long long hashString(char *s,double a,int m);
int getHash(char *s,const int table_size,const int atempt);


HashTable * initHashTable(int size)
{
  HashTable *table = (HashTable *)malloc(sizeof(HashTable));
  table->size = size;
  table->item_count = 0;
  table->items = calloc(table->size,sizeof(Item *));
  return table;
}

void resizeHashTable(struct HashTable *table)
{
  // Zistime novu velkost tabulky
  int table_size = nextPrime(table->size * 2);

  // Inicializujeme novu tabulku
  HashTable *tb = initHashTable(table_size);

  int index = 0;

  // Pre kazdy prvok v starej tabulke nakopirujeme do novej
  for (int i = 0; i < table->size; i++)
  {
    Item *item = table->items[i];
    if (item != NULL)
    {
      index = HashTableInsert(tb,item->key->str,item->data.tokenType,item->data.ptr);

      tb->items[index]->data.inicialization = item->data.inicialization;
      tb->items[index]->data.var_number = item->data.var_number;
    }
  }

  table->item_count = tb->item_count;

  // Flipneme hodnoty
  const int tmp_size = table->size;
  table->size = tb->size;
  tb->size = tmp_size;

  // flipneme ukazovatele
  Item **tmp_items = table->items;
  table->items = tb->items;
  tb->items = tmp_items;

  freeHashTable(tb);
}

int HashTableInsert(struct HashTable *table,char *key,int type,void *ptr)
{
  const int load = table->item_count * 100 / table->size;
  /*
    Ak je tabulka zaplnena na viac nez 65%
    tak collision error stupa preto zvacsime tabulku
  */
  if (load > 65)
  {
    resizeHashTable(&(*table));
  }

  // Vytvorime strukturu itemu
  struct Item *item = (struct Item *)malloc(sizeof(struct Item));

  // Vytvorime 2 pointery na stringy
  struct String *k;
  // Inicializujeme ich
  initString(&k);
  // Nakopirujeme tam stringy
  copyString(key,k);
  // Pripevnime struktury na Item
  item->key = k;

  // Inicializujeme hodnoty itemu
  item->data.tokenType = type;
  item->data.inicialization = 0;
  item->data.var_number = 0;
  item->data.ptr = ptr;

  // Hashneme key
  int index = getHash(item->key->str,table->size,0);

  // Ziskame current item ktory sa nachadza na indexe
  struct Item *cur_item = table->items[index];

  int i = 1;

  // Hladame prazdne miesto na ulozenie
  while (cur_item != NULL)
  {
    // Zistime ci ITEM nevkladame 2 raz
    if (strcmp(cur_item->key->str,item->key->str) == 0)
    {
      // Item ktory vkladame tam uz je
      freeItem(item);
      return -1;
    }
    index = getHash(item->key->str,table->size,i);
    cur_item = table->items[index];
    i++;
  }
  table->items[index] = item;
  table->item_count++;
  return index;
}

struct Item * HashTableSearch(struct HashTable *table,char *key)
{
  int index = getHash(key,table->size,0);
  struct Item *it = table->items[index];
  int i = 1;

  while(it != NULL)
  {
    if (strcmp(it->key->str,key) == 0)
    {
      return it;
    }
    index = getHash(key,table->size,i);
    it = table->items[index];
    i++;
  }
  return NULL;
}

int HashTableDelete(struct HashTable *table,char *key)
{
  int index = getHash(key,table->size,0);
  struct Item *item = table->items[index];

  int i = 1;
  while (item != NULL)
  {
    if (strcmp(item->key->str,key) == 0)
    {
      freeItem(table->items[index]);
      table->items[index] = NULL;
      table->item_count--;
      return index;
    }
    index = getHash(key,table->size,i);
    item = table->items[index];
    i++;
  }
  return -1;
}

int getHash(char *s,const int table_size,const int atempt)
{
  int hash_a = hashString(s,PRIME_1,table_size);
  int hash_b = hashString(s,PRIME_2,table_size);
  return (hash_a + (atempt * (hash_b + 1))) % table_size;
}
/*
hashString zobere:
- s => Array charakterov ktore sa budu hashovat (key)
- a => Prvocislo ktore bude pouzite pri hashovani
- m => <0,m> index do ktoreho sa snazime umiestnit hash
*/
long long hashString(char *s,double a,int m)
{
  long long hash = 0;    // Variable moze byt dlksia nez int tak preto pouzijeme long long ktory je 18446744073709551615
  int len_s = strlen(s);  // Dlzka stringu
  for (int i = 0; i < len_s; i++) // Pre kazdy jeden charakter pripocitame urcitu hodnotu
  {
    hash += (long long)pow(a, len_s - (i+1)) * s[i];
  }
  hash = hash % m;
  return hash;
}

void freeItem(struct Item *item)
{
  freeString(item->key);
  free(item);
}

void freeHashTable(struct HashTable *table)
{
  for (int i = 0; i < table->size; i++)
  {
    if (table->items[i] != NULL)
    {
      if (table->items[i]->data.ptr != NULL)
      {
        freeHashTable(((struct HashTable *)table->items[i]->data.ptr));
      }
      freeItem(table->items[i]);
    }
  }
  free(table->items);
  free(table);
}
