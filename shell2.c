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

extern char *pathname;
extern char *line;
extern char *tr_line;
extern char **arguments;
static builtins built_cmds[] = {{"exit", exit_shell}, {"env", env_print}, {NULL, NULL}}; 
static int bindex;

void set_pathname_from_env(char **env, char *path)
{
	char  *dirpath, *path_var;

	path_var = get_env(env, "PATH");  //allocates new memory and stores path there
	dirpath = search_path(path_var, path); 
	if (!dirpath)
	{
		printf("command not found\n");
		free_setnull(&path);
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
	token = strtok(tmp, " ");
	while (token != NULL)
	{
		no_of_args++;
		token = strtok(NULL, " ");
	}
	free(tmp);
	arguments = malloc(sizeof(*arguments) * (no_of_args + 1));
	if (!arguments)
		clean_exit("malloc error", 1);
	arguments[0] = pathname;
	token = strtok(line, " ");
	for (i = 1; i < no_of_args; i++)
		arguments[i] = strtok(NULL, " "); 
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
void exec_builtin(char **env, char *cmd)
{
	(built_cmds[bindex].bfunc)(env, cmd);
	free(cmd);
	clean_buff();
}

void exit_shell(__attribute__((unused)) char **env, char *cmd)
{
	int exit_status = 0, i, not_an_int = 0;

	pathname = cmd;
	set_arguments();
	printf("%s\n", cmd);
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
	while (*ptr != '\n' && *ptr != 4)
		ptr++;
	if (*ptr == 4)
		printf("\n");
	*ptr = '\0';
	return str;
}
void env_print(char **env, char *cmd)
{
	int i;

	for (i = 0; env[i] != NULL; i++)
		printf("%s\n", env[i]);	
}

void set_trimmed_line(void)
{
	ssize_t line_size = 0, res;

	res = getline(&line, &line_size, stdin);
	if (res == -1)
	{
		if (line == NULL)
			clean_exit("getline error", 1);
		else
		{
			printf("\n");
			clean_exit(NULL, 1);
		}
	}
	tr_line = remove_newline_trim_str(line);
}

int set_command_pathname(char **env, char *str)
{
	if (strchr(str, '/') == NULL)
	{
		set_pathname_from_env(env, str);
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

int main(__attribute__((unused)) int ac, __attribute__((unused)) char **av, char **env)
{
        char *prompt = "myshell$ ", *tmp = NULL;
	pid_t cpid;

        do {
		printf("%s", prompt);
		set_trimmed_line();
		if (*tr_line == '\0')
		{
			clean_buff();
			continue;
		}
		tmp = strdup(tr_line);
		if (!tmp)
			clean_exit("strdup error", 1);
		tmp = strtok(tmp, " ");
		if (isbuiltin(tmp))
		{
			exec_builtin(env, tmp);
			continue;
		}
		if (!set_command_pathname(env, tmp))
			continue;
		set_arguments();
                if ((cpid = fork()) == -1)
			clean_exit("fork error", 1);
                wait(NULL);
                if (cpid == 0)
                {
                        execve(pathname, arguments, env);
			clean_exit("execve error", 1);
		}
		clean_buff();
        } while (1);
        return (0);
}
