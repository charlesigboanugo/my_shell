#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

char *pathname = NULL;
char *line = NULL;
char *tr_line = NULL;
char **arguments = NULL;
builtins built_cmds[] = {{"exit", exit_shell}, {"env", env_print}, {"setenv", set_env}, {"unsetenv", unset_env}, {NULL, NULL}}; 
int bindex;
char **environ = NULL;

void set_pathname_from_env(char *path)
{
	char  *dirpath, *path_var;

	if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0)
		return;
	path_var = get_env("PATH");
	dirpath = search_path(path_var, path); 
	if (!dirpath)
	{
		printf("command not found\n");
		free_setnull(&path);
		free_setnull(&path_var);
		clean_buff();
		return;
	}
        pathname = malloc((sizeof(dirpath + 1) + sizeof(path)) * sizeof(char));
	if (!pathname)
		clean_exit("malloc error", 1);
	strcpy(pathname, dirpath);
        strcat(pathname, "/");
       	strcat(pathname, path);
	free_setnull(&path);
	free_setnull(&path_var);
}
int validate_cmd_path(char *path)
{
	int filestatus;

	filestatus = get_file_status(path);
	if (filestatus != 2)
	{
		if (filestatus == 0)
			printf("%s no such file or directory\n", path);
		else if (filestatus == 1)
			printf("%s is a directory\n", path);
		else
			printf("%s is a not an executable\n", path);
		clean_buff();
		return (0);
	}
	return (1);

}
void free_setnull(char **mem)
{
	free(*mem);
	*mem = NULL;
}
void clean_buff(void)
{
	if (line != NULL)
		free_setnull(&line);
	if (pathname != NULL)
		free_setnull(&pathname);
	if (arguments != NULL)
	{
		free(arguments);
		arguments = NULL;
	}
}
void clean_exit(char *errmsg, int exit_status)
{
	clean_buff();
	if (errmsg != NULL)
		fprintf(stderr, "%s\n", errmsg);
	exit(exit_status);
}

void set_arguments(void)
{
	int no_of_args = 0, i;
	char *token = NULL, *tmp = NULL;

	tmp = strdup(tr_line);
	if (!tmp)
		clean_exit("strdup error", 1);
	token = str_tok(tmp, " ");
	while (token != NULL)
	{
		no_of_args++;
		token = str_tok(NULL, " ");
	}
	free(tmp);
	arguments = malloc(sizeof(*arguments) * (no_of_args + 1));
	if (!arguments)
		clean_exit("malloc error", 1);
	arguments[0] = pathname;
	token = str_tok(line, " ");
	for (i = 1; i < no_of_args; i++)
		arguments[i] = str_tok(NULL, " "); 
	arguments[i] = NULL;
}
int isbuiltin(char *cmd)
{
	bindex = 0;
	while (built_cmds[bindex].command != NULL)
	{
		if (strcmp(built_cmds[bindex].command, cmd) == 0)
			return (1);
		bindex++;
	}
	return (0);
}
void exec_builtin(char *cmd)
{
	pathname = cmd;
	set_arguments();
	(built_cmds[bindex].bfunc)();
	clean_buff();
}

void exit_shell(void)
{
	int exit_status = 0, i, not_an_int = 0;

	printf("%s\n", pathname);
	if (arguments[1] != NULL)
	{
		for (i = 0; arguments[1][i] != '\0'; i++)
		{
			if (arguments[1][i] < '0' || arguments[1][i] > '9')
			{
				fprintf(stderr, "an integer argument required\n");
				not_an_int = 1;
				break;
			}
		}
		if (!not_an_int)
			exit_status = atoi(arguments[1]);
	}
	clean_exit(NULL, exit_status);
}

char *remove_newline_trim_str(char *str)
{
	char *ptr = str;

	if (str == NULL)
		return (NULL);
	while (*str == ' ')
	{
		str++;
		ptr++;
	}
	while (*ptr != '\n' && *ptr != '\0')
		ptr++;
	if (*ptr == '\0')
		printf("\n");
	*ptr = '\0';
	return str;
}
void env_print(void)
{
	int i;

	for (i = 0; environ[i] != NULL; i++)
		printf("%s\n", environ[i]);	
}

void set_trimmed_line(void)
{
	size_t line_size = 0;
        ssize_t res;

	errno = 0;
	res = get_line(&line, &line_size, stdin);
	if (res == -1)
	{
		if (errno == 0)
		{
			printf("\n");
			clean_exit(NULL, 0);
		}
		else
			clean_exit("get_line error", 1);
	}
	tr_line = remove_newline_trim_str(line);
}

int set_command_pathname(char *str)
{
	if (strchr(str, '/') == NULL)
	{
		set_pathname_from_env(str);
		if (!pathname)
			return (0);
	}
        else
	{
		pathname = str;
		if (!validate_cmd_path(str))
			return (0);
	}
	return (1);

}

char **envcpy(char **env)
{
	char **copy = NULL;
	int i = 0;

	while (env[i] != NULL)
		i++;
	copy = malloc(sizeof(*copy) * (i + 1));
	if (copy == NULL)
		clean_exit("malloc error", 1);
	for (i = 0; env[i] != NULL; i++)
		copy[i] = strdup(env[i]);
	copy[i] = NULL;
	return (copy);
}

int main(__attribute__((unused)) int ac, __attribute__((unused)) char **av, char **env)
{
        char *prompt = "myshell$ ", *tmp = NULL;
	pid_t cpid;

        do {
		printf("%s", prompt);
		fflush(stdout);
		set_trimmed_line();
		if (*tr_line == '\0')
		{
			clean_buff();
			continue;
		}
		tmp = strdup(tr_line);
		if (!tmp)
			clean_exit("strdup error", 1);
		tmp = str_tok(tmp, " ");	
		environ = envcpy(env);
		if (isbuiltin(tmp))
		{
			exec_builtin(tmp);
			continue;
		}
		if (!set_command_pathname(tmp))
			continue;
		set_arguments();
                if ((cpid = fork()) == -1)
			clean_exit("fork error", 1);
                wait(NULL);
                if (cpid == 0)
                {
                        execve(pathname, arguments, environ);
			clean_exit("execve error", 1);
		}
		clean_buff();
        } while (1);
        return (0);
}
