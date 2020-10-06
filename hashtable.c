/*hashtable.c*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define MAX_MODEL 15                             //tamanio maximo de string (solo chars de ascii)
#define TABLE_TAM 64                             //tamanio maximo de hashtable
#define U MAX_MODEL*8                            //tamanio maximo de string, en bits (y anchura de la hash matrix)
#define B (int)(log(TABLE_TAM) / log(2.0))      //altura de la hash matrix
#define BORRADO (movil*)(0xFAFA)                //marcador para cosas borradas de la hashtable

//tipo de dato que guarda nuestra hashtable
typedef struct movil{
    char modelo[MAX_MODEL]; //key
    unsigned int costo;    //value
}movil;

//estructuras globales
movil* hash_table[TABLE_TAM]; //este array es el cuerpo de la hash table
unsigned char** hashMatrix;     //esta matriz se usa para hashear las llaves

//crear una matriz de hash (random, diferente para cada ejecucion)
void doMatrix() {

    int i,j;

    //reservar memoria para la hash matrix
    hashMatrix = malloc(B * sizeof(unsigned char*));
    for(i=0; i<B; i++) {
        hashMatrix[i] = malloc(U * sizeof(unsigned char));
    }

    //llenar cada celda de la matriz con 1 o 0
    for(i=0; i<B; i++) {
        for(j=0; j<U; j++) {
            hashMatrix[i][j] = (unsigned char)rand()%2;
        }
    }
}

//wrapper del bitwise AND para checar el i-esimo bit
unsigned char checkBit(int n, unsigned char d) {

    //elevar d a alguna potencia de 2
    unsigned int b = (unsigned)pow(2.0, (double)d);

    if(n & b) {
        return 1;
    }else {
        return 0;
    }
}

//obtener un arreglo de 8 espacios con los bits de un caracter de ASCII
unsigned char* charToBin(char c) {

    //reservar 8 bytes de memoria (uno para cada bit)
    //overhead? C deberia tener un bool nativo....
    unsigned char* digits = calloc(8, sizeof(*digits));

    //checar cada bit del caracter y guardarlo en el arreglo
    unsigned int i, j;
    for(i=7, j=0; i>=0 && j<8; i--, j++) {
        digits[i] = checkBit(c, j);
    }

    return digits;
}

//obtener un arreglo con los bits de un string de ASCII
unsigned char* strToBin(char* str) {

    int j = U;

    //reservar memoria para el arreglo (8 bytes por cada caracter)
    unsigned char* digits = calloc(j, sizeof(*digits));
    unsigned int len = strnlen(str, MAX_MODEL);

    //leer el string y llenar el arreglo
    int i;
    for(i=len-1; i>=0; i--) {
        j-=8;
        memmove(digits+j, charToBin(str[i]), 8);
    }

    return digits;
}

//Obtiene la key realizando multiplicacion de matrices
unsigned int getHash(char* modelo) {

    //convertir la llave a arreglo de bits
    unsigned char* keyMatrix;
    keyMatrix = strToBin(modelo);

    unsigned int hval = 0;
    unsigned int exp  = 1;
    unsigned int val;
    unsigned int i, j;

    //recorrer filas de la hash matrix
    for(i=0; i<B; i++) {

        val = 0;

        //recorrer columnas de la hash matrix, multiplicar por la llave
        //y acumular el resultado
        for(j=0; j<U; j++) {
            val += ( hashMatrix[i][j] * keyMatrix[j] ); //XOR tambien funciona...
        }

        //sumar el resultado al hash value final
        val  %= 2;
        hval += val*exp;
        exp  *= 2;
    }

    return hval;
}

//inicializar la hash table con NULL
void initHash() {

    int i;
    for(i=0; i<TABLE_TAM; i++) {
        hash_table[i] = NULL;
    }
}

//imprimir todos los elementos de la Hash Table
void printHash() {
    printf("*-*-*-*-*-*-*-* HASH TABLE *-*-*-*-*-*-*-*-\n");
    //imprimir las celdas que tengan datos e ignorar las NULLs
    int i, j=0;
    for(i=0; i<TABLE_TAM; i++) {
        if(hash_table[i]==BORRADO) { //mostrar lo que ha sido borrado
            printf("%.2d: <----------------------BORRADO\n", i);
            j++;
        }else if(hash_table[i]!=NULL) { //mostrar key/value de cada dato
            printf("%.2d: %s, precio: %.2d \n", i, hash_table[i]->modelo, hash_table[i]->costo);
            j++;
        }
    }

    if(!j) {
        printf("La tabla esta vacia.\n");
    }

    printf("*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*\n");
}

//Insertar o cambiar un elemento
bool insertHash(char* modelo, unsigned int costo) {

    //hashear la llave
    int index = (int)getHash(modelo);
    int probe;

    int i;
    for(i=0; i<TABLE_TAM; i++) {

        probe = (index+i) % TABLE_TAM; //incrementar el probe para hacer linear probing

        if(hash_table[probe]==NULL || hash_table[probe]==BORRADO) { //celda vacia

            //crear una movil nueva y copiar sus datos
            movil* p = malloc(sizeof(movil)); //le haremos free() cuando lo borremos
            strncpy(p->modelo, modelo, MAX_MODEL);
            p->costo = costo;

            hash_table[probe] = p; //insertar
            return true;
        }else if(strncmp(hash_table[probe]->modelo, modelo, MAX_MODEL)==0) { //cambios en vez de altas

            //insertar un key que ya existe es editar su value
            hash_table[probe]->costo = costo; //editar
            return true;
        }else {
            continue; //colision, hacer linear probing
        }
    }

    printf("\nTHE HASH TABLE IS FULL. CAN'T INSERT.\n");
    return false;
}

//Eliminar un elemento
bool deleteHash(char* modelo) {

    //hashear la llave
    int index = (int)getHash(modelo);
    int probe;

    int i;
    for(i=0; i<TABLE_TAM; i++) {

        probe = (index+i) % TABLE_TAM; //incrementar el probe para hacer linear probing

        if(hash_table[probe]==NULL) { //no vamos a encontrar la llave
            return false;
        }else if(hash_table[probe]==BORRADO) { //aun no la encontramos, seguir buscando
            continue; //linear probing
        }else if(strncmp(hash_table[probe]->modelo, modelo, MAX_MODEL)==0) { //bajas

            free(hash_table[probe]); //liberar memoria
            hash_table[probe] = BORRADO; //borrar
            return true;
        }else {
            continue; //colision, aun no la encontramos, seguir buscando
        }
    }

    return false;
}

//Leer un elemento
int readHash(char* modelo) {

    //hashear la llave
    int index = (int)getHash(modelo);
    int probe;

    int i;
    for(i=0; i<TABLE_TAM; i++) {

        probe = (index+i) % TABLE_TAM; //incrementar el probe para hacer linear probing

        if(hash_table[probe]==NULL) { //no vamos a encontrar la llave
            return -1;
        }else if(hash_table[probe]==BORRADO) { //aun no la encontramos, seguir buscando
            continue; //linear probing
        }else if(strncmp(hash_table[probe]->modelo, modelo, MAX_MODEL)==0) { //consultas
            return hash_table[probe]->costo; //retornar value del key:value
        }else {
            continue; //colision, aun no la encontramos, seguir buscando
        }
    }

    return -1;
}

//InicioRandom de la Hash Table
void inicioRandom() {

    //inicializar valores necesarios
    srand(time(NULL)); //random seed
    doMatrix();  //matriz de hash
    initHash();   //hash table

    //^^^^ THIS IS NEEDED AT THE BEGINNING ^^^^
    //-----------------------------------------

    //ingresar algunos valores de InicioRandom e imprimir
    printf("This hash table is 64-spaces long.\n");
    printf("It takes only modelo:costo (str:int) key:value pairs.\n");
    printf("ONLY ASCII characters are allowed!!!\n");
    printf("Initializing with some sample data...\n");

    insertHash("Nokia",  rand()%10+20);
    insertHash("Huawei",    rand()%10+20);
    insertHash("Moto",     rand()%10+20);
    
    printHash();
}

//liberar la memoria al finalizar
void finish() {
    //-----------------------------------------
    //vvvvvvv THIS IS NEEDED AT THE END vvvvvvv

    //liberar la memoria de la hash table
    int i;
    for(i=0; i<B; i++) {
        free(hashMatrix[i]);
    }
    free(hashMatrix);
}


int main() {
    inicioRandom();
    finish();
    return 0;
}


//EOF
