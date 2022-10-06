CFLAGS = `sdl2-config --libs --cflags` -Wall
CC = gcc
MAIN = decoder
DEPENDENCIES = qoi.o

all: build clean

build: $(DEPENDENCIES) 	
	$(CC) $(MAIN).c $(CFLAGS) $(DEPENDENCIES) -o $(MAIN).out  	

qoi.o:
	$(CC) -c qoi.c -o qoi.o 

clean:
	rm qoi.o