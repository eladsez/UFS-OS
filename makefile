CC = gcc
FLAGS = -Wall -g

all: main

main: libmyfs.so main.o libmylibc.so
	$(CC) $(FLAGS) main.o ./libmyfs.so ./libmylibc.so -o main

libmyfs.so: myfs.c myfs.h
	$(CC) $(FLAGS) --shared -fPIC myfs.c -o libmyfs.so

libmylibc.so: mystdio.c mystdio.h
	$(CC) $(FLAGS) -w --shared -fPIC mystdio.c -o libmylibc.so

main.o: myfs.h main.c
	$(CC) $(FLAGS) -c main.c -o main.o

clean:
	rm -f *.so *.o main UFS