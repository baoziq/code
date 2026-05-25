world.out: hello.o main.o
	gcc -o world.out hello.o main.o

hello.o: hello.c
	gcc -c hello.c

main.o: main.c hello.h
	gcc -c main.c

clean: 
	rm -f *.o world.out