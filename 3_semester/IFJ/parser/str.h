#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include <string.h>

typedef struct String{
  int length; // Mnozstvo charakterov v str bez '\0'
  char *str;  // Array charakterov
  int alloc;  // Mnozstvo naalokovanych charakterov
}String;


void initString(struct String **str);   // Inicializuje string
void appendChar(char ch,struct String *str);  // Pripevni charakter k stringu
void appendString(char *str_2,struct String *str_1);  // Pripevni cely array charakterov k stringu
void copyString(char *str_1,struct String *str_2);  // Nakopiruje array charakterov na string
int stringLength(struct String *str); // Vrati dlzku stringu
int containString(struct String *str,char *syntax); // Skontroluje od ktoreho charakteru je syntax
int charOccurances(struct String *str,char ch); // Napocita kolko krat sa nachadza charakter v stringu
void splitString(struct String *str,struct String *splitted_string,int split);  // Rozdeli string na indexe
void joinString(struct String *str_1,struct String *str_2); // Spoji 2 stringy
void clearString(struct String *str); // Vycisti cely string
void freeString(struct String *str);  // Uvolni string

#endif
