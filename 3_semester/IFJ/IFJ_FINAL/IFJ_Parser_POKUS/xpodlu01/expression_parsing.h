/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Jaromír Hradil <xhradi15@stud.fit.vutbr.cz>
 *
 **/

#include "stack.h"
#include "scanner.h"
#include "str.h"
#include "err_code.h"
#include "bin_struct.h"
#define NON_TERM_BIN_ITEM 300
#define NON_TERM_BIN_NODE 400


///vyctovy typ na vyhledavani indexu v tabulce
typedef enum ptElements
{
   PT_ADD,  // +
   PT_SUB,  // -
   PT_MUL,  // *
   PT_DIV,  // /
   PT_LP,   // (
   PT_RP,   // )
   PT_L,    // <
   PT_G,    // >
   PT_LE,   // <=
   PT_GE,   // >=
   PT_EQ,   // =
   PT_LEQ,  // ==
   PT_NEQ,  // !=
   PT_ID,   // id
   PT_END,   // $
   PT_ERROR    // jine, nez co tam ma byt

}ptElements;

///Mod, podle ktereho se jede
typedef enum stateMods
{
 WHILE_MOD,
 IF_MOD,
 ASSIGNMENT_MOD,
}stateMods;


///precedenci tabulka pro vyber akce

 static const char priorityTable [15][15] =



{
///   +,  -   *,  / , ( , ) , <,  >,  <=, >=, =,  == !=, id,  $
    {'>','>','<','<','<','>','>','>','>','>','!','>','>','<','>'}, /// +
    {'>','>','<','<','<','>','>','>','>','>','!','>','>','<','>'}, /// -
    {'>','>','>','>','<','>','>','>','>','>','!','>','>','<','>'}, /// *
    {'>','>','>','>','<','>','>','>','>','>','!','>','>','<','>'}, /// /
    {'<','<','<','<','<','=','<','<','<','<','<','<','<','<','!'}, /// (
    {'>','>','>','>','!','>','>','>','>','>','!','>','>','!','>'}, /// )
    {'<','<','<','<','<','>','!','!','!','!','!','!','!','<','>'}, /// <
    {'<','<','<','<','<','>','!','!','!','!','!','!','!','<','>'}, /// >
    {'<','<','<','<','<','>','!','!','!','!','!','!','!','<','>'}, /// <=
    {'<','<','<','<','<','>','!','!','!','!','!','!','!','<','>'}, /// >=
    {'<','<','<','<','<','>','!','!','!','!','!','!','!','<','>'}, /// =
    {'<','<','<','<','<','>','!','!','!','!','!','!','!','<','>'}, /// ==
    {'<','<','<','<','<','>','!','!','!','!','!','!','!','<','>'}, /// !=
    {'>','>','>','>','!','>','>','>','>','>','>','>','>','!','>'}, /// id
    {'<','<','<','<','<','!','<','<','<','<','<','<','<','<','!'}  /// $

// <  ---> shift
// >  ---> reduce
// !  ---> error
// =  ---> bracket reduce
 };


int precedenceAnalysisParsing (struct TokenStack *stack,int state);
int findTopOfStack(TokenStack * stack);

stateMods stateModsAssignment(int state);
int reduceID(TokenStack * stack);

int ruleReduce(TokenStack * stack,binStruct * nodeStackn, int index);
int assignmentStackSolver(struct TokenStack * stack, struct TokenStack * tmp_sort_stack, struct binStruct * derivative_stack);

ptElements operatorTypeAssignment(int type_of_token);
char actionByPrecedenceTable(int row, int column);
void freeResources(struct TokenStack * stack,struct binStruct * binaryStack);
