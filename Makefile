CC = "gcc"
CCOPTS = -Wall -I./include/ -o brainycake
BINDIR = /usr/bin/
all:
	make build
build:
	${CC} ${CCOPTS} src/main.c

clean:
	rm brainycake
install:
	cp brainycake ${BINDIR}
