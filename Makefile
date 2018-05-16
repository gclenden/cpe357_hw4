CC = gcc
CFLAGS =-Wall -ansi -g
MAIN = mytar
OBJS = mytar.o createtar.o listtar.o extracttar.o

all : $(MAIN)

$(MAIN) : $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

mytar.o: mytar.c
	$(CC) $(CFLAGS) -c mytar.c

createtar.o: createtar.c
	$(CC) $(CFLAGS) -c createtar.c

listtar.o: listtar.c
	$(CC) $(CFLAGS) -c listtar.c

extracttar.o: extracttar.c
	$(CC) $(CFLAGS) -c extracttar.c

clean :
	rm *.o $(MAIN)

handin:
	handin getaylor-grader 357hw4-11 handindir/*

test: 
	~getaylor-grader/tryAsgn4
