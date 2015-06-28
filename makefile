hungry_birds: hungry_birds.o
	gcc -pthread -o hungry_birds hungry_birds.o -lgmp

hungry_birds.o: hungry_birds.c
	gcc -c hungry_birds.c -Wall -pedantic -ansi -g

clean:
	rm -rf *.o
	rm -rf *~
	rm hungry_birds