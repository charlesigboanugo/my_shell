#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *str_tok(char *str, char *delim)
{
	static char *buff = NULL;
	char *tmp = NULL, *tok = NULL;

	if (delim == NULL)
	{
		fprintf(stderr, "non null delimeter required");
		return (NULL);
	}

	if (str == NULL)
	{
		tok = buff;
		tmp = buff;
	}
	else
	{
		tok = str;
		tmp = str;
	}
	if (tmp != NULL)
	{
		while (strchr(delim, *tmp) != NULL && *tmp != '\0')
		{
			tok++;
			tmp++;
		}
		while (strchr(delim, *tmp) == NULL && *tmp != '\0')
			tmp++;
		if (*tmp != '\0')
		{
			buff = (tmp + 1);
			*tmp = '\0';
		}
		else
		{
			buff = NULL;
			if (*tok == '\0')
				tok = NULL;
		}
	}
	return (tok);
}
