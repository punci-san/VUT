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
#define SEMANTIC_ARGUMENTS_ERR 5
#define SEMANTIC_OTHER_ERR 6
#define SEMANTIC_ZERO_DIVISION 9
#define INTERNAL_COMP_ERR 99
