/*
One command line parameter – the name of the database file
File structure:
    Header consists of 28 Bytes:
        The first 20 bytes are a string where:
            The first character indicates the type of ordering of the parts:
                T=0 - Order by id
                T=1 - Order by name
            The following 19 bytes are reserved for string description.
        Next are two binary encoded integers (4 bytes each):
            Number of parts (NP)
            Number of brands (NB)
    Next sections consists of a database of brands ordered as a set of brands.
        Each brand is a string of at most 16 characters including the string terminator

Manipulate the database of parts in a binary file, using 
    open() close() read() write() and lseek() 
!!! You may NOT use the standard library file functions !!!
    fopen() fread() fwrite() etc...
You may use the standard library for reading and writing to a terminal 
    standard input scanf() and output printf()

Options:
    [X] 1 - print the header
        Output: HEADER followed by number of parts and number of brands. Tokens separated by space
            ex: HEADER 6 2
    [X] 9 - exit
    
    When the database file is ordered by ID:
    [X] 2 - view a part with id
    [X] maybe 3 - add a new part specifying (in this order): name, brand, id, weight, price - the id can be added automatically
    [X] 4 - change a part with id specifying (in this order): new name, brand, id, weight, price
    
    When the database file is ordered by part NAME:
    [X] 5 - view a part with name

[X] Limits:
    0 <= NB <= 1000000
    0 <= NP <= 1000000

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#define SZ_NAME 16
#define SZ_BRAND 16
#define SZ_PECA 40

int fd;
int nb;
int np;

int parse(char *command, char **args){
    int count = 0;
    while (*command != '\0'){
        while (isspace((unsigned char) *command))
            *command++ = '\0';

        *args++ = command;
        count++;

        while ((*command != '\0') && (!isspace ((unsigned char) *command)))
            command++;
    }
    *args = (char *) NULL;
    return count;
}

// ====== Read file contents ======
// Read HEADER
void readheader(char header[29]) {
    lseek(fd, 0, SEEK_SET);

    if(read(fd, header, 28) == -1) {
        exit(EXIT_FAILURE);
    }
}

// Read BRANDS
void readbrands(char *brands) {
    lseek(fd, 28, SEEK_SET);

    for (int i = 0; i < nb; i++) {
        if(read(fd, brands + i * (SZ_BRAND + 1), SZ_BRAND) == -1) {
            exit(EXIT_FAILURE);
        }
        brands[i * (SZ_BRAND + 1) + SZ_BRAND] = '\0';
    }
}

// Read PART by ID
void readpartid(char *peca, int id){
    lseek(fd, 28 + (SZ_BRAND * nb) + (SZ_PECA * id), SEEK_SET);

    if (read(fd, peca, SZ_PECA) == -1) {
        exit(EXIT_FAILURE);
    }
}

// Print part by ID
void printpartid(int id){
    if(id >= np)
        return;

    char peca[SZ_PECA];
    readpartid(peca, id);

    char name[SZ_NAME];
    memcpy(name, peca + 4, 15);
    
    name[SZ_NAME - 1] = '\0';

    int brandid = *((int*)(peca + 20));
    char brands[nb * (SZ_BRAND + 1)];
    readbrands(brands);

    unsigned int weight = *((unsigned int*)(peca + 24));
    double price = *((double*)(peca + 32));

    printf("%d %s %d %s %d %lf\n", id, name, brandid, brands + brandid * (SZ_BRAND + 1), weight, price);
}

// Write part
int writepart(int id, char *name, int idbrand, unsigned int weight, double price){
    // Check name length
    int count = 0;
    for(int i = 0; name[i] != '\0'; i++){
        count++;
        if(count > SZ_NAME)
            return -1;
    }

    // Check id and idbrand exists
    if(id >= np || idbrand >= nb)
        return -1;

    // Write part
    char peca[SZ_PECA];
    *(int*) peca = id;
    memcpy(peca + 4, name, count);
    *((int*)(peca + 20)) = idbrand;
    *((unsigned int*)(peca + 24)) = weight;
    *((double*)(peca + 32)) = price;

    lseek(fd, 28 + (SZ_BRAND * nb) + (SZ_PECA * id), SEEK_SET);
    if(write(fd, peca, SZ_PECA) == -1)
        exit(EXIT_FAILURE);

    return 0;
}

// Write new part
void addpart(char *name, int idbrand, unsigned int weight, double price){
    // Update global variable
    np++;

    // Create new part
    if (writepart((np - 1), name, idbrand, weight, price) == -1){
        np--;
        return;
    }
    
    // Update header in file
    char npnew[4];
    *(int *) npnew = np;
    lseek(fd, 20, SEEK_SET);
    if(write(fd, npnew, 4) == -1)
        exit(EXIT_FAILURE);
    
    printpartid(np - 1);
}

// Read part by NAME
void readpartname(char *name){
    // Check name length
    int count = 0;
    for(int i = 0; name[i] != '\0'; i++){
        count++;
        if(count > SZ_NAME)
            return;
    }

    lseek(fd, 28 + (SZ_BRAND * nb), SEEK_SET);

    char peca[SZ_PECA];
    for (int i = 0; i < np; i++) {
        if (read(fd, peca, SZ_PECA) == -1) {
            exit(EXIT_FAILURE);
        }

        char pecaname[SZ_NAME];
        memcpy(pecaname, peca + 4, SZ_NAME);

        // Check if the names match
        if (strcmp(pecaname, name) == 0) {
            int id = *((int *) peca);
            int brandid = *((int*)(peca + 20));
            char brands[nb * (SZ_BRAND + 1)];
            readbrands(brands);
            unsigned int weight = *((unsigned int*)(peca + 24));
            double price = *((double*)(peca + 32));

            printf("Enter Part Name: PECA %d %s %d %s %d %lf\n", id, pecaname, brandid, brands + brandid * (SZ_BRAND + 1), weight, price);

            return;
        }
    }
}

int main(int argc, char *argv[]){
    // Check if file is provided
    if(argc != 2)
        exit(EXIT_FAILURE);

    // Open file
    int fdopen = open(argv[1], O_RDWR);
    if (fdopen == -1)
        exit(EXIT_FAILURE);
    fd = fdopen;

    // Get header contents
    char header[29];
    readheader(header);

    // Order type
    char t = header[0];

    // Description 
    char description[20];
    memcpy(description, header + 1, 19);
    description[19] = '\0';

    // Number of parts
    int npopen = *((int*)(header + 20));
    // Number of brands
    int nbopen = *((int*)(header + 24));
    np = npopen;
    nb = nbopen;

    // Check that np and nb are inside the limits
    if (np < 0 || np >= 1000000 || nb < 0 || nb >= 1000000)
        exit(EXIT_FAILURE);

    char comando[1024];
    char *args[64];
    int len;

    while(1){
        if (fgets(comando, sizeof(comando), stdin) == NULL) {
            // End-of-file reached
            break;
        }

        len = strlen(comando);
        if(len == 1)
            continue;
        if(comando[len - 1] == '\n')
            comando[len - 1] = '\0';
   
        parse(comando, args);


        // General options
        if (strcmp(args[0], "1") == 0 && args[1] == NULL)
            printf("HEADER %c%s %d %d\n", t, description, np, nb);
        
        if (strcmp(args[0], "9") == 0 && args[1] == NULL){
            close(fd);
            exit(EXIT_SUCCESS);
        }
            

        // Ordered by ID options
        if (t == '0'){
            if (strcmp(args[0], "2") == 0 && args[1] != NULL && args[2] == NULL){
                if (isdigit(*args[1])){
                    printf("PECA ");
                    printpartid(atoi(args[1]));
                }
            }
            if (strcmp(args[0], "3") == 0 && args[4] != NULL && args[5] == NULL){
                if (isdigit(*args[2]) && isdigit(*args[3]) && isdigit(*args[4]))
                    addpart(args[1], atoi(args[2]), atoi(args[3]), atoi(args[4]));
            }
            if (strcmp(args[0], "4") == 0 && args[5] != NULL && args[6] == NULL){
                if (isdigit(*args[1]) && isdigit(*args[3]) && isdigit(*args[4]) && isdigit(*args[5]))
                    writepart(atoi(args[1]), args[2], atoi(args[3]), atoi(args[4]), atoi(args[5]));
            }
        }
        
        // Ordered by NAME option
        if (t == '1' && strcmp(args[0], "5") == 0 && args[1] != NULL && args[2] == NULL){
            readpartname(args[1]);
        }
    }

    return 0;
}
