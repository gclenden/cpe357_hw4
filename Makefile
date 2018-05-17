CC = gcc
CFLAGS =-Wall -ansi -pedantic -g
MAIN = mytar
OBJS = mytar.o
all : $(MAIN)

$(MAIN) : $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

mytar.o: mytar.c
	$(CC) $(CFLAGS) -c mytar.c

clean :
	rm *.o $(MAIN) hdecode

handin:
	handin getaylor-grader 357hw4-11 handindir/*

test: 
	~getaylor-grader/tryAsgn4

pushmaster:
	git add *
	git commit -a
	git push origin master
