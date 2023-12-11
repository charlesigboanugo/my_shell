#include "shell.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

char *get_env(char *var)
{
	char *val = NULL, *tmp = NULL;
	int i = 0;

	while (environ[i] != NULL && val == NULL)
	{
		tmp = strdup(environ[i]);
		if (!tmp)
		{
			printf("strdup error");
			exit(1);
		}
		if (strcmp(str_tok(tmp, "="), var) == 0)
		{
			val = strdup(str_tok(NULL, ""));
			if (val == NULL)
			{
				printf("strdup error");
				exit(1);
			}
		}
		free(tmp);
		i++;
	}	
	return (val);
}

char *search_path(char *path, char *cmd)
{
        DIR *dirp = NULL;
        struct dirent *direntry = NULL;
	char *dirpath = NULL, *nxt_path;
	int file_present = 0;

	nxt_path = str_tok(path, ":");
	while (nxt_path && !file_present)
	{
		dirpath = nxt_path;
		nxt_path = str_tok(NULL, ":");
        	dirp = opendir(dirpath);
        	if (!dirp)
			break;
       		do {
			errno = 0;
                	direntry = readdir(dirp);
                	if (direntry == NULL)
			{
				if (errno)
				{
					closedir(dirp);
					exit(1);
				}
				break;
			}
			if (strcmp(direntry->d_name, cmd) == 0)
			{
				file_present = 1;
				break;
			}

       		} while (1);
		closedir(dirp);
	};
	if (file_present)
		return (dirpath);

	else
		return (NULL);
}
