#include <stdio.h>
#include <stdlib.h>
#include "expression_parsing.h"
#define SUCCESS  0
#define SUCCESS_ASSIGNMENT 6




int precedenceAnalysisParsing (struct TokenStack *stack,int state)
{
    ///nastaveni modu
    stateMods mode_identifier = stateModsAssignment(state);


    ///kontrola parametru
    if( stack == NULL)
    {
        return INTERNAL_COMP_ERR;
    }
    ///deklarace zasobniku
    struct TokenStack * tmp_sort_stack = createStack();
    struct binStruct  * derivative_stack = createBinStruct();

    ///Pokud nevysla alokace
    if(!tmp_sort_stack || !derivative_stack )
    {
        freeResources(tmp_sort_stack,derivative_stack);
        return INTERNAL_COMP_ERR;

    }

    ///Volba konecneho symbolu podle modu

    if(mode_identifier == WHILE_MOD || mode_identifier == IF_MOD)
    {
        ///$ na pocatek zasobniku
        pushStack(tmp_sort_stack,TT_EOL,NULL,"");
    }

    /// zde se zkontroluje presune a zredukuji polozky ze stacku, pokud se jedna o assignment nebo neprirazeny expression
    else if(mode_identifier == ASSIGNMENT_MOD)

    {
        int error = assignmentStackSolver(stack,tmp_sort_stack,derivative_stack);
        ///Pokud nastal problem s resenim ze stacku z parametru
        if(error == SYNTAX_ERR)
        {
            return SYNTAX_ERR;
        }

        else if(error == INTERNAL_COMP_ERR)
        {
            return INTERNAL_COMP_ERR;
        }
         ///Zde pokud jsme zpracovali jiz vse, co bylo spravne napsane a ohrazene spravnym endmarkerem, tak vracime uspech
        else if(error == SUCCESS_ASSIGNMENT )
        {
          return SUCCESS;
        }


    }




    String * token_value = NULL;

    initString(&token_value);


    ///Ukoncovaci podminka pro cyklus
    unsigned is_over = 0;

    int type_of_token = 0 ;

    ///Pokud jsme volali assignmentStackSolver, musime si overit, ze na vrcholu zasobniku nneni prava zavorka
    if(mode_identifier == ASSIGNMENT_MOD)
    {
        struct TokenStackItem * item_right_bracket = peekStack(tmp_sort_stack,findTopOfStack(tmp_sort_stack));

        ///Pokud je, tak predame type_of_token jeji hodnoty, jako by ji nacetl, aby mohla spravne pote probehnout redukce
        if(item_right_bracket->type == TT_RIGHT_BRACKET)
        {
            type_of_token = TT_RIGHT_BRACKET;
            appendString(")",token_value);
            popIndex(tmp_sort_stack,findTopOfStack(tmp_sort_stack));
        }
        //jinak cteme ze vstupu
        else
        {
            type_of_token = getToken(token_value);
        }


    }

    else
    {
        type_of_token = getToken(token_value);
    }

    do
    {
        ///Pokud getToken vrati lexikalni chybu, tak se vse uvolni a skonci se
        if(type_of_token == -1)
        {

            freeResources(tmp_sort_stack,derivative_stack);
            freeString(token_value);
            return LEXICAL_ERR;
        }

        ///Pokud je tam neco jineho nez tam melo byt
        else if(operatorTypeAssignment(type_of_token) == PT_ERROR )
        {
            freeResources(tmp_sort_stack,derivative_stack);
            freeString(token_value);
            return SYNTAX_ERR;
        }

        ///Pokud jsme ve while nebo if modu, musime kontrolovat, ze tady neni "="
        if((mode_identifier == WHILE_MOD || mode_identifier == IF_MOD) && type_of_token == TT_ASSIGNMENT )
        {
              ///Jinak vracime error
            freeResources(tmp_sort_stack,derivative_stack);
            freeString(token_value);
            return SYNTAX_ERR;
        }


        ///Index nejblizsiho terminalu od vrcholu zasobniku
        int top_terminal_index = findTopOfStack(tmp_sort_stack);
        TokenStackItem * item_terminal = peekStack(tmp_sort_stack,top_terminal_index);


        ///Indexy pro hledani v tabulce prefenci
        ptElements index1 = operatorTypeAssignment(item_terminal->type);
        ptElements index2 = operatorTypeAssignment(type_of_token);

        ///pravidlo
        char action = actionByPrecedenceTable(index1,index2);


        switch(action)
        {

        case '<':
            ///pouze pushne na stack dalsi hodnotu
            pushStack(tmp_sort_stack,type_of_token,NULL,token_value->str);
            type_of_token = getToken(token_value);
            break;

        case '>':
            ///zde provadi redukci podle pravidel
            if(index1 == PT_ID && top_terminal_index == sizeStack(tmp_sort_stack) - 1)
            {
                ///Pokud se nepovedla redukce ID, vracime chybu prekladace
                if(!reduceID(tmp_sort_stack))
                {
                    freeResources(tmp_sort_stack,derivative_stack);
                    freeString(token_value);
                    return INTERNAL_COMP_ERR;
                }
            }
            else
            {  
               int ruleResult = ruleReduce(tmp_sort_stack,derivative_stack,top_terminal_index);
               ///Pokud se nepovedla redukce podle pravidel, vraci se syntakticka chyba
                if (ruleResult == -SYNTAX_ERR)
                {
                    freeResources(tmp_sort_stack,derivative_stack);
                    freeString(token_value);
                    return SYNTAX_ERR;
                }
                else if (ruleResult == -INTERNAL_COMP_ERR)
                {
                    freeResources(tmp_sort_stack,derivative_stack);
                    freeString(token_value);
                    return INTERNAL_COMP_ERR;
                }
             }
            break;

        case '=':


            //Pokude mezi zavorkami neni jen 1 neterminal, je nekde chyba
            if(((sizeStack(tmp_sort_stack)-1)-top_terminal_index) != 1 )
            {
                freeResources(tmp_sort_stack,derivative_stack);
                freeString(token_value);
                return SYNTAX_ERR;
            }
            else
            {

                type_of_token = getToken(token_value);

                ///Znova se musi provest kontrola nacitaneho prvku
                if(type_of_token == -1)
                {
                    freeResources(tmp_sort_stack,derivative_stack);
                    freeString(token_value);
                    return LEXICAL_ERR;
                }

                ///Pokud je tam neco jineho nez tam melo byt
                else if(operatorTypeAssignment(type_of_token) == PT_ERROR )
                {
                    freeResources(tmp_sort_stack,derivative_stack);
                    freeString(token_value);
                    return SYNTAX_ERR;
                }

                ///Koukame, jake pravidlo plati pro prvek za zavorkou
                action = actionByPrecedenceTable(PT_RP,operatorTypeAssignment(type_of_token));

                ///Pokud nelze, konci se
                ///Staci pouze neuspech protoze prava zavorka pak uz muze pouze shiftovat
                if(action == '!')
                {
                    freeResources(tmp_sort_stack,derivative_stack);
                    freeString(token_value);
                    return SYNTAX_ERR;
                }


                ///Probiha redukce
                popIndex(tmp_sort_stack, top_terminal_index);

            }

            break;

        default:
                  ///Pokud je na topu i vstupu endmarker a mezi nimi je jeden neterminal
            if(index1 == PT_END && index2 == PT_END && (sizeStack(tmp_sort_stack) - top_terminal_index) == 2 )
            {

                ///Kontrola spravnych endmarkeru
                if(mode_identifier == WHILE_MOD && type_of_token == TT_DO)
                {
                    //Uspech syntakticke analyzy
                    is_over = 1;
                    break;
                }
                else  if(mode_identifier == IF_MOD && type_of_token == TT_THEN)
                {
                    //Uspech syntakticke analyzy
                    is_over = 1;
                    break;
                }

                else  if(mode_identifier == ASSIGNMENT_MOD  && type_of_token == TT_EOL)
                {
                    //Uspech syntakticke analyzy
                    is_over = 1;
                    break;
                }
                else
                {
                    freeResources(tmp_sort_stack,derivative_stack);
                    freeString(token_value);
                    return SYNTAX_ERR;
                }



            }
            else
            {

                freeResources(tmp_sort_stack,derivative_stack);
                freeString(token_value);
                return SYNTAX_ERR;
            }


        }

    }
    while(is_over != 1);

    ///uvolneni stringu
    freeString(token_value);



    TokenStackItem * lastCheck = peekStack(tmp_sort_stack,sizeStack(tmp_sort_stack)-1);

    if(lastCheck->type == NON_TERM_BIN_ITEM)
    {

        int lastControl =  addBinNode(derivative_stack,(struct binItem *)lastCheck->ptr,NULL,NULL,NT_ALONE);

        if(lastControl == -1)
          {
              freeResources(tmp_sort_stack,derivative_stack);
              return INTERNAL_COMP_ERR;       
          }


    }

     ///Temporary stack uvolnime a parametrovy procistime a vratime do nej
    freeStack(tmp_sort_stack);
    clearStack(stack);


    pushStack(stack,DERIVATIVE_TREE,(struct binStruct *)derivative_stack,NULL);


    return SUCCESS;



}

//-------------------------------------------------------------------------------------------------------------------------

int assignmentStackSolver(struct TokenStack * stack, struct TokenStack * tmp_sort_stack, struct binStruct * derivative_stack)
{

    ///Ukazatele na polozky ze zasobniku
    struct TokenStackItem * item_tmp_stack = NULL;
    struct TokenStackItem * item_param_stack = NULL;



    pushStack(tmp_sort_stack,TT_EOL,NULL,"");

    int counter = 0;


///Kontrolni pointer na prvek, jestli na konci stacku bude endmarkers
    TokenStackItem * lastAssignmentCheck = NULL;

    //Cyklus probiha tak dlouho, dokud, se nedojede na posledni index
    while(counter != sizeStack(stack))
    {



        item_tmp_stack = peekStack(tmp_sort_stack,findTopOfStack(tmp_sort_stack));
        item_param_stack = peekStack(stack,counter);

        ptElements index_tmp_item = operatorTypeAssignment(item_tmp_stack->type);
        ptElements index_param_item = operatorTypeAssignment(item_param_stack->type);



        if(index_param_item == PT_ERROR || index_tmp_item == PT_ERROR)
        {
            freeResources(tmp_sort_stack,derivative_stack);
            return SYNTAX_ERR;
        }


        char action2 = actionByPrecedenceTable(index_tmp_item,index_param_item);



        switch(action2)
        {

        case '<':

            pushStack(tmp_sort_stack,item_param_stack->type,NULL,item_param_stack->str->str);
            counter++;
            break;

        case '>':
            ///Pokud jsme na vrcholu a jedna se o typ ID, tak ho redukujeme
            if(index_tmp_item == PT_ID && findTopOfStack(tmp_sort_stack) == sizeStack(tmp_sort_stack) - 1)
            {
                ///Pokud se redukce ID nevydarila, konci se
                if(!reduceID(tmp_sort_stack))
                {
                    freeResources(tmp_sort_stack,derivative_stack);
                    return INTERNAL_COMP_ERR;
                }
            }
            else
            {
                ///Jinak se zkusi redukce podle pravidel, ta se teze ukonci, pokud nebudou splneny podminky ve funkci
  
              int assignmentRuleResult = ruleReduce(tmp_sort_stack,derivative_stack,findTopOfStack(tmp_sort_stack));

              if (assignmentRuleResult == -SYNTAX_ERR)
                {
                    freeResources(tmp_sort_stack,derivative_stack);
                    return SYNTAX_ERR;
                }

             else if (assignmentRuleResult == -INTERNAL_COMP_ERR)
                {
                    freeResources(tmp_sort_stack,derivative_stack);
                    return INTERNAL_COMP_ERR;
                }
            }
            break;
        case '=':

            //Pokude mezi zavorkami neni jen 1 neterminal, je nekde chyba
            if(((sizeStack(tmp_sort_stack)-1)- findTopOfStack(tmp_sort_stack)) != 1 )
            {
                freeResources(tmp_sort_stack,derivative_stack);
                return SYNTAX_ERR;
            }
            else
            {
                counter++;
                ///Pokud jsme na konci zasobniku, ulozime si zavorku a pokracujeme v hlavni funkci
                if(counter == sizeStack(stack))
                {
                    pushStack(tmp_sort_stack,item_param_stack->type,NULL,item_param_stack->str->str);
                    break;
                }
                else
                {
                    ///Vybira akci, aby vedel, zda lze zredukovat zavorky

                    item_param_stack = peekStack(stack,counter);
                    index_param_item = operatorTypeAssignment(item_param_stack->type);


                    action2 = actionByPrecedenceTable(PT_RP,index_param_item);

                    ///Pokud nelze, konci se
                    if(action2 == '!')
                    {
                        freeResources(tmp_sort_stack,derivative_stack);
                        return SYNTAX_ERR;
                    }

                    ///Probiha redukce
                    popIndex(tmp_sort_stack, findTopOfStack(tmp_sort_stack));

                }
            }
            break;
        ///Pokud nebylo nalezeno spravne pravidlo, tak se konci
        default:

            //Pokud je na vstupu i vrcholu zasobniku endmarker a mezi nimi je jeden neterminal
            if((index_tmp_item == PT_END && index_param_item == PT_END) && ((sizeStack(tmp_sort_stack) - findTopOfStack(tmp_sort_stack)) == 2) )
            {
                ///Kontrola spravnych endmarkeru a spravneho indexu
                if(item_param_stack->type == TT_EOL && counter == sizeStack(stack)-1)
                {
                    //Uspech syntakticke analyzy

                    lastAssignmentCheck = peekStack(tmp_sort_stack,sizeStack(tmp_sort_stack)-1);

                     ///Pokud je neterminalem binItem, tak z nej vytvorime binNode
                    if(lastAssignmentCheck->type == NON_TERM_BIN_ITEM)
                    {
                       int lastControl2 = addBinNode(derivative_stack,(struct binItem *)lastAssignmentCheck->ptr,NULL,NULL,NT_ALONE);
                          
                       if(lastControl2 == -1)
                       {
                         freeResources(tmp_sort_stack,derivative_stack);
                         return INTERNAL_COMP_ERR;       
                        }
 

                    }

                      ///Uvolnovani zdroju, neuvolnujeme binStack, protoze ho bude zapotrebi dale
                    freeStack(tmp_sort_stack);
                    clearStack(stack);

                     ///Pushneme na parametrovy zasobnik item s ukazatelem na strukturu
                    pushStack(stack,DERIVATIVE_TREE,(struct binStruct *)derivative_stack,NULL);

                   ///Vracime hodnotu, ze dale v hlavni funkci uz nepokracujeme
                    return SUCCESS_ASSIGNMENT;
                }
                else
                {

                    freeResources(tmp_sort_stack,derivative_stack);
                    return SYNTAX_ERR;
                }

            }

            else
            {
                freeResources(tmp_sort_stack,derivative_stack);
                return SYNTAX_ERR;
            }
        }
    }
    return SUCCESS;
}


int findTopOfStack(struct TokenStack * stack)
{

    TokenStackItem * stackItem = NULL;

    ///Jedeme od vrcholu zasobniku
    for(int i = sizeStack(stack) - 1; i >= 0; i--)
    {
        //nakoukneme na prvek
        stackItem = peekStack(stack,i);

        //jestli to je neterminal, jedeme dal
        if(stackItem->type == NON_TERM_BIN_ITEM || stackItem->type == NON_TERM_BIN_NODE)
        {
            continue;
        }
        ///jinak vracime index prvniho terminalu
        else
        {
            return i;
        }
    }

    return 0;


}


//Urcuje, jak bude fce precedenceAnalysisParsing postupovat v dalsich krocich
stateMods stateModsAssignment(int state)
{
    switch (state)
    {

    case 0:
        return WHILE_MOD;

    case 1:
        return IF_MOD;

    default:
        return ASSIGNMENT_MOD;

    }
}


//Meni terminaly typu ID na neterminaly v podobe binItemu
int reduceID(struct TokenStack * stack)
{
    ///Vybereme ID z vrcholu zasobniku
    struct TokenStackItem * item = peekStack(stack, sizeStack(stack) - 1);

    ///Vytvorime z neho binItem
    struct binItem * element  = createBinItem(item->type,item->str->str);
    ///Pokud se nevytvoril, vraci chybu
    if(element == NULL)
    {
        return 0;
    }

    else
    {   ///Jinak popne ID a nahradi ho binItemem, tedy z nej udela neterminal
        popIndex(stack, sizeStack(stack) - 1);
        pushStack(stack,NON_TERM_BIN_ITEM,(struct binItem *)element,"");
        return 1;
    }
}


int ruleReduce(struct TokenStack * stack,struct binStruct * nodeStack,int index)
{

    int number_of_non_terminals_after_index = (sizeStack(stack)-1) - index;

    ///Kontrola, ze nejsme na vrcholu a zaroven je pred operatorem neterminal
    if(number_of_non_terminals_after_index != 1 || ((sizeStack(stack)-1) == index) || ((index - 1) <= 0))
    {
        return -SYNTAX_ERR;
    }

    ///veci na redukci
    TokenStackItem * item1 = peekStack(stack,(index - 1));
    TokenStackItem * item2 = peekStack(stack,index);
    TokenStackItem * item3 = peekStack(stack,(index + 1));

    //Podminka, ze oba jsou neterminaly
    if((item1->type != NON_TERM_BIN_ITEM && item1->type != NON_TERM_BIN_NODE) || (item3->type != NON_TERM_BIN_ITEM && item3->type != NON_TERM_BIN_NODE))
    { ///Pokud ne, vracime chybu
        item1 = NULL;
        item2 = NULL;
        item3 = NULL;
        return -SYNTAX_ERR;

    }
       ///U prirazeni muze byt pouze na leve strane ID
    else if(item2->type == TT_ASSIGNMENT)
    { 

       ///Pokud je tam vyraz, vracime syntax error
       if(item1->type == NON_TERM_BIN_NODE) 
        
        { 
        ///Pokud ne, vracime chybu
        item1 = NULL;
        item2 = NULL;
        item3 = NULL;
        return -SYNTAX_ERR;
        }

    }


        ///operator pripraveny pro vlozeni
        struct binItem * operator = createBinItem(item2->type, item2->str->str);
        char arrayBinIndex[200];

        //hodnota
        int  nodeIndex1 = 0;



        ///Ukladame itemy na ukazatele a u nodu jejich indexy do stringu


        ///oba jsou itemy
        if(item1->type == NON_TERM_BIN_ITEM && item3->type == NON_TERM_BIN_ITEM)
        {
            ///vytvorim node
            nodeIndex1 = addBinNode(nodeStack,item1->ptr,operator,item3->ptr,NT_ALONE);
            
            if(nodeIndex1 == -1)
             {
              return -INTERNAL_COMP_ERR;
             }

            sprintf(arrayBinIndex,"%d",nodeIndex1);

            ///vypopuji se zredukovane polozky
            popIndex(stack,sizeStack(stack)-1);
            popIndex(stack,sizeStack(stack)-1);
            popIndex(stack,sizeStack(stack)-1);

            ///Pushneme na zasobnik uzel
            pushStack(stack,NON_TERM_BIN_NODE,NULL,arrayBinIndex);


            return SUCCESS;
        }
        ///1. node 2. je item
        else if(item1->type == NON_TERM_BIN_NODE && item3->type == NON_TERM_BIN_ITEM)
        {
            nodeIndex1 = addBinNode(nodeStack,nodeStack->Stack->array[atoi(item1->str->str)],operator,item3->ptr,NT_LEFT);
          
            if(nodeIndex1 == -1)
             {
              return -INTERNAL_COMP_ERR;
             }


            sprintf(arrayBinIndex,"%d",nodeIndex1);


            ///vypopuji se zredukovane polozky
            popIndex(stack,sizeStack(stack)-1);
            popIndex(stack,sizeStack(stack)-1);
            popIndex(stack,sizeStack(stack)-1);


            ///Pushneme na zasobnik uzel
            pushStack(stack,NON_TERM_BIN_NODE,NULL,arrayBinIndex);

            return SUCCESS;
        }

        ///vlevo je item, vpravo je node
        else if(item1->type == NON_TERM_BIN_ITEM && item3->type == NON_TERM_BIN_NODE)
        {
            nodeIndex1 = addBinNode(nodeStack,item1->ptr,operator,nodeStack->Stack->array[atoi(item3->str->str)],NT_RIGHT);

            if(nodeIndex1 == -1)
             {
              return -INTERNAL_COMP_ERR;
             }
            
            sprintf(arrayBinIndex,"%d",nodeIndex1);

            ///vypopuji se zredukovane polozky
            popIndex(stack,sizeStack(stack)-1);
            popIndex(stack,sizeStack(stack)-1);
            popIndex(stack,sizeStack(stack)-1);

            ///Pushneme na zasobnik uzel
            pushStack(stack,NON_TERM_BIN_NODE,NULL,arrayBinIndex);

            return SUCCESS;

        }

        ///oba jsou nody
        else if(item1->type == NON_TERM_BIN_NODE && item3->type == NON_TERM_BIN_NODE)
        {    ///Vztvorime nody,
            int nodeIndex1 = addBinNode(nodeStack,nodeStack->Stack->array[atoi(item1->str->str)],operator,nodeStack->Stack->array[atoi(item3->str->str)],NT_BOTH);
           
            if(nodeIndex1 == -1)
             {
              return -INTERNAL_COMP_ERR;
             }

            sprintf(arrayBinIndex,"%d",nodeIndex1);

            ///vypopuji se zredukovane polozky

            popIndex(stack,sizeStack(stack)-1);
            popIndex(stack,sizeStack(stack)-1);
            popIndex(stack,sizeStack(stack)-1);

            ///Pushneme na zasobnik uzel
            pushStack(stack,NON_TERM_BIN_NODE,NULL,arrayBinIndex);

            return SUCCESS;
        }



    return -SYNTAX_ERR;

}



void freeResources(struct TokenStack * stack,struct binStruct * binaryStack)
{

    if(stack)
    {   ///Jdeme od svrchu yasobniku
        int max = sizeStack(stack)-1;

        for(int i = max; i >= 0; i--)
        {
            TokenStackItem * item = peekStack(stack,i);
            ///Uvolnujeme nezredukovane binItemy
            if(item->type == NON_TERM_BIN_ITEM)
            {
                freeBinItem((struct binItem *)item->ptr);
            }
        }
    }

    //Redukce stacku a binarniho stacku
    freeStack(stack);
    freeBinStruct(binaryStack);


}



char actionByPrecedenceTable (int row, int column)
{
    ///Vraci akci podle precedencni tabulky
    return priorityTable[row][column];
}


 ///Vraci index pro tabulku precedenci
ptElements operatorTypeAssignment(int type_of_token)
{

    switch (type_of_token)
    {

    case TT_PLUS:            //    +
        return PT_ADD;

    case TT_MINUS:
        return PT_SUB;           //    -

    case TT_MULTIPLY:
        return PT_MUL;      //    *

    case TT_DIVISION:
        return PT_DIV;        //    /

    case TT_LEFT_BRACKET:    //    (
        return PT_LP;

    case TT_RIGHT_BRACKET:   //    )
        return PT_RP;

    case TT_EQUAL:           //    ==
        return PT_LEQ;

    case TT_HIGHER:          //    >
        return PT_G;

    case TT_LOWER:           //    <
        return PT_L;

    case TT_HIGHER_EQUAL:    //    >=
        return PT_GE;

    case TT_LOWER_EQUAL:     //    <=
        return PT_LE;

    case TT_ASSIGNMENT:
        return PT_EQ;        // =

    case TT_FALSE_EQUAL:     //    !=
        return PT_NEQ;

    case TT_IDENTIFIKATOR:
    case TT_STRING:
    case TT_INTEGER:
    case TT_FLOAT:
    case TT_EXPONENT:
    case TT_NIL:
        return PT_ID;            //id



    case TT_EOL:              /// $
    case TT_DO:
    case TT_THEN:
        return PT_END;
    default:
        return PT_ERROR;

    }

}
