#ifndef SHELLH
#define SHELLH
#include <stddef.h>
#include <stdio.h>

char *search_path(char *path, char *cmd);
char *get_env(char *var);
int get_file_status(char *path);
int validate_cmd_path(char *path);
void set_pathname_from_env(char *path);
int set_command_pathname(char *str);
void clean_exit(char *errmsg, int exit_status);
void clean_buff(void);
void free_setnull(char **mem);
void set_arguments(void);
int isbuiltin(char *cmd);
void exec_builtin(char *cmd);
void exit_shell(void);
char *remove_newline_trim_str(char *str);
char *str_tok(char *str, char *delim);
ssize_t get_line(char **lineptr, size_t *size, FILE *stream);
void set_trimmed_line(void);
void env_print(void);
void set_env(void);
void unset_env(void);
int checkvar(char *varstring, char *var);
char **getvarstring(char *var);
typedef struct builtins {
	char *command;
	void (*bfunc)();
} builtins;

extern char *pathname;
extern char *line;
extern char **arguments;
extern char *tr_line;
extern builtins built_cmds[];
extern int bindex;
extern char **environ;


#endif
