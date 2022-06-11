CC = gcc
FLAGS = -Wall -g

main: libmyfs.so main.o
	$(CC) $(FLAGS) main.o ./libmyfs.so -o main

libmyfs.so: myfs.c
	$(CC) $(FLAGS) --shared -fPIC myfs.c -o libmyfs.so

main.o: myfs.h main.c
	$(CC) $(FLAGS) -c main.c -o main.o

clean:
	rm -f *.so *.o main UFS