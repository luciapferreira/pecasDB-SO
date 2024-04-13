CC=cc
FLAGS=-c -Wall
LIBS=-lm
OBS= main.o functions.o

all: main.o functions.o
	$(CC) -o teste $(OBS) $(LIBS)
	
main.o : header.h main.c 
	$(CC) $(FLAGS) main.c
functions.o : header.h functions.c 
	$(CC) $(FLAGS) functions.c

