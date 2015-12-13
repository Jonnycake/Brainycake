CC = "gcc"
CCOPTS = -Wall -I./include/
BINDIR = /usr/bin/
all:
	make build
	make clean
bin:
	${CC} ${CCOPTS} -o bcinterpreter src/bcinterpreter.c brainycake.o stack.o registry.o
	${CC} ${CCOPTS} -o bcc src/bcc.c brainycake.o stack.o registry.o	
build:
	make lib
	make bin
clean:
	rm *.o
lib:
	${CC} ${CCOPTS} -c -o brainycake.o src/brainycake.c
	${CC} ${CCOPTS} -c -o stack.o src/stack.c
	${CC} ${CCOPTS} -c -o registry.o src/registry.c
lib_test:
	make lib
	${CC} ${CCOPTS} -o registry_test lib_tests/registry_test.c registry.o
	${CC} ${CCOPTS} -o stack_test lib_tests/stack_test.c stack.o
clean_test:
	make clean
	rm *_test
install:
	cp bcinterpreter ${BINDIR}
