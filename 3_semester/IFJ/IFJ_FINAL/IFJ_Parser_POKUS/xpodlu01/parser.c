/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
 *
 **/

#include "semantic.h"
#include "err_code.h"
#include "bin_struct.h"
#include "generate_code.h"
#include "expression_parsing.h"


#include <stdio.h>
#include <stdlib.h>

typedef enum ScopeIdentifier
{
  SI_WHILE,
  SI_IF,
  SI_ELSE,
  SI_FUNCTION
}ScopeIdentifier;


/*
  HashTable bude pouzity na ukladanie globalnych variable a funkcii
  tokenStack bude pouzity na predavanie medzi funkciami
  function_call donho budeme ukladat cally funkcii
  scopeStack bude obsahovat scopy if while def else
  instructionList bude obsahovat list instrukcii
*/
struct HashTable *globalTable = NULL;
struct TokenStack *tokenStack = NULL;
struct InstructionStruct *instructionList = NULL;

struct TokenStack *function_call = NULL;
struct TokenStack *scopeStack = NULL;



struct HashTable * workingHashTable();
int ParseExpression();
int ParseFunctionCall();
int ParseIf();
/*
  Variable pre generovanie kodu
  var_counter bude pouzity na generovanie variablov typu A0,A1 ..
  while_counter bude pouzity na vytvaranie labelov WHILESTART0,WHILEEND0 ...
  if_counter bude pouzity na vytvaranie labelov IFSTART0,IFELSE
*/
int var_counter = 0;
int if_counter = 0;
int while_counter = 0;

String *currFunction = NULL;

// Kontrola ci tokenType je jedna z konstant
int isConstant(TokenType type)
{
  switch (type)
  {
    case TT_STRING:
    case TT_FLOAT:
    case TT_EXPONENT:
    case TT_INTEGER:
    case TT_NIL:
      return 1;
    break;
    default:
      return 0;
    break;
  }
}

int createVarDef(char *str)
{
  struct Item *itm = NULL;

  struct HashTable *hashTable = NULL;

  if (((hashTable = workingHashTable())) == NULL)
  {
    return INTERNAL_COMP_ERR;
  }


  // Skontrolujeme najprv ci sa to nenachadza v global hash table
  if ((itm = HashTableSearch(globalTable,str)) != NULL)
  {
    // Ak sa nachadza skontrolujeme ci sa jedna o identifikator
    if (itm->data.tokenType != TT_IDENTIFIKATOR)
    {
      // Ak sa o neho nejedna error
      return SEMANTIC_PROG_ERR;
    }
  }

  // Skontrolujeme ci premenna existuje
  if ((itm = HashTableSearch(hashTable,str)) == NULL)
  {
    // Ak je ju treba vytvorit tak najprv zistime kde ju mozme definovat
    // Vytvorime string
    String *tmp_str;
    initString(&tmp_str);

    // Vytvorime prvu cast LF@
    appendString("LF@",tmp_str);

    // DOplnime LF@identifikator
    appendString(str,tmp_str);

    // Vytvorime item
    struct TokenStackItem *tmp_item = NULL;

    for (int i = 0; i < sizeStack(scopeStack); i++)
    {
      tmp_item = peekStack(scopeStack,i);

      if (tmp_item != NULL)
      {
        // Ak sme nasli while
        if (tmp_item->type == TT_WHILE)
        {
          // Tak pridame definiciu variable nad zaciatok WHILE
          prependInstruction(instructionList,tmp_item->ptr,IT_DEFVAR,tmp_str->str,NULL,NULL);

          // Pridame move instrukciu na inicializovanie na nil hodnotu
          prependInstruction(instructionList,tmp_item->ptr,IT_MOVE,tmp_str->str,"nil@nil",NULL);

          // Pridame do HashTable
          HashTableInsert(hashTable,str,TT_IDENTIFIKATOR,NULL);

          // Uvolnime
          freeString(tmp_str);

          // Vlozime
          return 0;
        }
      }
    }

    // Inac skontrolujeme ci sme nasli posledny IF
    if (tmp_item != NULL)
    {
      if (tmp_item->type != TT_IF)
      {
        freeString(tmp_str);
        return INTERNAL_COMP_ERR;
      }

      // Tak pridame definiciu variable nad zaciatok IF
      prependInstruction(instructionList,tmp_item->ptr,IT_DEFVAR,tmp_str->str,NULL,NULL);

      // Pridame move instrukciu na inicializovanie na nil hodnotu
      prependInstruction(instructionList,tmp_item->ptr,IT_MOVE,tmp_str->str,"nil@nil",NULL);

      // Pridame do HashTable
      HashTableInsert(hashTable,str,TT_IDENTIFIKATOR,NULL);

      // Uvolnime
      freeString(tmp_str);

      return 0;
    }

    // Inac normalne appendujeme definiciu
    appendInstruction(instructionList,IT_DEFVAR,tmp_str->str,NULL,NULL);

    // Inicializujeme na NIL hodnotu
    appendInstruction(instructionList,IT_MOVE,tmp_str->str,"nil@nil",NULL);

    // Pridame do HashTable
    HashTableInsert(hashTable,str,TT_IDENTIFIKATOR,NULL);

    //Uvolnime string
    freeString(tmp_str);
  }
  // Ak nahodou uz existuje treba skontrolovat ci sa nahodou nesnazime redefinovat nieco co uz existuje
  else
  {
    // Ak type itemu nie je TT_IDENTIFIKATOR tak je to error lebo sa snazime o redefiniciu
    if (itm->data.tokenType != TT_IDENTIFIKATOR)
    {
      return SEMANTIC_PROG_ERR;
    }
  }
  return 0;
}

int createRetval(char *str,int type)
{
  // Ak je RETVAL token
  if (type == TT_FUN_RETVAL)
  {
    // Vytvorime instrukciu ak sme vo funkcii
    if (stringLength(currFunction) > 0)
    {
      appendInstruction(instructionList,IT_MOVE,"LF@%retval",str,NULL);
    }
    return 0;
  }
  int errCode = 0;
  String *tmp_str;
  initString(&tmp_str);

  // Zmenime str do assembly
  if ((errCode = changeToAssembly(type,str,tmp_str)) != 0)
  {
    // Uvolnime string
    freeString(tmp_str);
    // Vratime error
    return errCode;
  }

  // Ak je to premenna
  if (type == TT_IDENTIFIKATOR)
  {
    // Vytvorime string
    String *tmp_str_1;
    initString(&tmp_str_1);

    // Ziskame premennu
    returnVarName(tmp_str->str,tmp_str_1);

    // Treba skontrolovat ci je definovana
    if (HashTableSearch(workingHashTable(),tmp_str_1->str) == NULL)
    {
      // Uvolnime stringy
      freeString(tmp_str);
      freeString(tmp_str_1);
      return SEMANTIC_PROG_ERR;
    }
    // Uvolnime string
    freeString(tmp_str_1);
  }

  // Generujeme len vtedy ked je currFunction dlksia nez 0
  if (stringLength(currFunction) > 0)
  {
    // Inac vygenerujeme RETVAL
    appendInstruction(instructionList,IT_MOVE,"LF@%retval",tmp_str->str,NULL);
  }

  // Uvolnime string
  freeString(tmp_str);
  // Vratime OK
  return 0;
}

int checkMathOperators(TokenType type)
{
  switch (type)
  {
    case TT_PLUS:
    case TT_MINUS:
    case TT_MULTIPLY:
    case TT_DIVISION:
      return type;
    break;
    default:
      return 0;
    break;
  }
}

int checkBinaryOperators(TokenType type)
{
  switch (type)
  {
    case TT_LOWER:              // <
    case TT_LOWER_EQUAL:        // <=
    case TT_HIGHER:             // >
    case TT_HIGHER_EQUAL:       // >=
    case TT_EQUAL:              // ==
    case TT_FALSE_EQUAL:        // !=
      return type;
    break;
    default:
      return 0;
    break;
  }
}

int checkReserved(char *str)
{
  // Urobime si zoznam co nemoze byt
  char *reserved[17] =
  {
    // Ak je to jeden z keywordov
    "def",
    "do",
    "else",
    "end",
    "if",
    "not",
    "nil",
    "then",
    "while",
    // Alebo je to jeden z rezervovanych funkcii
    "inputs",
    "inputf",
    "inputi",
    "print",
    "length",
    "substr",
    "ord",
    "chr"
  };

  // Prejdeme vsetky a skontrolujeme
  for (int i = 0; i < 17; i++)
  {
    if (strcmp(str,reserved[i]) == 0)
    {
      // Nasli sme vratime true
      return 1;
    }
  }

  // Nenasli sme nic vratime false
  return 0;
}

int checkIdentifikator(char *str)
{
  Item *item = NULL;
  struct HashTable *hTable = NULL;

  if ((hTable = workingHashTable()) == NULL)
  {
    return -1;
  }

  if ((item = HashTableSearch(hTable,str)) == NULL)
  {
    // Vratime -1 neni tam
    return -1;
  }
  // Inac vratime typ
  return item->data.tokenType;
}

int initParserReservedFunctions()
{
  // ErrCode
  int errCode = 0;

  // Vlozime inputs
  HashTableInsert(globalTable,"inputs",TT_FUN_RESERVED,NULL);

  // Inicializujeme ju
  if ((errCode = inicializeFun(globalTable,"inputs",0)) != 0)
  {
    return errCode;
  }

  // Vlozime inputi
  HashTableInsert(globalTable,"inputi",TT_FUN_RESERVED,NULL);

  // Inicializujeme ju
  if ((errCode = inicializeFun(globalTable,"inputi",0)) != 0)
  {
    return errCode;
  }

  // Vlozime inputf
  HashTableInsert(globalTable,"inputf",TT_FUN_RESERVED,NULL);

  // Inicializujeme ju
  if ((errCode = inicializeFun(globalTable,"inputf",0)) != 0)
  {
    return errCode;
  }

  // Vlozime print
  HashTableInsert(globalTable,"print",TT_FUN_RESERVED,NULL);

  // Inicializujeme ju
  if ((errCode = inicializeFun(globalTable,"print",0)) != 0)
  {
    return errCode;
  }

  // Vlozime length
  HashTableInsert(globalTable,"length",TT_FUN_RESERVED,NULL);

  // Inicializujeme ju
  if ((errCode = inicializeFun(globalTable,"length",1)) != 0)
  {
    return errCode;
  }

  // Vlozime substr
  HashTableInsert(globalTable,"substr",TT_FUN_RESERVED,NULL);

  // Inicializujeme ju
  if ((errCode = inicializeFun(globalTable,"substr",3)) != 0)
  {
    return errCode;
  }

  // Vlozime ord
  HashTableInsert(globalTable,"ord",TT_FUN_RESERVED,NULL);

  // Inicializujeme ju
  if ((errCode = inicializeFun(globalTable,"ord",2)) != 0)
  {
    return errCode;
  }

  // Vlozime chr
  HashTableInsert(globalTable,"chr",TT_FUN_RESERVED,NULL);

  // Inicializujeme ju
  if ((errCode = inicializeFun(globalTable,"chr",1)) != 0)
  {
    return errCode;
  }

  // Vratime konecny kod
  return errCode;
}

int initParser()
{
  // Vytvorime globalny table pre variable definovane v programe mimo funkcie
  globalTable = initHashTable(HS_INIT_SIZE);

  // Stack ktory bude obsahovat tokeny a budu sa predavat medzi funkciami
  tokenStack = createStack();
  // Stack bude obsahovat cally funkcii aby sa mohli potom skontrolovat ci boly volane s dobrym poctom variable
  function_call = createStack();
  // scopeStack je stack ktory bude mat vzdy While If Zaciatok funkcie a pod a vzdy ked natrafime na end popneme posledne pridany
  scopeStack = createStack();
  // instructionList je list instrukcii z ktoreho sa bude generovat pseudo assembler
  instructionList = createInstructionStruct();
  // Inicializujeme currFunction => Nazov funkcie v ktorej sa nachadzame pokial to nie je "" tak nemozme dalsiu definovat
  initString(&currFunction);

  // Ak nastala chyba pri mallocu
  if (globalTable == NULL || tokenStack == NULL || currFunction == NULL || scopeStack == NULL || instructionList == NULL || function_call == NULL)
  {
    // Vraciame chybu
    return INTERNAL_COMP_ERR;
  }
  // Zavolame funkciu nech sa vygeneruju TT_FUN_RESERVED
  return initParserReservedFunctions();
}

struct HashTable * workingHashTable()
{
  // Ak je nazov funkcie prazdny
  if (currFunction->length == 0)
  {
    // Vraciame hromadnu table
    return globalTable;
  }
  // Inac
  else
  {
    // Ziskame item z nazvom nasej funkcie
    struct Item *item = NULL;
    if ((item = HashTableSearch(globalTable,currFunction->str)) == NULL)
    {
      return NULL;
    }

    // Vratime naspat
    return ((struct HashTable *)(item->data.ptr));
  }
}

void appendFunctionCall(char *fun_name,int var_number)
{
  pushStack(function_call,var_number,NULL,fun_name);
}

void getFunctionName(char *str,String *ret_str)
{
  // Vycistime
  clearString(ret_str);
  copyString(str,ret_str);

  String *tmp_str;
  initString(&tmp_str);

  int index = -1;

  index = containString(ret_str,"$");

  if (index >= 0)
  {
    // Splitneme
    splitString(ret_str,tmp_str,4);
  }

  freeString(tmp_str);
}

int checkFunctionCalls()
{
  TokenStackItem *item = NULL;
  Item *hItem = NULL;
  String *fun_name;
  initString(&fun_name);


  // Zacneme loopovat cely stack
  for (int i = 0; i < sizeStack(function_call); i++)
  {
    // AK je empty je chyba
    if ((item = peekStack(function_call,i)) == NULL)
    {
      freeString(fun_name);
      // Vratime chybu
      return INTERNAL_COMP_ERR;
    }

    // Ziskame meno funkcie
    getFunctionName(item->str->str,fun_name);

    // Inac zacneme kontrolovat
    if ((hItem = HashTableSearch(globalTable,fun_name->str)) == NULL)
    {
      freeString(fun_name);
      return SEMANTIC_PROG_ERR;
    }

    // Inac skontrolujeme ci sa jedna o funkciu a ak hej tak ci pocet premennych sedi
    if (hItem->data.tokenType == TT_FUN_RESERVED || hItem->data.tokenType == TT_FUN_IDENTIFIKATOR)
    {
      // Skontrolujeme ci pocet premennych potrebnych je rovny poctu premennych zavolanych
      if (hItem->data.var_number == ((int)item->type))
      {
        continue;
      }
      freeString(fun_name);
      return SEMANTIC_ARGUMENTS_ERR;
    }
    freeString(fun_name);
    return SEMANTIC_PROG_ERR;
  }
  freeString(fun_name);
  return 0;

}

int popScopeStack(char *str,TokenType type)
{
  // Item
  TokenStackItem *item = NULL;

  // Prejdeme cely stack
  for (int i = 0; i < sizeStack(scopeStack); i++)
  {
    item = peekStack(scopeStack,i);

    if (item != NULL)
    {
      // Ak je to IF
      if (item->type == type)
      {
        // Skontrolujeme ci sa jedna o nase IF
        if (strcmp(item->str->str,str) == 0)
        {
          // Mozme ho popnut
          popIndex(scopeStack,i);
          // Ukoncime
          return 0;
        }
      }
    }
  }

  // Inac sme nenasli nic a vraciame INTERNAL ERRORR
  return INTERNAL_COMP_ERR;
}

/*
  Funkcia sluzi na parsovanie While loopu
*/
int ParseWhile()
{
  int tokenType = -60;

  String *tmp_str;
  initString(&tmp_str);

  // WHILE identifikator
  int while_identifikator = while_counter++;
  int errCode = 0;

  char end_while[200];

  // Nazov zaciatku loopu
  sprintf(end_while,"$while$%d$start$loop",while_identifikator);

  // Pridame instrukciu na zaciatok loopu
  appendInstruction(instructionList,IT_LABEL,end_while,NULL,NULL);

  // Pushneme na stack pointer na instrukciu
  pushStack(scopeStack,TT_WHILE,&(*instructionList->Last),end_while);

  // Pred volanim funkcie musime clearnut stack
  clearStack(tokenStack);

  // Zavolame precedencnu analyzu ktora nam vrati derivacny strom
  if ((errCode = precedenceAnalysisParsing(tokenStack,WHILE_MOD)) != 0)
  {
    // Vratime errCode
    freeString(tmp_str);
    return errCode;
  }
  // po IF VYRAZ THEN musi is EOL
  if (getToken(tmp_str) != TT_EOL)
  {
    // Uvolnime strukturu
    freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
    // Uvolnime string
    freeString(tmp_str);
    return SYNTAX_ERR;
  }

  // Ziskame hashtable s ktorou robime
  struct HashTable *hTable = workingHashTable();

  // Ak sme ju nenasli tak vraciame error
  if (hTable == NULL)
  {
    // Uvolnime strukturu
    freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
    // Uvolnime string a ukoncime
    freeString(tmp_str);
    return INTERNAL_COMP_ERR;
  }

  // Vygenerujeme while
  if ((errCode = generateWhile(instructionList,hTable,((struct binStruct *)peekStack(tokenStack,0)->ptr),while_identifikator,&var_counter)) != 0)
  {
    // Uvolnime strukturu
    freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
    // Uvolnime string
    freeString(tmp_str);
    // Vratime errCode
    return errCode;
  }

  // Uvolnime strukturu
  freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));

  // Vycistime stack
  clearStack(tokenStack);

  // Vycistime string
  clearString(tmp_str);

  // Ziskame prvy token
  tokenType = getToken(tmp_str);

  // AK nam scanner vratil ERROR
  if (tokenType < 0)
  {
    freeString(tmp_str);
    return LEXICAL_ERR;
  }

  // Pushneme ho na stack
  pushStack(tokenStack,tokenType,NULL,tmp_str->str);
  // WHILE BODY
  while (tokenType != TT_END)
  {
    // A skontrolujeme co sme dostali
    switch (tokenType)
    {
      // Moze byt while
      case TT_WHILE:
        if ((errCode = ParseWhile()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;
      // Dalsie IF
      case TT_IF:
        if ((errCode = ParseIf()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_IDENTIFIKATOR:
      case TT_INTEGER:
      case TT_FLOAT:
      case TT_EXPONENT:
      case TT_STRING:
      case TT_LEFT_BRACKET:

        // Tuto funkciu volame ked sme si isty ze tam assignment neni
        if ((errCode = ParseExpression()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_FUN_IDENTIFIKATOR:
        if ((errCode = ParseFunctionCall()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_EOL:
        // NIC NEROBIME
      break;

      default:

        freeString(tmp_str);
        if (tokenType < 0)
        {
          return LEXICAL_ERR;
        }
        return SYNTAX_ERR;
      break;
    }

    // Vycistime string
    clearString(tmp_str);

    // Vycistime stack
    clearStack(tokenStack);

    // Ziskame token
    tokenType = getToken(tmp_str);

    // AK nam scanner vratil ERROR
    if (tokenType < 0)
    {
      freeString(tmp_str);
      return LEXICAL_ERR;
    }

    // Pushneme ho na stack
    pushStack(tokenStack,tokenType,NULL,tmp_str->str);
  }

  // Ked najdeme END musi nasledovat EOL
  if ((tokenType = getToken(tmp_str)) != TT_EOL)
  {
    freeString(tmp_str);
    return SYNTAX_ERR;
  }

  // Jumpneme naspat na zaciatok
  appendInstruction(instructionList,IT_JUMP,end_while,NULL,NULL);

  // Vygenerujeme end label
  sprintf(end_while,"$while$%d$end$loop",while_identifikator);


  // Vytvorime LABEL pre skok z while loopu
  appendInstruction(instructionList,IT_LABEL,end_while,NULL,NULL);

  // Zobereme zaciatocny nazov
  sprintf(end_while,"$while$%d$start$loop",while_identifikator);

  // popneme ho zo stacku lebo sme na konci WHILE
  if ((errCode = popScopeStack(end_while,TT_WHILE)) != 0)
  {
    // Uvolnime a ukoncime
    freeString(tmp_str);
    return errCode;
  }

  // While vracia nil
  if (stringLength(currFunction) > 0)
  {
    // Cize %retval zmenime na nil po vykonani whilu
    appendInstruction(instructionList,IT_MOVE,"LF@%retval","nil@nil",NULL);
  }

  // Uvolnime a ukoncime
  freeString(tmp_str);
  return 0;
}

int ParseIf()
{
  int tokenType = -60;
  // Vygenerujeme koniec IF_ELSE

  //  Error code
  int errCode = 0;
  int if_identifikator = if_counter++;

  char end_ifelse[200];
  sprintf(end_ifelse,"$if$else$%d$start",if_identifikator);

  // Appendneme instrukciu
  appendInstruction(instructionList,IT_LABEL,end_ifelse,NULL,NULL);

  // Pushneme na stack pointer na item
  pushStack(scopeStack,TT_IF,&(*instructionList->Last),end_ifelse);

  String *tmp_str;
  initString(&tmp_str);

  // Pred volanim funkcie musime clearnut stack
  clearStack(tokenStack);


  // Zavolame precedencnu analyzu ktora nam vrati derivacny strom
  if ((errCode = precedenceAnalysisParsing(tokenStack,IF_MOD)) != 0)
  {
    // Vratime errCode
    freeString(tmp_str);
    return errCode;
  }

  // po IF VYRAZ THEN musi is EOL
  if (getToken(tmp_str) != TT_EOL)
  {
    // Uvolnime strukturu
    freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
    // Uvolnime string
    freeString(tmp_str);
    return SYNTAX_ERR;
  }

  // Ziskame hashtable s ktorou robime
  struct HashTable *hTable = workingHashTable();

  // Ak sme ju nenasli tak vraciame error
  if (hTable == NULL)
  {
    // Uvolnime strukturu
    freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
    // Uvolnime string a ukoncime
    freeString(tmp_str);
    return INTERNAL_COMP_ERR;
  }

  // Vygenerujeme if
  if ((errCode = generateIf(instructionList,hTable,((struct binStruct *)peekStack(tokenStack,0)->ptr),if_identifikator,&var_counter)) != 0)
  {
    // Uvolnime strukturu
    freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
    // Uvolnime string
    freeString(tmp_str);
    // Vratime errCode
    return errCode;
  }

  // Uvolnime strukturu
  freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));

  // Vycistime stack
  clearStack(tokenStack);

  // Vycistime string
  clearString(tmp_str);

  // Ziskame prvy token
  tokenType = getToken(tmp_str);

  // AK nam scanner vratil ERROR
  if (tokenType < 0)
  {
    freeString(tmp_str);
    return LEXICAL_ERR;
  }

  // Pushneme ho na stack
  pushStack(tokenStack,tokenType,NULL,tmp_str->str);

  // Retval ak nie su ziadne prikazy bude NIL
  createRetval("nil@nil",TT_NIL);

  // IF BODY
  while (tokenType != TT_ELSE)
  {
    // A skontrolujeme co sme dostali
    switch (tokenType)
    {
      // Moze byt while
      case TT_WHILE:
        if ((errCode = ParseWhile()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;
      // Dalsie IF
      case TT_IF:
        if ((errCode = ParseIf()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_IDENTIFIKATOR:
      case TT_INTEGER:
      case TT_FLOAT:
      case TT_EXPONENT:
      case TT_STRING:
      case TT_LEFT_BRACKET:

        // Tuto funkciu volame ked sme si isty ze tam assignment neni
        if ((errCode = ParseExpression()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_FUN_IDENTIFIKATOR:
        if ((errCode = ParseFunctionCall()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_EOL:
        // NIC NEROBIME
      break;

      default:

        freeString(tmp_str);
        if (tokenType < 0)
        {
          return LEXICAL_ERR;
        }
        return SYNTAX_ERR;
      break;
    }

    // Vycistime string
    clearString(tmp_str);

    // Vycistime stack
    clearStack(tokenStack);

    // Ziskame prvy token
    tokenType = getToken(tmp_str);

    // AK nam scanner vratil ERROR
    if (tokenType < 0)
    {
      freeString(tmp_str);
      return LEXICAL_ERR;
    }

    // Pushneme ho na stack
    pushStack(tokenStack,tokenType,NULL,tmp_str->str);
  }

  // Za ELSE musi ist EOL
  if ((tokenType = getToken(tmp_str)) != TT_EOL)
  {
    freeString(tmp_str);
    return SYNTAX_ERR;
  }


  // Vygenerujeme ELSE
  generateElse(instructionList,if_identifikator);

  // Vycistime stack
  clearStack(tokenStack);

  // Vycistime string
  clearString(tmp_str);

  // Ziskame prvy token
  tokenType = getToken(tmp_str);

  // AK nam scanner vratil ERROR
  if (tokenType < 0)
  {
    freeString(tmp_str);
    return LEXICAL_ERR;
  }

  // Pushneme ho na stack
  pushStack(tokenStack,tokenType,NULL,tmp_str->str);

  // Retval ak je prazdne bude NIL
  createRetval("nil@nil",TT_NIL);

  // ELSE BODY
  while (tokenType != TT_END)
  {
    // A skontrolujeme co sme dostali
    switch (tokenType)
    {
      // Moze byt while
      case TT_WHILE:
        if ((errCode = ParseWhile()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;
      // Dalsie IF
      case TT_IF:
        if ((errCode = ParseIf()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_IDENTIFIKATOR:
      case TT_INTEGER:
      case TT_FLOAT:
      case TT_EXPONENT:
      case TT_STRING:
      case TT_LEFT_BRACKET:

        // Tuto funkciu volame ked sme si isty ze tam assignment neni
        if ((errCode = ParseExpression()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_FUN_IDENTIFIKATOR:
        if ((errCode = ParseFunctionCall()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_EOL:
        // NIC NEROBIME
      break;

      default:

        freeString(tmp_str);
        if (tokenType < 0)
        {
          return LEXICAL_ERR;
        }
        return SYNTAX_ERR;
      break;
    }

    // Vycistime string
    clearString(tmp_str);

    // Vycistime stack
    clearStack(tokenStack);

    // Ziskame token
    tokenType = getToken(tmp_str);

    // AK nam scanner vratil ERROR
    if (tokenType < 0)
    {
      freeString(tmp_str);
      return LEXICAL_ERR;
    }

    // Pushneme ho na stack
    pushStack(tokenStack,tokenType,NULL,tmp_str->str);
  }

  // Ked najdeme END musi nasledovat EOL
  if ((tokenType = getToken(tmp_str)) != TT_EOL)
  {
    freeString(tmp_str);
    return SYNTAX_ERR;
  }

  sprintf(end_ifelse,"$if$else$%d$end",if_identifikator);

  appendInstruction(instructionList,IT_LABEL,end_ifelse,NULL,NULL);

  // If Skoncilo popujeme
  sprintf(end_ifelse,"$if$else$%d$start",if_identifikator);

  // Popneme IF
  if ((errCode = popScopeStack(end_ifelse,TT_IF)) != 0)
  {
    freeString(tmp_str);
    return errCode;
  }

  freeString(tmp_str);
  return 0;
}

int functionSpecialSemanticCheck(int function_index)
{
  TokenStackItem *function_name = NULL;
  TokenStackItem *var_item = NULL;

  // Ak sme nenasli nic
  if ((function_name = peekStack(tokenStack,function_index)) == NULL)
  {
    return INTERNAL_COMP_ERR;
  }

  // Ak sa jedna o length
  if (strcmp(function_name->str->str,"length") == 0)
  {
    // Ziskame prvu premennu
    if ((var_item = peekStack(tokenStack,(function_index + 1))) == NULL)
    {
      return INTERNAL_COMP_ERR;
    }

    // Premenna moze byt iba STRING alebo IDENTIFIKATOR
    if (var_item->type == TT_IDENTIFIKATOR || var_item->type == TT_STRING)
    {
      // Vkladame dobry typ
      return 0;
    }
    // Zly typ vkladame do funkcie
    return SEMANTIC_TYPE_COMP_ERR;
  }
  // Ak sa jedna o substr
  else if (strcmp(function_name->str->str,"substr") == 0)
  {
    // Ziskame prvu premennu
    if ((var_item = peekStack(tokenStack,(function_index + 1))) == NULL)
    {
      return INTERNAL_COMP_ERR;
    }

    // Premenna moze byt iba STRING alebo IDENTIFIKATOR
    if (!(var_item->type == TT_IDENTIFIKATOR || var_item->type == TT_STRING))
    {
      // Ak neni vraciame chybu
      return SEMANTIC_TYPE_COMP_ERR;
    }

    // Ziskame druhu premennu
    if ((var_item = peekStack(tokenStack,(function_index + 2))) == NULL)
    {
      return INTERNAL_COMP_ERR;
    }

    // Premenna moze byt iba INTEGER alebo IDENTIFIKATOR alebo FLOAT
    if (!(var_item->type == TT_IDENTIFIKATOR || var_item->type == TT_INTEGER || var_item->type == TT_FLOAT || var_item->type == TT_EXPONENT))
    {
      // Ak neni vraciame chybu
      return SEMANTIC_TYPE_COMP_ERR;
    }

    // Ziskame tretiu premennu
    if ((var_item = peekStack(tokenStack,(function_index + 3))) == NULL)
    {
      return INTERNAL_COMP_ERR;
    }

    // Premenna moze byt iba INTEGER alebo IDENTIFIKATOR alebo FLOAT
    if (!(var_item->type == TT_IDENTIFIKATOR || var_item->type == TT_INTEGER || var_item->type == TT_FLOAT || var_item->type == TT_EXPONENT))
    {
      // Ak neni vraciame chybu
      return SEMANTIC_TYPE_COMP_ERR;
    }
    // Inac vraciame 0
    return 0;
  }
  // Ak sa jedna o substr
  else if (strcmp(function_name->str->str,"ord") == 0)
  {
    // Ziskame prvu premennu
    if ((var_item = peekStack(tokenStack,(function_index + 1))) == NULL)
    {
      return INTERNAL_COMP_ERR;
    }

    // Premenna moze byt iba STRING alebo IDENTIFIKATOR
    if (!(var_item->type == TT_IDENTIFIKATOR || var_item->type == TT_STRING))
    {
      // Ak neni vraciame chybu
      return SEMANTIC_TYPE_COMP_ERR;
    }

    // Ziskame druhu premennu
    if ((var_item = peekStack(tokenStack,(function_index + 2))) == NULL)
    {
      return INTERNAL_COMP_ERR;
    }

    // Premenna moze byt iba INTEGER alebo IDENTIFIKATOR alebo FLOAT
    if (!(var_item->type == TT_IDENTIFIKATOR || var_item->type == TT_INTEGER || var_item->type == TT_FLOAT || var_item->type == TT_EXPONENT))
    {
      // Ak neni vraciame chybu
      return SEMANTIC_TYPE_COMP_ERR;
    }
    // Inac vraciame 0
    return 0;
  }
  // Ak sa jedna o substr
  else if (strcmp(function_name->str->str,"chr") == 0)
  {
    // Ziskame prvu premennu
    if ((var_item = peekStack(tokenStack,(function_index + 1))) == NULL)
    {
      return INTERNAL_COMP_ERR;
    }

    // Premenna moze byt iba STRING alebo IDENTIFIKATOR alebo FLOAT
    if (!(var_item->type == TT_IDENTIFIKATOR || var_item->type == TT_INTEGER || var_item->type == TT_FLOAT || var_item->type == TT_EXPONENT))
    {
      // Ak neni vraciame chybu
      return SEMANTIC_TYPE_COMP_ERR;
    }
    // Inac vraciame 0
    return 0;
  }
  // Nejedna sa specialnu funkciu neriesime
  else
  {
    return 0;
  }
}

int functionSyntaxCheck(Item *fun,int fun_start_index)
{
  // Bude obsahovat ci mame dobry pocet zatvoriek
  int bracket_count = 0;
  int var_count = 0;
  int var_before = 0;
  TokenStackItem *item = NULL;

  int errCode = 0;

  // Skontrolujeme syntax
  for (int i = fun_start_index; i < sizeStack(tokenStack); i++)
  {
    // Ziskame item
    item = peekStack(tokenStack,i);

    // Skontrolujeme ci prvy item je bud ( alebo konstanta alebo identifikator
    if (item->type == TT_LEFT_BRACKET && bracket_count == 0)
    {
      popIndex(tokenStack,i);
      i--;
      bracket_count++;
      continue;
    }
    // Je to premenna ?
    else if (var_before == 0 && (item->type == TT_IDENTIFIKATOR || isConstant(item->type)))
    {
      // Ak hej tak zvysime counter var_before
      var_before++;
      var_count++;
      continue;
    }
    // Je to ciarka ?
    else if (var_before == 1 && item->type == TT_COMA)
    {
      var_before--;
      popIndex(tokenStack,i);
      i--;
      continue;
    }
    // Inac este moze byt RIGHT_BRACKET
    else if ((item->type == TT_RIGHT_BRACKET && bracket_count == 1 && var_before == 1) || (item->type == TT_RIGHT_BRACKET && bracket_count == 1 && var_before == 0 && var_count == 0))
    {
      // A hned za nim musi ist EOL
      popIndex(tokenStack,i);
      bracket_count--;

      item = peekStack(tokenStack,i);

      if (item->type != TT_EOL)
      {
        return SYNTAX_ERR;
      }
      // Skontrolujeme  ci sme na konci
      if (i == (sizeStack(tokenStack) - 1))
      {
        popIndex(tokenStack,i);
        break;
      }
      else
      {
        return SYNTAX_ERR;
      }
    }
    // Inac moze byt len EOL
    else if ((item->type == TT_EOL && bracket_count == 0 && var_before == 1) || (item->type == TT_EOL && var_count == 0 && var_before == 0 && bracket_count == 0))
    {
      // Skontrolujeme  ci sme na konci
      if (i == (sizeStack(tokenStack) - 1))
      {
        popIndex(tokenStack,i);
        break;
      }
      else
      {
        return SYNTAX_ERR;
      }
    }
    // Inac SYNTAX_ERR
    else
    {
      return SYNTAX_ERR;
    }
  }

  // Ak sa jedna o specialnu funkciu urobime semantiku
  if ((errCode = functionSpecialSemanticCheck((fun_start_index - 1))) != 0)
  {
    return errCode;
  }


  // Moze sa jednat o nedefinovany function call
  // Je vrateny ITEM NULL ?
  if (fun == NULL)
  {
    // Hash table
    struct HashTable *hTable = NULL;

    if ((hTable = workingHashTable()) == NULL)
    {
      return INTERNAL_COMP_ERR;
    }
    // Jedna sa o volanie nedefinovanej funkcie
    return generateFunctionCall(instructionList,hTable,tokenStack,(fun_start_index - 1));
  }
  // Inac ziskame informacie o nom
  else
  {
    // Jedna sa o volanie rezervovanej funkcie
    if (fun->data.tokenType == TT_FUN_RESERVED)
    {
      // Ak sa jedna o print funkcii ta je v intervale <1,N>
      if (strcmp(fun->key->str,"print") == 0)
      {
        if (var_count == 0)
        {
          return SEMANTIC_ARGUMENTS_ERR;
        }
        // Hash table
        struct HashTable *hTable = NULL;

        if ((hTable = workingHashTable()) == NULL)
        {
          return INTERNAL_COMP_ERR;
        }

        return generateFunctionCallPrint(instructionList,hTable,tokenStack,(fun_start_index - 1));
      }
      // Inac kontrolujeme normalne pocet premennych
      else
      {
        // AK je zly pocet premennych
        if (var_count != fun->data.var_number)
        {
          return SEMANTIC_ARGUMENTS_ERR;
        }

        // Hash table
        struct HashTable *hTable = NULL;

        if ((hTable = workingHashTable()) == NULL)
        {
          return INTERNAL_COMP_ERR;
        }

        return generateFunctionCall(instructionList,hTable,tokenStack,(fun_start_index - 1));
      }
      // Zavolame generaciu funkcie
    }
    // Jedna sa o volanie definovanej funkcie
    else if (fun->data.tokenType == TT_FUN_IDENTIFIKATOR)
    {
      // Hash table
      struct HashTable *hTable = NULL;

      if ((hTable = workingHashTable()) == NULL)
      {
        return INTERNAL_COMP_ERR;
      }

      return generateFunctionCall(instructionList,hTable,tokenStack,(fun_start_index - 1));
    }
    // Zle sme identifikovaly
    else
    {
      // Vratime semanticku chybu
      return SEMANTIC_PROG_ERR;
    }
  }
}

int ParseFunctionCall()
{
  struct Item *hItem = NULL;
  TokenStackItem *item = NULL;
  TokenStackItem *item_assignment = NULL;

  int function_name_index = 0;

  int tokenType = -6;
  String *tmp_str;
  String *function_name;
  initString(&tmp_str);
  initString(&function_name);

  // Ziskame prvy item
  item_assignment = peekStack(tokenStack,1);


  if (item_assignment == NULL)
  {
    // Ak je empty ziskame dalsi item
    tokenType = getToken(tmp_str);

    if (tokenType < 0)
    {
      freeString(tmp_str);
      freeString(function_name);
      return LEXICAL_ERR;
    }

    // Pushneme ho
    pushStack(tokenStack,tokenType,NULL,tmp_str->str);

    // A nacitame ho
    if ((item_assignment = peekStack(tokenStack,1)) == NULL)
    {
      freeString(tmp_str);
      freeString(function_name);
      return INTERNAL_COMP_ERR;
    }
  }
  // Inac ak je to = znamena ze by sme maly mat a = nazov_funkcie
  if (item_assignment->type == TT_ASSIGNMENT)
  {
    // Tak ziskame treti item
    if ((item = peekStack(tokenStack,2)) == NULL)
    {
      // Ziskame token
      tokenType = getToken(tmp_str);

      // Pushneme ho naspat
      pushStack(tokenStack,tokenType,NULL,tmp_str->str);

      // Ziskame ho naspat
      if ((item = peekStack(tokenStack,2)) == NULL)
      {
        // Ak ho neziskame uz je to chyba na 100%
        freeString(tmp_str);
        freeString(function_name);
        return INTERNAL_COMP_ERR;
      }
    }

    // Ak je to nieco ine ako funkcia chyba
    if (!(item->type == TT_IDENTIFIKATOR || item->type == TT_FUN_IDENTIFIKATOR || item->type == TT_FUN_RESERVED))
    {
      // Ak ho neziskame uz je to chyba na 100%
      freeString(tmp_str);
      freeString(function_name);
      return SYNTAX_ERR;
    }

    // ziskame index
    function_name_index = 2;
  }
  // Inac to moze byt identifikator alebo konstanta a musi byt definovana
  else if (item_assignment->type == TT_IDENTIFIKATOR || isConstant(item_assignment->type) || item_assignment->type == TT_LEFT_BRACKET || item_assignment->type == TT_EOL)
  {
    // Ziskame prvy item
    if ((item = peekStack(tokenStack,0)) == NULL)
    {
      // Ak ho neziskame uz je to chyba na 100%
      freeString(tmp_str);
      freeString(function_name);
      return INTERNAL_COMP_ERR;
    }
  }
  // Inac syntax error
  else
  {
    // Uvolnime string
    freeString(function_name);
    freeString(tmp_str);
    return SYNTAX_ERR;
  }

  // A nazov funkcie sa rovna itemu
  copyString(item->str->str,function_name);

  if (item_assignment == NULL)
  {
    freeString(function_name);
    freeString(tmp_str);
    return SYNTAX_ERR;
  }

  // Pokial sme nenasli koniec tak hladame
  if (item_assignment->type != TT_EOL)
  {
    // Prejdeme cely vyraz
    while (tokenType != TT_EOL)
    {
      // Ziskame token
      tokenType = getToken(tmp_str);

      // AK nam scanner vratil ERROR
      if (tokenType < 0)
      {
        freeString(function_name);
        freeString(tmp_str);
        return LEXICAL_ERR;
      }

      // Zobereme cely vyraz
      pushStack(tokenStack,tokenType,NULL,tmp_str->str);
    }
  }

  int _start = 0;

  // Ak je nazov_funkcie 0
  if (function_name_index == 0)
  {
    // Nastavime index od kedy treba prechadzat
    _start = function_name_index + 1;
  }
  // AK je nazov_funkcie 2
  else if (function_name_index == 2)
  {
    // Nastavime index od kedy treba prechadzat
    _start = function_name_index + 1;

    // prvy item musi byt 0
    if ((item = peekStack(tokenStack,0)) == NULL)
    {
      freeString(function_name);
      freeString(tmp_str);
      return INTERNAL_COMP_ERR;
    }

    // PRvy item musi byt identifikator
    if (item->type != TT_IDENTIFIKATOR)
    {
      freeString(function_name);
      freeString(tmp_str);
      // Vratime error
      return SYNTAX_ERR;
    }

    // prvy item musi byt 1
    if ((item = peekStack(tokenStack,1)) == NULL)
    {
      freeString(function_name);
      freeString(tmp_str);
      return INTERNAL_COMP_ERR;
    }

    // PRvy item musi byt identifikator
    if (item->type != TT_ASSIGNMENT)
    {
      freeString(function_name);
      freeString(tmp_str);
      // Vratime error
      return SYNTAX_ERR;
    }
  }
  // Inac je chyba
  else
  {
    freeString(function_name);
    freeString(tmp_str);
    // Vratime error
    return SYNTAX_ERR;
  }

  // Ziskame informacie o nej
  hItem = HashTableSearch(globalTable,function_name->str);

  // Skontrolujeme syntax a pocet premennych
  freeString(tmp_str);
  freeString(function_name);
  return functionSyntaxCheck(hItem,_start);
}

int ParseFunctionDefinition()
{
  struct Item *hsItem = NULL;

  int errCode = 0;

  // Clearneme stack ktory obsahuje DEF
  clearStack(tokenStack);

  // Vytvorime stringy
  String *tmp_str;
  initString(&tmp_str);

  int tokenType = -60;

  // Ziskame token
  tokenType = getToken(tmp_str);

  // AK nam scanner vratil ERROR
  if (tokenType < 0)
  {
    freeString(tmp_str);
    return LEXICAL_ERR;
  }

  // Pushneme ho na stack
  pushStack(tokenStack,tokenType,NULL,tmp_str->str);

  if (!(tokenType == TT_IDENTIFIKATOR || tokenType == TT_FUN_IDENTIFIKATOR))
  {
    // Chyba
    freeString(tmp_str);
    return SYNTAX_ERR;
  }

  // Malo by sa jednat o nazov funkcie tak skontrolujeme ci nahodou neni jedna z rezervovanych slov
  if (checkReserved(tmp_str->str) == 1)
  {
    // Ak je tak vratime chybu
    freeString(tmp_str);
    return SEMANTIC_PROG_ERR;
  }

  // Povieme ze sme vo currFunction
  copyString(tmp_str->str,currFunction);

  // Nasledujuci musi byt ( cize ziskame token
  tokenType = getToken(tmp_str);

  // AK nam scanner vratil ERROR
  if (tokenType < 0)
  {
    freeString(tmp_str);
    return LEXICAL_ERR;
  }

  // Nepushujeme ho na stack lebo neni potrebne

  // Zistime ci je (
  if (tokenType != TT_LEFT_BRACKET)
  {
    // Uvolnime a ukoncime
    freeString(tmp_str);
    return SYNTAX_ERR;
  }

  int bracket_count = 1;
  int var_count = 0;
  int var_before = 0;
  int comma_count = 0;

  // Robime pokial nenajdeme )
  while (1)
  {
    // Ziskame token
    tokenType = getToken(tmp_str);

    // AK nam scanner vratil ERROR
    if (tokenType < 0)
    {
      freeString(tmp_str);
      return LEXICAL_ERR;
    }

    // Pushneme ho na stack
    pushStack(tokenStack,tokenType,NULL,tmp_str->str);

    // Token moze byt iba identifkator
    if (tokenType == TT_IDENTIFIKATOR && var_before == 0)
    {
      // Ak je premenna ta ista ako
      if (strcmp(currFunction->str,tmp_str->str) == 0)
      {
        freeString(tmp_str);
        return SEMANTIC_PROG_ERR;
      }

      // Ziskame item
      hsItem = HashTableSearch(globalTable,tmp_str->str);

      // Ak ITEM neni NULL
      if (hsItem != NULL)
      {
        // Skontrolujeme ci sa nahodou nejedna o uz definovanu funkciu
        if (hsItem->data.tokenType == TT_FUN_IDENTIFIKATOR || hsItem->data.tokenType == TT_FUN_RESERVED)
        {
          // Ak hej je to chyba
          freeString(tmp_str);
          return SEMANTIC_PROG_ERR;
        }
      }

      var_count++;
      var_before++;

      continue;
    }
    // Token moze byt ciarka ale iba vtedy ked pred tym bola premenna
    else if (tokenType == TT_COMA && var_before == 1)
    {
      var_before--;
      comma_count++;
      // Popneme posledny item
      popStack(tokenStack);
      continue;
    }
    // Ak je to prava zatvorka koncime
    else if ((tokenType == TT_RIGHT_BRACKET && var_before == 1 && bracket_count == 1) || (comma_count == 0 && tokenType == TT_RIGHT_BRACKET && var_before == 0 && bracket_count == 1))
    {
      // Popneme posledny item
      popStack(tokenStack);

      // Ziskame token
      tokenType = getToken(tmp_str);

      // AK nam scanner vratil ERROR
      if (tokenType < 0)
      {
        freeString(tmp_str);
        return LEXICAL_ERR;
      }

      // Dalsi token musi byt EOL
      if (tokenType != TT_EOL)
      {
        freeString(tmp_str);
        return SYNTAX_ERR;
      }
      break;
    }
    // Inac syntax error
    else
    {
      freeString(tmp_str);
      return SYNTAX_ERR;
    }
  }

  // Vytvorime hashTable
  struct HashTable *hTable = initHashTable(HS_INIT_SIZE);

  // Pokusime sa insertnut funkciu
  if (HashTableInsert(globalTable,currFunction->str,TT_FUN_IDENTIFIKATOR,&(*hTable)) < 0)
  {
    // Jedna sa o reinsert chyba
    freeHashTable(hTable);
    freeString(tmp_str);
    return SEMANTIC_PROG_ERR;
  }

  // Item stacku
  TokenStackItem *item = NULL;

  // Zacneme insertovat premenne do hashtablu funkcie
  for (int i = 1; i < sizeStack(tokenStack); i++)
  {
    // Insertneme premennu
    item = peekStack(tokenStack,i);

    // Nasa chyba
    if (item == NULL)
    {
      // Uvolnime a vratime chybu
      freeString(tmp_str);
      return INTERNAL_COMP_ERR;
    }

    // Ak neni item typ IDENTIFATOR je to chyba
    if (item->type != TT_IDENTIFIKATOR)
    {
      // Uvolnime vratime chybu
      freeString(tmp_str);
      return SEMANTIC_PROG_ERR;
    }

    // Vlozime do tablu
    if (HashTableInsert(hTable,item->str->str,item->type,NULL) < 0)
    {
      // Uvolnime a ukoncime
      freeString(tmp_str);
      return SEMANTIC_PROG_ERR;
    }
  }

  // Clearneme string
  clearString(tmp_str);

  // Nakopirujeme obskok
  copyString("$function$",tmp_str);

  // Appendneme nazov funkcie
  appendString(currFunction->str,tmp_str);

  // Appendneme
  appendString("$continue$jump",tmp_str);

  // Urobime obskok
  appendInstruction(instructionList,IT_JUMP,tmp_str->str,NULL,NULL);

  // Vygenerujeme HEADER funkcie
  if ((errCode = generateFunctionDefinition(instructionList,hTable,tokenStack)) != 0)
  {
    // Uvolnime a ukoncime
    freeString(tmp_str);
    return errCode;
  }

  // FUNCTION body
  while (tokenType != TT_END)
  {
    // A skontrolujeme co sme dostali
    switch (tokenType)
    {
      // Moze byt while
      case TT_WHILE:
        if ((errCode = ParseWhile()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;
      // Dalsie IF
      case TT_IF:
        if ((errCode = ParseIf()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_IDENTIFIKATOR:
      case TT_INTEGER:
      case TT_FLOAT:
      case TT_EXPONENT:
      case TT_STRING:
      case TT_LEFT_BRACKET:

        // Tuto funkciu volame ked sme si isty ze tam assignment neni
        if ((errCode = ParseExpression()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_FUN_IDENTIFIKATOR:
        if ((errCode = ParseFunctionCall()) != 0)
        {
          freeString(tmp_str);
          return errCode;
        }
      break;

      case TT_EOL:
        // NIC NEROBIME
      break;

      default:

        freeString(tmp_str);
        if (tokenType < 0)
        {
          return LEXICAL_ERR;
        }
        return SYNTAX_ERR;
      break;
    }

    // Vycistime string
    clearString(tmp_str);

    // Vycistime stack
    clearStack(tokenStack);

    // Ziskame token
    tokenType = getToken(tmp_str);

    // AK nam scanner vratil ERROR
    if (tokenType < 0)
    {
      freeString(tmp_str);
      return LEXICAL_ERR;
    }

    // Pushneme ho na stack
    pushStack(tokenStack,tokenType,NULL,tmp_str->str);
  }

  // Ked najdeme END musi nasledovat EOL
  if ((tokenType = getToken(tmp_str)) != TT_EOL)
  {
    freeString(tmp_str);
    return SYNTAX_ERR;
  }

  // Vygenerujeme Konecne instrukcie popframe
  appendInstruction(instructionList,IT_POPFRAME,NULL,NULL,NULL);

  // Vygenerujeme RETURN
  appendInstruction(instructionList,IT_RETURN,NULL,NULL,NULL);

  // Clearneme string
  clearString(tmp_str);

  // Nakopirujeme obskok
  copyString("$function$",tmp_str);

  // Appendneme nazov funkcie
  appendString(currFunction->str,tmp_str);

  // Appendneme
  appendString("$continue$jump",tmp_str);

  // Urobime obskok
  appendInstruction(instructionList,IT_LABEL,tmp_str->str,NULL,NULL);

  // Inicializujeme funkciu
  inicializeFun(globalTable,currFunction->str,var_count);

  // Clearneme nazov funkcie
  clearString(currFunction);

  // Uvolnime a vratime OK
  freeString(tmp_str);
  return 0;
}

// Parsne expression
int ParseExpression()
{
  // Error
  int errCode = 0;

  // Token holder
  int tokenType;
  int tokenTypePrev;
  String *tmp_str_prev;
  initString(&tmp_str_prev);

  TokenStackItem *assignment_itm = NULL;

  // String pre nazov
  String *tmp_str;
  initString(&tmp_str);

  // Ziskame posledny item zo stacku
  if ((assignment_itm = peekStack(tokenStack,sizeStack(tokenStack) - 1)) != NULL)
  {
    // Nastavime predchadzajuci
    tokenTypePrev = assignment_itm->type;
    copyString(assignment_itm->str->str,tmp_str_prev);
  }

  // Nacitame 2 item zo stacku ak je =
  if ((assignment_itm = peekStack(tokenStack,1)) != NULL)
  {
    // Ak item je = tak nebude NULL
    if (assignment_itm->type != TT_ASSIGNMENT)
    {
      assignment_itm = NULL;
    }
    // Inac nic nerobime
  }

  // Ziskame token
  tokenType = getToken(tmp_str);

  if (tokenType < 0)
  {
    freeString(tmp_str);
    freeString(tmp_str_prev);
    return LEXICAL_ERR;
  }

  // Nesmie sa tu vyskytnut EOF
  if (tokenType == TT_EOF)
  {
    freeString(tmp_str);
    freeString(tmp_str_prev);
    return SYNTAX_ERR;
  }

  // Pusheneme token na stack
  pushStack(tokenStack,tokenType,NULL,tmp_str->str);

  // Ak je predchadzajuci (
  if (tokenTypePrev == TT_LEFT_BRACKET)
  {
    // Nasledujuce mozu byt bud konstanta alebo identifikator
    if (isConstant(tokenType) || tokenType == TT_IDENTIFIKATOR || tokenType == TT_LEFT_BRACKET)
    {
      // Zavolame precedencnu analyzu
      if ((errCode = precedenceAnalysisParsing(tokenStack,ASSIGNMENT_MOD)) != 0)
      {
        // Uvolnime string
        freeString(tmp_str);
        freeString(tmp_str_prev);
        // Vratime errCode
        return errCode;
      }

      // Vycistime string predtym nez to tam predame
      clearString(tmp_str);

      struct HashTable *hTable = NULL;

      if (((hTable = workingHashTable())) == NULL)
      {
        freeString(tmp_str_prev);
        freeString(tmp_str);
        return INTERNAL_COMP_ERR;
      }


      // Inac mozme generovat expression
      if ((errCode = generateExpression(instructionList,hTable,((struct binStruct *)peekStack(tokenStack,0)->ptr),tmp_str,&var_counter)) != 0)
      {
        // Uvolnime strukturu
        freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
        // Uvolnime string
        freeString(tmp_str);
        freeString(tmp_str_prev);
        // Vratime errCode
        return errCode;
      }

      // Vygenerujeme retval
      errCode = createRetval(tmp_str->str,TT_IDENTIFIKATOR);

      // Uvolnime strukturu
      freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
      // Uvolnime string
      freeString(tmp_str);
      freeString(tmp_str_prev);
      // Vratime 0
      return errCode;
    }
    // Inac syntax error
    else
    {
      freeString(tmp_str);
      freeString(tmp_str_prev);
      return SYNTAX_ERR;
    }
  }
  // Ak predchadzajuci je konstanta alebo identifikator
  else if (isConstant(tokenTypePrev) || tokenTypePrev == TT_IDENTIFIKATOR)
  {
    // Mozme mat EOL
    if (tokenType == TT_EOL)
    {
      // Ak mame treba skontrolovat ci assignment_itm je NULL a pocet itemov je 2
      // CIze mame len KONSTANT|IDENTIFIER EOL
      if (assignment_itm == NULL && (sizeStack(tokenStack) == 2))
      {
        //  Volanie funkcie moze nastat ak je token IDENTIFIKATOR
        if (tokenTypePrev == TT_IDENTIFIKATOR)
        {
          // Vtedy skontrolujeme ci sa nahodou nejedna o funkciu
          if (checkIdentifikator(tmp_str_prev->str) != TT_IDENTIFIKATOR)
          {
            // Uvolnime stringy
            freeString(tmp_str);
            freeString(tmp_str_prev);

            // Moze sa jednat o volanie funkcie cize to tak bereme
            return ParseFunctionCall();
          }
        }

        // Ziskame prvy item
        TokenStackItem *tmp_itm = peekStack(tokenStack,0);

        // Vytvorime %RETVAL
        errCode = createRetval(tmp_itm->str->str,tmp_itm->type);

        // Uvolnime string
        freeString(tmp_str);
        freeString(tmp_str_prev);
        // Vratime errCode
        return errCode;
      }
      // Inac sa jedna o priradenie ale aj tak volame jardu
      else
      {
        // Zavolame precedencnu analyzu
        if ((errCode = precedenceAnalysisParsing(tokenStack,ASSIGNMENT_MOD)) != 0)
        {
          // Uvolnime string
          freeString(tmp_str);
          freeString(tmp_str_prev);
          // Vratime errCode
          return errCode;
        }

        // Vycistime string predtym nez to tam predame
        clearString(tmp_str);

        struct HashTable *hTable = NULL;

        if (((hTable = workingHashTable())) == NULL)
        {
          // Uvolnime strukturu
          freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
          // Uvolnime string
          freeString(tmp_str);
          freeString(tmp_str_prev);
          return INTERNAL_COMP_ERR;
        }

        // Inac mozme generovat expression
        if ((errCode = generateExpression(instructionList,hTable,((struct binStruct *)peekStack(tokenStack,0)->ptr),tmp_str,&var_counter)) != 0)
        {
          // Uvolnime strukturu
          freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
          // Uvolnime string
          freeString(tmp_str);
          freeString(tmp_str_prev);
          // Vratime errCode
          return errCode;
        }

        // Vygenerujeme retval
        errCode = createRetval(tmp_str->str,TT_IDENTIFIKATOR);

        // Uvolnime strukturu
        freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
        // Uvolnime string
        freeString(tmp_str);
        freeString(tmp_str_prev);
        // Vratime 0
        return errCode;
      }
    }
    // Inac moze byt symbol alebo binarka
    else if (checkMathOperators(tokenType))
    {
      // Zavolame precedencnu analyzu
      if ((errCode = precedenceAnalysisParsing(tokenStack,ASSIGNMENT_MOD)) != 0)
      {
        // Uvolnime string
        freeString(tmp_str);
        freeString(tmp_str_prev);
        // Vratime errCode
        return errCode;
      }

      // Vycistime string predtym nez to tam predame
      clearString(tmp_str);

      struct HashTable *hTable = NULL;

      if (((hTable = workingHashTable())) == NULL)
      {
        // Uvolnime strukturu
        freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
        // Uvolnime string
        freeString(tmp_str);
        freeString(tmp_str_prev);
        return INTERNAL_COMP_ERR;
      }

      // Inac mozme generovat expression
      if ((errCode = generateExpression(instructionList,hTable,((struct binStruct *)peekStack(tokenStack,0)->ptr),tmp_str,&var_counter)) != 0)
      {
        // Uvolnime strukturu
        freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
        // Uvolnime string
        freeString(tmp_str);
        freeString(tmp_str_prev);
        // Vratime errCode
        return errCode;
      }

      // Vygenerujeme retval
      errCode = createRetval(tmp_str->str,TT_IDENTIFIKATOR);

      // Uvolnime strukturu
      freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
      // Uvolnime string
      freeString(tmp_str);
      freeString(tmp_str_prev);
      // Vratime 0
      return errCode;
    }
    // Dalej moze byt assignment ale iba vtedy ak predchadzajuci je IDENTIFIER a assignment_itm = NULL
    else if (assignment_itm == NULL && tokenTypePrev == TT_IDENTIFIKATOR && tokenType == TT_ASSIGNMENT)
    {
      // Pred tym nez zavolame musime ziskat dalsi item
      tokenType = getToken(tmp_str);

      // AK nam scanner vratil ERROR
      if (tokenType < 0)
      {
        freeString(tmp_str);
        freeString(tmp_str_prev);
        return LEXICAL_ERR;
      }

      // Pushneme item
      pushStack(tokenStack,tokenType,NULL,tmp_str->str);

      // Skontrolujeme ci sa jedna o identifikator
      if (tokenType == TT_IDENTIFIKATOR)
      {
        // Item
        TokenStackItem *tmp_item = NULL;

        // Ziskame prvy item zo stacku
        tmp_item = peekStack(tokenStack,0);

        // Ak je item NULL
        if (tmp_item == NULL)
        {
          // Uvolnime stringy
          freeString(tmp_str);
          freeString(tmp_str_prev);
          // Chyba ktora nemala nastat
          return INTERNAL_COMP_ERR;
        }

        // Ak sa rovna prvy item a tento 3 item
        if (strcmp(tmp_str->str,tmp_item->str->str) == 0)
        {
          // Jedna sa o definiciu premennej tou istou premennou
          // Zavolame precedencnu analyzu
          if ((errCode = precedenceAnalysisParsing(tokenStack,ASSIGNMENT_MOD)) != 0)
          {

            // Uvolnime string
            freeString(tmp_str);
            freeString(tmp_str_prev);
            // Vratime errCode
            return errCode;
          }

          // Vycistime string predtym nez to tam predame
          clearString(tmp_str);

          struct HashTable *hTable = NULL;

          if (((hTable = workingHashTable())) == NULL)
          {
            // Uvolnime strukturu
            freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
            // Uvolnime string
            freeString(tmp_str);
            freeString(tmp_str_prev);
            return INTERNAL_COMP_ERR;
          }

          // Inac mozme generovat expression
          if ((errCode = generateExpression(instructionList,hTable,((struct binStruct *)peekStack(tokenStack,0)->ptr),tmp_str,&var_counter)) != 0)
          {
            // Uvolnime strukturu
            freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
            // Uvolnime string
            freeString(tmp_str);
            freeString(tmp_str_prev);
            // Vratime errCode
            return errCode;
          }

          // Vygenerujeme retval
          errCode = createRetval(tmp_str->str,TT_IDENTIFIKATOR);

          // Uvolnime strukturu
          freeBinStruct(((struct binStruct *)peekStack(tokenStack,0)->ptr));
          // Uvolnime string
          freeString(tmp_str);
          freeString(tmp_str_prev);
          // Vratime 0
          return 0;
        }
      }

      // Mame identifkator =
      // Cize dalsi item ak je identifkator  moze sa jedna o function call
      if (tokenType == TT_IDENTIFIKATOR || tokenType == TT_FUN_IDENTIFIKATOR)
      {
        // Vtedy skontrolujeme ci sa nahodou nejedna o funkciu
        if (checkIdentifikator(tmp_str->str) != TT_IDENTIFIKATOR)
        {
          // Uvolnime stringy
          freeString(tmp_str);
          freeString(tmp_str_prev);

          // Moze sa jednat o volanie funkcie cize to tak bereme
          return ParseFunctionCall();
        }
      }

      // Pred tym uvolnime string
      freeString(tmp_str);
      freeString(tmp_str_prev);
      // Vtedy volame znova tuto funkciu
      return ParseExpression();
    }
    // Dalej moze byt konstanta alebo identifikator co znaci volanie funkcie
    else if (isConstant(tokenType) || tokenType == TT_IDENTIFIKATOR || tokenType == TT_LEFT_BRACKET)
    {
      // Zavolame parsovanie function callu
      errCode = ParseFunctionCall();
      // Uvolnime string
      freeString(tmp_str);
      freeString(tmp_str_prev);
      // Vratime errCode
      return errCode;
    }
    // Inac syntax error
    else
    {
      freeString(tmp_str);
      freeString(tmp_str_prev);
      return SYNTAX_ERR;
    }
  }
  // Inac syntax error
  else
  {
    // Uvolnime string
    freeString(tmp_str);
    freeString(tmp_str_prev);
    // Vratime syntax error
    return SYNTAX_ERR;
  }
}

/*
  Funckia ktora bude rozdelovat ktora funkcia ide parsovat
*/
int Program()
{
  int errCode = 0;
  String *tokenStr;
  initString(&tokenStr);

  int tokenType = 0;

  // Endless loop pokial nenajdeme error alebo koniec suboru
  while (1)
  {
    // Ziskame token zo scanneru
    tokenType = getToken(tokenStr);

    // Vyprazdnime stack
    clearStack(tokenStack);

    // Pridame prvy item do stacku
    pushStack(tokenStack,tokenType,NULL,tokenStr->str);

    // Zistime co to je za token
    switch (tokenType)
    {
      case TT_WHILE:
        if ((errCode = ParseWhile()) != 0)
        {
          freeString(tokenStr);
          return errCode;
        }
      break;

      case TT_IF:
        if ((errCode = ParseIf()) != 0)
        {
          freeString(tokenStr);
          return errCode;
        }
      break;

      case TT_DEF:
        if ((errCode = ParseFunctionDefinition()) != 0)
        {
          freeString(tokenStr);
          return errCode;
        }
      break;

      case TT_IDENTIFIKATOR:
      case TT_INTEGER:
      case TT_FLOAT:
      case TT_EXPONENT:
      case TT_STRING:
      case TT_LEFT_BRACKET:

        // Tuto funkciu volame ked sme si isty ze tam assignment neni
        if ((errCode = ParseExpression()) != 0)
        {
          freeString(tokenStr);
          return errCode;
        }
      break;

      case TT_FUN_IDENTIFIKATOR:
        if ((errCode = ParseFunctionCall()) != 0)
        {
          freeString(tokenStr);
          return errCode;
        }
      break;

      case TT_EOL:
        // NIC NEROBIME
      break;

      case TT_EOF:
        freeString(tokenStr);
        return 0;
      break;

      default:

        freeString(tokenStr);
        if (tokenType < 0)
        {
          return LEXICAL_ERR;
        }
        return SYNTAX_ERR;
      break;
    }
  }
}

int Parse()
{
  int err = 0;
  // Parsneme program
  if ((err = Program()) != 0)
  {
    return err;
  }

  // Skontrolujeme funkcie
  if ((err = checkFunctionCalls()) != 0)
  {
    return err;
  }

  // Vygenerujeme instrukcie
  if ((err = generateCode(instructionList)) != 0)
  {
    return err;
  }

  // Vratime naspat err
  return 0;
}

void freeParser()
{
  // Uvolnime globalTable ak nie je NULL
  if (globalTable != NULL)
  {
    freeHashTable(globalTable);
  }

  // Uvolnime tokenStack
  if (tokenStack != NULL)
  {
    freeStack(tokenStack);
  }

  // Uvolnime stack
  if (scopeStack != NULL)
  {
    freeStack(scopeStack);
  }

  // Uvolnime stack
  if (function_call != NULL)
  {
    freeStack(function_call);
  }

  // Uvolnime instrukcny list
  if (instructionList != NULL)
  {
    freeInstructionStruct(instructionList);
  }

  // Uvolnime string
  if (currFunction != NULL)
  {
    freeString(currFunction);
  }
}
