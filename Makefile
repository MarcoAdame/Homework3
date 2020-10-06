all: hashtable.o hashtable.so

hashtable.o: hashtable.c
	gcc -c -Wall -Werror -fpic hashtable.c

hashtable.so: hashtable.o
	gcc -shared -o hashtable.so hashtable.o
