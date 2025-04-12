BIN ?= keypressed
DEV ?= FALSE
ifeq (${DEV}, TRUE)
	BUILD_CMD = ${CC} *.c -o ${BIN} -fsanitize=address -Wall
else
	BUILD_CMD = ${CC} *.c -o ${BIN}
endif

compile: *.c
	${BUILD_CMD}

run: compile
	./${BIN}
