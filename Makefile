CC = "gcc"
CCOPTS = -Wall -I./include/
BINDIR = /usr/bin/
all:
	make build
build:
	make lib
	${CC} ${CCOPTS} -o brainycake src/main.c brainycake.o
clean:
	rm brainycake.o
lib:
	${CC} ${CCOPTS} -c -o brainycake.o src/brainycake.c
install:
	cp brainycake ${BINDIR}
