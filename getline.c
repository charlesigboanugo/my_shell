#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

ssize_t get_line(char **lineptr, size_t *size, FILE *stream)
{
	static ssize_t default_size = 120, count = 1;
	ssize_t buf_alloc = 0, line_size = 0, byte_read = 0;
	char *tmp = NULL, single[1];
	int fd;

	errno = 0;
	if (lineptr == NULL)
		return (-1);
	if (*lineptr == NULL || size == NULL || *size == 0)
	{
		if (*lineptr != NULL)
			free(*lineptr);	
		*lineptr = malloc(sizeof(char) * default_size);
		if (*lineptr == NULL)
			return (-1);
		buf_alloc = default_size;
	}
	else
		buf_alloc = (ssize_t) *size;

	fd  = fileno(stream);
	if (fd == -1)
		return (-1);
	tmp = *lineptr;

	do {
		if (line_size + 2 > buf_alloc)
		{
			printf("yes");
			buf_alloc = (buf_alloc / default_size + 1) * default_size;
			tmp = malloc(sizeof(char) * buf_alloc);
			if (tmp == NULL)
				return (-1);
			memcpy(tmp, *lineptr, line_size);
			free(*lineptr);
			*lineptr = tmp;
		}
		if ((byte_read = read(fd, single, 1)) == -1)
			return (-1);
		if (byte_read != 0)
			tmp[line_size] = *single;
		line_size++;
	} while (byte_read != 0 && *single != '\n');
	if (byte_read == 0 && (line_size - 1) == 0)
		return (-1);
	if (byte_read == 0)
		tmp[--line_size] = '\0';
	else
		tmp[line_size] = '\0';
	return (line_size);
}
