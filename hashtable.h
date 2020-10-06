#ifndef HASHTABLE_FILE
#define HASHTABLE_FILE

    /*hashtable.h*/

    typedef struct movil movil;
    void doHash();
    unsigned char checkBit(int n, unsigned char d);
    unsigned char* charToBin(char c);
    unsigned char* strToBin(char* str);
    unsigned int getHash(char* modelo);
    void initHash();
    void printHash();
    bool insertHash(char* modelo, unsigned int costo);
    bool deleteHash(char* modelo);
    int readHash(char* modelo);
    void inicioRandom();
    void finish();
#endif

//EOF
