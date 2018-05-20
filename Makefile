CC = gcc
CFLAGS =-Wall -std=gnu99 -g
MAIN = mytar
OBJS = mytar.o createtar.o listtar.o extracttar.o

all : $(MAIN)

$(MAIN) : $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

mytar.o: mytar.c mytar.h
	$(CC) $(CFLAGS) -c mytar.c

createtar.o: createtar.c createtar.h
	$(CC) $(CFLAGS) -c createtar.c

listtar.o: listtar.c listtar.h
	$(CC) $(CFLAGS) -c listtar.c

extracttar.o: extracttar.c extracttar.h
	$(CC) $(CFLAGS) -c extracttar.c

clean :
	rm *.o $(MAIN)

handin:
	handin getaylor-grader 357hw4-11 handindir/*

test: 
	~getaylor-grader/tryAsgn4
