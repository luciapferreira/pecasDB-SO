#include "header.h"

void parse(char *command, char **args){
    // int count = 0;
    while (*command != '\0'){
        while (isspace((unsigned char) *command))
            *command++ = '\0';
        
        *args++ = command;
        // count++;

        while ((*command != '\0') && (!isspace ((unsigned char) *command)))
            command++;        
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
void readheader(int fd, char header[29]) {
    lseek(fd, 0, SEEK_SET);
    if (read(fd, header, 28) == -1) {
        perror("Error reading header");
        exit(EXIT_FAILURE);
    }
}
// Read PARTS

// ====== Option 1 ========
void printheader(int fd){
    char header[29];

    readheader(fd, header);


    char t = header[0];
    char description[20];
    memcpy(description, header + 1, 19);
    description[19] = '\0';
    int np = *((int*)(header + 20));
    int nb = *((int*)(header + 24));

    printf("HEADER %c%s %d %d\n", t, description, np, nb);
}
