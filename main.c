/*
Realizado por:
    45524 Joao Novais
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

typedef struct {
    int id;
    char name[SZ_NAME];
    int brandId;
    unsigned int weight;
    double price;
} Peca;

#define SZ_PECA (sizeof(Peca))

int fdreadonly;
int nb;
int np;

// ====== Read file contents ======
// Read HEADER
void readheader(char header[29]) {
    lseek(fdreadonly, 0, SEEK_SET);
    if(read(fdreadonly, header, 28) == -1)
        return;
}

// Read name of BRANDS
void readbrands(char *brands) {
    lseek(fdreadonly, 28, SEEK_SET);
    if(read(fdreadonly, brands, (SZ_BRAND * nb)) == -1)
        return;
}

// ====== Ordered by ID functions ======
// Read PART by ID
void readpartid(Peca *peca, int id){
    lseek(fdreadonly, 28 + (SZ_BRAND * nb) + (SZ_PECA * id), SEEK_SET);
    if (read(fdreadonly, peca, SZ_PECA) == -1)
        return;
}

// Print PART by ID
void printpartid(int id, int extrainfo){
    if(id >= np)
        return;

    Peca peca;
    readpartid(&peca, id);

    if (extrainfo == 1){
        // Prints the extra information: "PECA" and the name of the brand
        char brands[nb * SZ_BRAND];
        readbrands(brands);
        printf("PECA %d %s %d %s %d %lf\n", peca.id, peca.name, peca.brandId, brands + peca.brandId * SZ_BRAND, peca.weight, peca.price);
    }
    else
        printf("%d %s %d %d %lf\n", peca.id, peca.name, peca.brandId, peca.weight, peca.price);
}

// Write PART
void writepart(int fd, Peca peca) {
    lseek(fd, 28 + (SZ_BRAND * nb) + (SZ_PECA * peca.id), SEEK_SET);
    if (write(fd, &peca, SZ_PECA) == -1)
        return;
}

// Write new PART
void addpart(int fd, Peca newpeca) {
    np++;

    writepart(fd, newpeca);

    // Update header in file
    char npnew[4];
    *(int *)npnew = np;
    lseek(fd, 20, SEEK_SET);
    if (write(fd, npnew, 4) == -1)
        return;

    printpartid(np - 1, 0);
}

// ====== Ordered by NAME functions ======
// Read part by NAME - Binary search
void readpartname(char *name) {
    // Check if given name is not bigger than SZ_NAME
    int count = 0;
    for(int i = 0; name[i] != '\0'; i++){
        count++;
        if(count > SZ_NAME)
            return;
    }

    int low = 0;
    int high = np - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        Peca peca;
        lseek(fdreadonly, 28 + (SZ_BRAND * nb) + (SZ_PECA * mid), SEEK_SET);
        if (read(fdreadonly, &peca, SZ_PECA) == -1) {
            return;
        }

        int compare = strcmp(peca.name, name);
        if (compare == 0){
            printpartid(mid, 1);
            break;
        }
        else if (compare < 0)
            low = mid + 1;
        else
            high = mid - 1;
    }
}

int main(int argc, char *argv[]){
    // Check if file is provided
    if(argc != 2)
        return 0;

    // Open file
    int fdopen = open(argv[1], O_RDONLY);
    if (fdopen == -1)
        return 0;
    fdreadonly = fdopen;

    // Get header contents
    char header[29];
    readheader(header);

    // Order type
    int t = atoi(&header[0]);

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
    if (np < 0 || np > 1000000 || nb < 0 || nb > 1000000)
        return 0;

    int input;
    
    while (1)
    {
        // Get option
        scanf("%d", &input);
        // General options
        if (input == 9){
            break;
        }
        else if (input == 1){
            printf("HEADER %d%s %d %d\n", t, description, np, nb);
        }
        // Ordered by ID options
        else if (t == 0){
            // Print ID
            if (input == 2){
                int idinp;
                scanf(" %d", &idinp);
                printpartid(idinp, 1);
            }
            // Add new PART
            else if (input == 3){
                int fdreadwrite = open(argv[1], O_RDWR);
                if (fdreadwrite != -1){
                    Peca newpeca;
                    newpeca.id = np;
                    scanf(" %s %d %u %lf", newpeca.name, &newpeca.brandId, &newpeca.weight, &newpeca.price);
                    addpart(fdreadwrite, newpeca);
                }
                close(fdreadwrite);
            }
            // Modify PART
            else if (input == 4){
                int fdreadwrite = open(argv[1], O_RDWR);
                if (fdreadwrite != -1){
                    Peca peca;
                    scanf(" %d %s %d %u %lf", &peca.id, peca.name, &peca.brandId, &peca.weight, &peca.price);
                    writepart(fdreadwrite, peca);
                }
                close(fdreadwrite);
            }
        }

        // Ordered by NAME option
        else if (t == 1){
            if(input == 5){
                char name[SZ_NAME];
                scanf(" %s", name);
                readpartname(name);
            }
        }
    } 
    
    close(fdreadonly);
    return 0;
}
