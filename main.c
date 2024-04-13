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
    [] 2 - view a part with id
    [] 3 - add a new part specifying (in this order): name, brand, id, weight, price - the id can be added automatically
    [] 4 - change a part with id specifying (in this order): new name, brand, id, weight, price
    
    When the database file is ordered by part NAME:
    [] 5 - view a part with name

[X] Limits:
    0 <= NB <= 1000000
    0 <= NP <= 1000000

*/

#include "header.h"

int main(int argc, char *argv[]){

    // Check if file is provided
    if(argc != 2)
        exit(EXIT_FAILURE);

    // Open file
    int fd = open(argv[1], O_RDWR);
    if (fd == -1)
        exit(EXIT_FAILURE);
    
    // Check that np and nb are inside the limits
    /*char header[] = readheader(fd);
    int np = *((int*)(header + 20));
    int nb = *((int*)(header + 24));
    if (np < 0 || np >= 1000000 || nb < 0 || np >= 1000000)
        exit(EXIT_FAILURE);
    */
    char comando[1024];
    char *args[64];
    int len;

    while(1){
        fgets(comando, sizeof(comando), stdin);

        len = strlen(comando);
        if(len == 1)
            continue;
        if(comando[len - 1] == '\n')
            comando[len - 1] = '\0';
        
        parse(comando, args);
        builtin(args, fd);
    }

    close(fd);
    return 0;
}
