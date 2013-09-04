CC=gcc
CFLAGS=-Wall -std=c99 -pedantic

a.out: file_organizer.o
	$(CC) file_organizer.o
file_organizer.o: file_organizer.c
	$(CC) $(CFLAGS) -c file_organizer.c

clean:
	rm *.o a.out

