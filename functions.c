#include "header.h"

void parse(char *comando, char **args){
    // int count = 0;
    while (*comando != '\0'){
        while (isspace((unsigned char) *comando))
            *comando++ = '\0';
        
        *args++ = comando;
        // count++;

        while ((*comando != '\0') && (!isspace ((unsigned char) *comando)))
            comando++;        
    }
    *args = (char *) NULL;
    // return count;
}

void builtin(char **args, int fd){
    // General options
    if (strcmp(args[0], "1") == 0){
        printheader(fd);
    }
    if (strcmp(args[0], "9") == 0){
        exit(EXIT_SUCCESS);
    }
    // Ordered by ID options
    if (strcmp(args[0], "2") == 0){
        printf("Option 2");
    }
    if (strcmp(args[0], "3") == 0){
        printf("Option 3");
    }
    if (strcmp(args[0], "4") == 0){
        printf("Option 4");
    }
    // Ordered by NAME option
    if (strcmp(args[0], "5") == 0){
        printf("Option 5");
    }
}

// ====== Read file contents ======
// Read HEADER
char readheader(int fd){
    lseek(fd, 0, SEEK_SET);
    char header[29];
    if (read(fd, header, 28) == -1) {
        //perror("Error reading header");
        exit(EXIT_FAILURE);
    }
    return header;
}
// Read PARTS

// ====== Option 1 ========
void printheader(int fd){
    char header[] = readheader(fd);

    char t = header[0];
    char description[20];
    memcpy(description, header + 1, 19);
    description[19] = '\0';
    int np = *((int*)(header + 20));
    int nb = *((int*)(header + 24));

    printf("HEADER %c %s %d %d\n", t, description, np, nb);
}