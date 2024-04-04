#define _POSIX_C_SOURCE 200809L // required for strdup() on cslab

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Use 16-bit code words */
#define NUM_CODES 65536

/* allocate space for and return a new string s+c */
char *strappend_char(char *s, char c);

/* read the next code from fd
 * return NUM_CODES on EOF
 * return the code read otherwise
 */
unsigned int read_code(int fd);

/* uncompress in_file_name to out_file_name */
void uncompress(char *in_file_name, char *out_file_name);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: unzip file\n");
        exit(1);
    }

    char *in_file_name = argv[1];
    char *out_file_name = strdup(in_file_name);
    out_file_name[strlen(in_file_name)-4] = '\0';

    uncompress(in_file_name, out_file_name);

    free(out_file_name);

    return 0;
}

/* allocate space for and return a new string s+c */
char *strappend_char(char *s, char c)
{
    if (s == NULL)
    {
        return NULL;
    }

    // reminder: strlen() doesn't include the \0 in the length
    int new_size = strlen(s) + 2;
    char *result = (char *)malloc(new_size*sizeof(char));
    strcpy(result, s);
    result[new_size-2] = c;
    result[new_size-1] = '\0';

    return result;
}

/* read the next code from fd
 * return NUM_CODES on EOF
 * return the code read otherwise
 */
unsigned int read_code(int fd)
{
    // code words are 16-bit unsigned shorts in the file
    unsigned short actual_code;
    int read_return = read(fd, &actual_code, sizeof(unsigned short));
    if (read_return == 0)
    {
        return NUM_CODES;
    }
    if (read_return != sizeof(unsigned short))
    {
       perror("read");
       exit(1);
    }
    return (unsigned int)actual_code;
}

/* uncompress in_file_name to out_file_name */
void uncompress(char *in_file_name, char *out_file_name)
{
	int in_fd= open(in_file_name, O_RDONLY);
	if (in_fd == -1)
	{
		perror("open");
		exit(1);
	}
	int out_fd= open(out_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (out_fd== -1)
	{
		perror("open");
		close(in_fd);
		exit(1);
	}
	
	char *dictionary[NUM_CODES];
	for (int i= 0; i< 256; i++)
	{
		char *str = (char *) malloc(2 * sizeof(char));
		str[0] = (char)i;
		str[1] = '\0';
		dictionary[i]= str;
	}
	for (int i= 256; i< NUM_CODES; i++)
	{
		dictionary[i]= NULL;
	}
	unsigned int CurrentCode = read_code(in_fd);
	if (CurrentCode== NUM_CODES)
	{
		close(in_fd);
		close(out_fd);
		return;
	}

	char *CurrentChar = dictionary[CurrentCode];
	write(out_fd, CurrentChar, strlen(CurrentChar));

	while (1)
	{
		unsigned int NextCode = read_code(in_fd);
		if (NextCode== NUM_CODES)
		{
			break;
		}

	char *CurrentString;
	if (NextCode< NUM_CODES && dictionary[NextCode] != NULL)
	{
		CurrentString = dictionary[NextCode];
	}
	else
	{
		CurrentString = strappend_char(dictionary[CurrentCode], dictionary[CurrentCode][0]);
	}
	write(out_fd, CurrentString, strlen(CurrentString));
	dictionary[NUM_CODES]= strappend_char(dictionary[CurrentCode], CurrentString[0]);
	CurrentCode= NextCode;
	}
	for (int i= 0; i< NUM_CODES; i++)
	{
		free(dictionary[i]);
	}
	close(in_fd);
	close(out_fd);

	
}
