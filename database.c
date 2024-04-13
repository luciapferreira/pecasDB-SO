/*
It is suggested that you create a separate standalone program to create test database files of your choosing
Peca structure:
    #define SZ_NAME 16
    #define SZ_BRAND 16
    typedef struct
    {
        int id;
        char name[SZ_NAME];
        int brandId;
        unsigned int weight;
        double price;
    } Peca;
    #define SZ_PECA (sizeof(Peca))
*/