#include "shell2.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

extern char *pathname;

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
