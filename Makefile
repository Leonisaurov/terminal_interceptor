BIN ?= terminal_interceptor
DEV ?= FALSE

STRINGS_DIR = ./libs/BSL
STRINGS_NAME = libstrings.so
STRINGS_LIB = ${STRINGS_DIR}/${STRINGS_NAME}

LIBRARIES = -L${STRINGS_DIR} -lstrings
INCLUDES = -I./libs/BSL

ifeq (${DEV}, TRUE)
	BUILD_CMD = ${CC} *.c ${INCLUDES} ${LIBRARIES} -o ${BIN} -fsanitize=address -Wall
else
	BUILD_CMD = ${CC} *.c ${INCLUDES} ${LIBRARIES} -o ${BIN}
endif

${STRINGS_LIB}:
	@echo "Compiling submodule. . ."
	${MAKE} ${STRINGS_NAME} -C ${STRINGS_DIR}

compile: ${STRINGS_LIB} *.c
	${BUILD_CMD}

run: compile
	./${BIN}

.PHONY: run compile library
