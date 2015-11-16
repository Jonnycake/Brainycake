CC = "gcc"
CCOPTS = -Wall -I./include/
BINDIR = /usr/bin/
all:
	make build
	make clean
build:
	make lib
	${CC} ${CCOPTS} -o bcinterpreter src/bcinterpreter.c brainycake.o
	${CC} ${CCOPTS} -o bcc src/bcc.c brainycake.o
clean:
	rm brainycake.o
lib:
	${CC} ${CCOPTS} -c -o brainycake.o src/brainycake.c
install:
	cp bcinterpreter ${BINDIR}
