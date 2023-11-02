#ifndef SHELLH
#define SHELLH
#include <stddef.h>

char *search_path(char *path, char *cmd);
char *get_env(char **envp, char *var);
int get_file_status(char *path);
int validate_cmd_path(char *path);
void set_pathname_from_env(char **env, char *path);
int set_command_pathname(char **env, char *str);
void clean_exit(char *errmsg, int exit_status);
void clean_buff(void);
void free_setnull(char **mem);
void set_arguments(void);
int isbuiltin(char *cmd);
void exec_builtin(char **env, char *cmd);
void exit_shell(__attribute__((unused)) char **env, char *cmd);
char *remove_newline_trim_str(char *str);
void set_trimmed_line(void);
void env_print(char **env, char *cmd);
typedef struct builtins {
	char *command;
	void (*bfunc)(char **env, char *cmd);
} builtins;

char *pathname = NULL;
char *line = NULL;
char **arguments = NULL;
char *tr_line = NULL;

#endif
