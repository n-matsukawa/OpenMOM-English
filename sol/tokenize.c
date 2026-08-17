/*
tokensize.c

tokenize a string
*/

#include <stdlib.h>
#include <string.h>

int tokenize(char *str, const char tokensep[], char *token[], size_t maxtoken)
{
	if ((str == NULL) || !maxtoken) return 0;

	char *thistoken = strtok(str, tokensep);

	int   count;
	for (count = 0; (count < maxtoken) && (thistoken != NULL); ) {
		token[count++] = thistoken;
		thistoken = strtok(NULL, tokensep);
	}

	token[count] = NULL;

	return count;
}
