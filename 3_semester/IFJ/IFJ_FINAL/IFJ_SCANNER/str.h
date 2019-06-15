#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

typedef struct String{
  int length; // Mnozstvo charakterov v str bez '\0'
  char *str;  // Array charakterov
  int alloc;  // Mnozstvo naalokovanych charakterov
}String;


void initString(struct String **str);   // OK
void appendChar(char ch,struct String *str);  // OK
void appendString(char *str_2,struct String *str_1);  // OK
void copyString(char *str_1,struct String *str_2);  // OK
int charLength(char *str);  // OK
int stringLength(struct String *str); // OK
int containString(struct String *str,char *syntax); // OK
int charOccurances(struct String *str,char ch);
void splitString(struct String *str,struct String *splitted_string,int split);  // OK
void joinString(struct String *str_1,struct String *str_2); // OK
void clearString(struct String *str); // OK
void freeString(struct String *str);  // OK

#endif
