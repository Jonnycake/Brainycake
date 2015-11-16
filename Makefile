CC = "gcc"
CCOPTS = -Wall -I./include/
BINDIR = /usr/bin/
all:
	make build
	make clean
build:
	make lib
	${CC} ${CCOPTS} -o bcinterpreter src/bcinterpreter.c brainycake.o stack.o
	${CC} ${CCOPTS} -o bcc src/bcc.c brainycake.o stack.o
clean:
	rm *.o
lib:
	${CC} ${CCOPTS} -c -o brainycake.o src/brainycake.c
	${CC} ${CCOPTS} -c -o stack.o src/stack.c
install:
	cp bcinterpreter ${BINDIR}
