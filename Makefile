CC = "gcc"
CCOPTS = -Wall -Wextra -I./include/ `pkg-config --cflags glib-2.0`
BINDIR = /usr/bin/
LIBS = -lm `pkg-config --libs glib-2.0`
all:
	make build
	make clean
bin:
	${CC} ${CCOPTS} -o bcinterpreter src/bcinterpreter.c brainycake.o stack.o registry.o ${LIBS}
	${CC} ${CCOPTS} -o bcc src/bcc.c brainycake.o stack.o registry.o ${LIBS}
build:
	make lib
	make bin
lib:
	${CC} ${CCOPTS} -c -o brainycake.o src/brainycake.c
	${CC} ${CCOPTS} -c -o stack.o src/stack.c
	${CC} ${CCOPTS} -c -o registry.o src/registry.c
install:
	cp bcinterpreter ${BINDIR}
	cp bcc ${BINDIR}

#########
# Tests #
#########
build_test:
	make lib_test
	make bin
	./runtests.sh bin
	make clean_test
lib_test:
	make lib
	${CC} ${CCOPTS} -o registry_test lib_tests/registry_test.c registry.o
	${CC} ${CCOPTS} -o stack_test lib_tests/stack_test.c stack.o

############
# Cleaning #
############
clean_build:
	rm bcc
	rm bcinterpreter
clean:
	rm *.o
clean_test:
	rm *_test
	make clean
