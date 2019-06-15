#include <stdio.h>
#include "scanner.h"

int main(int argc, char **argv)
{
	initScanner(argv[1]);
	String *str;
	TokenType tokenType;
	initString(&str);

	while (tokenType != EOF)
	{
		tokenType = getToken(str);
		printf("TYPE: %d, KEY: %s\n",tokenType, str->str);
	}

	return 0;
}
