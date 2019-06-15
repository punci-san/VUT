/**
•  1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexé-
	   mu).
•  2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu).
•  3 - sémantická chyba v programu – nedefinovaná funkce/promenná, pokus o redefi-nici funkce/promenné, atd.
•  4 - sémantická chyba typové kompatibility v aritmetických, retezcových a relacních výrazech, príp. špatný pocet parametr  ̊u u volání funkce.
•  6 - ostatní sémantické chyby.
•  99 - interní chyba prekladace tj. neovlivnená vstupním programem (napr. chyba alo-kace pam
		eti, atd.)
*/
#define LEXICAL_ERR 1
#define SYNTAX_ERR 2
#define SEMANTIC_PROG_ERR 3
#define SEMANTIC_TYPE_COMP_ERR 4
#define FILE_ERROR 5
#define SEMANTIC_OTHER_ERR 6
#define INTERNAL_COMP_ERR 99

/**
• 50 - chybne zadané vstupní parametry na príkazovém rádku pri spouštení interpretu.
• 51 - chyba pri analýze (lexikální, syntaktická) vstupního kódu v IFJcode18.
• 52 - chyba pri sémantických kontrolách vstupního kódu v IFJcode18.
• 53 - behová chyba interpretace – špatné typy operand ̊u.
• 54 - behová chyba interpretace – prístup k neexistující promenné (rámec existuje).
• 55 - behová chyba interpretace – rámec neexistuje (napr.ctení z prázdného zásobníku rámcu).
• 56 - behová chyba interpretace – chybející hodnota (v promenné nebo na datovém
	   zásobníku).
• 57 - behová chyba interpretace – delení nulou.
• 58 - behová chyba interpretace – chybná práce sretezcem.
• 60 - interní chyba interpretu tj. neovlivnená vstupním programem (napr. chyba alo-kace pameti, chyba pri otvírání souboru s
	   rídicím programem atd.).
*/
#define INPUT_ERR 50
#define ANALYSE_ERR 51
#define SEMANTIC_CONTROL_ERR 52
#define OPERAND_ERR 53
#define VARIABLE_EXIST_ERR 54
#define FRAME_EXIST_ERR 55
#define MISSING_VALUE_ERR 56
#define NULL_DIVISION 57
#define STRING_ERR 58
#define INTERNAL_INTERPRET_ERR 60
