CC = gcc
CFLAGS = -Wall -I.
LIBS = -lsqlite3

# Hedef
executable_output: main.o database.o data_generation.o
	$(CC) -o $@ $^ $(LIBS)

# Derleme kuralları
main.o: main.c database.h data_generation.h
	$(CC) $(CFLAGS) -c main.c

database.o: database.c database.h
	$(CC) $(CFLAGS) -c database.c

data_generation.o: data_generation.c data_generation.h
	$(CC) $(CFLAGS) -c data_generation.c

# Temizlik
clean:
	rm -f *.o executable_output
