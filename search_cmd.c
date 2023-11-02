#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

char *get_env(char **envp, char *var)
{
	char *val = NULL, *tmp;

	while (envp != NULL && val == NULL)
	{
		tmp = strdup(*envp);
		if (!tmp)
		{
			printf("strdup error");
			exit(1);
		}
		if (strcmp(strtok(tmp, "="), var) == 0)
		{
			val = strdup(strtok(NULL, ""));
			if (val == NULL)
			{
				printf("strdup error");
				exit(1);
			}
		}
		free(tmp);
		envp++;
	}	
	return (val);
}

char *search_path(char *path, char *cmd)
{
        DIR *dirp = NULL;
        struct dirent *direntry = NULL;
	char *dirpath = NULL, *nxt_path;
	int file_present = 0;

	nxt_path = strtok(path, ":");
	while (nxt_path && !file_present)
	{
		dirpath = nxt_path;
		nxt_path = strtok(NULL, ":");
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
