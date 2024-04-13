#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

void parse(char *comando, char **args);
void builtin(char **args, int fd);

char readheader(int fd);
void printheader(int fd);
