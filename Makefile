CC = "gcc"
CCOPTS = -Wall -Wextra -I./include/ `pkg-config --cflags glib-2.0`
BINDIR = /usr/bin/
LIBS = -lm `pkg-config --libs glib-2.0`
TESTDEF = -D TEST
TESTLIBS = bcdebug.o
TESTSRC =  src/bcdebug.c

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
bin_test:
	make lib_test
	${CC} ${TESTDEF} ${CCOPTS} -o bcinterpreter src/bcinterpreter.c brainycake.o stack.o registry.o ${LIBS} ${TESTLIBS}
	${CC} ${TESTDEF} ${CCOPTS} -o bcc src/bcc.c brainycake.o stack.o registry.o ${LIBS} ${TESTLIBS}

build_test:
	make lib_test
	make bin
	./runtests.sh bin
	make clean_test
lib_test:
	${CC} ${TESTDEF} ${CCOPTS} -c -o ${TESTLIBS} ${TESTSRC}
	${CC} ${TESTDEF} ${CCOPTS} -c -o brainycake.o src/brainycake.c ${TESTLIBS}
	${CC} ${TESTDEF} ${CCOPTS} -c -o stack.o src/stack.c ${TESTLIBS}
	${CC} ${TESTDEF} ${CCOPTS} -c -o registry.o src/registry.c ${TESTLIBS}

	${CC} ${CCOPTS} -o registry_test lib_tests/registry_test.c registry.o ${TESTLIBS}
	${CC} ${CCOPTS} -o stack_test lib_tests/stack_test.c stack.o ${TESTLIBS}

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
