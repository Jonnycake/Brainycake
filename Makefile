CC = "gcc"
CCOPTS = -Wall -I./include/
BINDIR = /usr/bin/
all:
	make build
	make clean
build:
	make lib
	${CC} ${CCOPTS} -o bcinterpreter src/bcinterpreter.c brainycake.o stack.o registry.o
	${CC} ${CCOPTS} -o bcc src/bcc.c brainycake.o stack.o registry.o
clean:
	rm *.o
lib:
	${CC} ${CCOPTS} -c -o brainycake.o src/brainycake.c
	${CC} ${CCOPTS} -c -o stack.o src/stack.c
	${CC} ${CCOPTS} -c -o registry.o src/registry.c
lib_test:
	make lib
	${CC} ${CCOPTS} -o registry_test lib_tests/registry_test.c registry.o
install:
	cp bcinterpreter ${BINDIR}
