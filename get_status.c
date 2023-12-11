#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int get_file_status(char *path)
{
	struct stat statbuf;

	errno = 0;
	if (stat(path, &statbuf) == 0)
	{
		if (S_ISDIR(statbuf.st_mode))
			return (1);
		else if (statbuf.st_mode & S_IXUSR)
			return (2);
		else
			return (3);
	}
	if (errno == ENOENT)
		return (0);
	printf("stat error");
	exit(1);
}
