#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "string.h"

char **arguments;
char **environ;

void set_env(void)
{
	char **ptr = NULL, *tmp, **new_env = NULL;
	int len = 0, varsize = 0, i = 0;

	if (arguments[1] == NULL || arguments[2] == NULL)
	{
		fprintf(stderr, "USEAGE: %s [variable] [value]\n", pathname);
		return;
	}

	varsize =  strlen(arguments[1]) + strlen(arguments[2]) + 1;
	ptr = getvarstring(arguments[1]);
	if (ptr != NULL)
	{
		tmp = malloc(sizeof(tmp) * varsize);
		if (tmp == NULL)
			clean_exit("malloc error", 1);
		strcpy(tmp, arguments[1]);
		strcat(tmp, "=");
		strcat(tmp, arguments[2]);
		free_setnull(ptr);
		*ptr = tmp;
	}
	else
	{
		while (environ[len] != NULL)
			len++;
		new_env = malloc(sizeof(*new_env) * (len + 1));
		if (new_env == NULL)
			clean_exit("malloc error", 1);
		tmp = malloc(sizeof(*tmp) * varsize);
		if (tmp == NULL)
			clean_exit("malloc error", 1);
		strcpy(tmp, arguments[1]);
		strcat(tmp, "=");
		strcat(tmp, arguments[2]);
		while (i < len - 1)
		{
			new_env[i] = strdup(environ[i]);
			i++;
		}
		new_env[i++] = tmp;
		new_env[i] = NULL;
		for (i = 0; environ[i] != NULL; i++)
			free(environ[i]);
		free(environ);
		environ = new_env;
		
	}
}

void unset_env()
{

}

char **getvarstring(char *var)
{
	char *ptr = NULL;
	int i = 0;

	while (environ[i] != NULL)
	{
		ptr = environ[i];
		if (checkvar(ptr, var))
			return (&environ[i]);
		i++;
	}	
	return (NULL);
}

int checkvar(char *varstring, char *var)
{
	if (varstring == NULL)
	{
		fprintf(stderr, "varstring cannot be NULL\n");
		exit(1);
	}
	if (var == NULL)
	{
		fprintf(stderr, "var cannot be NULL\n");
		exit(1);
	}
	while (1)
	{
		if (*var == '\0' && *varstring != '=')
			return (0);
		else if (*var == '\0' && *varstring == '=')
			return (1);
		else if (*varstring == '=' && *var != '\0')
			return (0);
		else if (*varstring != *var)
			return (0);
		else
		{
			var++;
			varstring++;
		}
	}
}
