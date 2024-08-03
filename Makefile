CC = gcc
CFLAGS = -Wall -I.
LIBS = -lsqlite3

# Output
executable_output: main.o database.o data_generation.o
	$(CC) -o $@ $^ $(LIBS)

# Rules for compiler
main.o: main.c database.h data_generation.h
	$(CC) $(CFLAGS) -c main.c

database.o: database.c database.h
	$(CC) $(CFLAGS) -c database.c

data_generation.o: data_generation.c data_generation.h
	$(CC) $(CFLAGS) -c data_generation.c

# Cleaning
clean:
	rm -f *.o executable_output
