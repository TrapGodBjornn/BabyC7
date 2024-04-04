#define _POSIX_C_SOURCE 200809L // required for strdup() on cslab

#include <stdio.h >
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
    return;
}
