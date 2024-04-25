/*
    45524 João Novais
    46811 Ana Ferreira
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

int fdreadonly;
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
int readheader(char header[29]) {
    lseek(fdreadonly, 0, SEEK_SET);

    if(read(fdreadonly, header, 28) == -1) {
        return -1;
    }

    return 0;
}

// Read BRANDS
int readbrands(char *brands) {
    lseek(fdreadonly, 28, SEEK_SET);

    for (int i = 0; i < nb; i++) {
        if(read(fdreadonly, brands + i * (SZ_BRAND + 1), SZ_BRAND) == -1) {
            return -1;
        }
        brands[i * (SZ_BRAND + 1) + SZ_BRAND] = '\0';
    }

    return 0;
}

// ====== Ordered by ID functions ======
// Read PART by ID
int readpartid(char *peca, int id){
    lseek(fdreadonly, 28 + (SZ_BRAND * nb) + (SZ_PECA * id), SEEK_SET);

    if (read(fdreadonly, peca, SZ_PECA) == -1) 
        return -1;

    return 0;
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
int writepart(int fd, int id, char *name, int idbrand, unsigned int weight, double price){
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
        return -1;

    return 0;
}

// Write new part
int addpart(int fd, char *name, int idbrand, unsigned int weight, double price){
    // Update global variable
    np++;

    // Create new part
    if (writepart(fd, (np - 1), name, idbrand, weight, price) == -1){
        np--;
        return -1;
    }
    
    // Update header in file
    char npnew[4];
    *(int *) npnew = np;
    lseek(fd, 20, SEEK_SET);
    if(write(fd, npnew, 4) == -1)
        return -1;
    
    printpartid(np - 1);
    return 0;
}

// ====== Ordered by name functions ======
int binarysearch(char *name) {
    int count = 0;
    for(int i = 0; name[i] != '\0'; i++){
        count++;
        if(count > SZ_NAME)
            return -1;
    }

    int low = 0;
    int high = np - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        char peca[SZ_PECA];
        lseek(fdreadonly, 28 + (SZ_BRAND * nb) + (SZ_PECA * mid), SEEK_SET);
        if (read(fdreadonly, peca, SZ_PECA) == -1) {
            return -1;
        }

        char pecaname[SZ_NAME];
        memcpy(pecaname, peca + 4, SZ_NAME);

        int compare = strcmp(pecaname, name);
        if (compare == 0)
            return mid;
        else if (compare < 0)
            low = mid + 1;
        else
            high = mid - 1;
    }

    return -1;
}

// Read part by NAME
int readpartname(char *name){
    int index = binarysearch(name);
    if (index != -1) {
        char peca[SZ_PECA];
        lseek(fdreadonly, 28 + (SZ_BRAND * nb) + (SZ_PECA * index), SEEK_SET);
        if (read(fdreadonly, peca, SZ_PECA) == -1)
            return -1;

        char pecaname[SZ_NAME];
        memcpy(pecaname, peca + 4, SZ_NAME);

        int id = *((int *) peca);
        int brandid = *((int*)(peca + 20));
        char brands[nb * (SZ_BRAND + 1)];
        readbrands(brands);
        unsigned int weight = *((unsigned int*)(peca + 24));
        double price = *((double*)(peca + 32));

        printf("Enter Part Name: PECA %d %s %d %s %d %lf\n", id, pecaname, brandid, brands + brandid * (SZ_BRAND + 1), weight, price);

        return 0;
    }

    return -1; 
}

int main(int argc, char *argv[]){
    // Check if file is provided
    if(argc != 2)
        return -1;

    // Open file
    int fdopen = open(argv[1], O_RDONLY);
    if (fdopen == -1)
        return -1;
    fdreadonly = fdopen;

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
    int argsnum;

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
   
        argsnum = parse(comando, args);


        // General options
        if (strcmp(args[0], "1") == 0 && argsnum == 1)
            printf("HEADER %c%s %d %d\n", t, description, np, nb);
        
        if (strcmp(args[0], "9") == 0 && argsnum == 1){
            close(fdreadonly);
            exit(EXIT_SUCCESS);
        }
            

        // Ordered by ID options
        if (t == '0'){
            if (strcmp(args[0], "2") == 0 && argsnum == 2){
                if (isdigit(*args[1]) && (atoi(args[1]) < np)){
                    printf("PECA ");
                    printpartid(atoi(args[1]));
                }
            }
            if (strcmp(args[0], "3") == 0 && argsnum == 5){
                if (isdigit(*args[2]) && isdigit(*args[3]) && isdigit(*args[4])){
                    int fdreadwrite = open(argv[1], O_RDWR);
                    if (fdreadwrite == -1)
                        return -1;
                    addpart(fdreadwrite, args[1], atoi(args[2]), atoi(args[3]), atoi(args[4]));
                    close(fdreadwrite);
                }
                    
            }
            if (strcmp(args[0], "4") == 0 && argsnum == 6){
                if (isdigit(*args[1]) && isdigit(*args[3]) && isdigit(*args[4]) && isdigit(*args[5])){
                    int fdreadwrite = open(argv[1], O_RDWR);
                    if (fdreadwrite == -1)
                        return -1;
                    writepart(fdreadwrite, atoi(args[1]), args[2], atoi(args[3]), atoi(args[4]), atoi(args[5]));
                    close(fdreadwrite);
                }   
            }
        }
        
        // Ordered by NAME option
        if (t == '1' && strcmp(args[0], "5") == 0 && argsnum == 2){
            readpartname(args[1]);
        }
    }

    return 0;
}
