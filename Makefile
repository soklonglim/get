get: get.o
	gcc -std=c99 -Wall -o get get.o
	chmod u+s get
get.o: get.c
	gcc -std=c99 -Wall -c get.c
clean:
	rm *.o get
