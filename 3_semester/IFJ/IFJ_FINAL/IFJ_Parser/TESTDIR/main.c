/**
 *  Project:
 *    Implementácia prekladača dynamického jazyka IFJ18
 *
 *  Authors:
 *    Pavel Podlužanský <xpodlu01@stud.fit.vutbr.cz>
 *
 **/
#include <stdio.h>

#include "err_code.h"
#include "parser.h"
#include "generate_code.h"

int main()
{
	// Error
	int err = 0;

	// Pokusime sa inicializovat parser
	if ((err = initParser()) != 0)
	{
		freeParser();
		return err;
	}

	// Zacneme parsovat subor
	err = Parse();

	// Uvolnime inicializovane struktury
	freeParser();

	return err;
}
