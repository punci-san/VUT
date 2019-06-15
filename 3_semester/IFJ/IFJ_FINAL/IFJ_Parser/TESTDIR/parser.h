/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
 *
 **/

#ifndef PARSER_HEADER
#define PARSER_HEADER

#include "scanner.h"
#include "generate_code.h"

#define TT_FUN_RETVAL 101

int initParser();
int Parse();
void freeParser();

int createVarDef(char *str);
int createRetval(char *str,int type);
void appendFunctionCall(char *fun_name,int var_number);
int isConstant(TokenType type);

#endif
