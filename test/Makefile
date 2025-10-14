BIN ?= terminal_interceptor
ARGS ?=
DEV ?= FALSE
CFLAGS ?= -I..
ifneq (${DEV}, FALSE)
	ifeq (${DEV},GDB)
		CFLAGS += -g
	endif
	CFLAGS += -Wall -Ddebug
endif

SRCS	=$(wildcard *.c)
OBJS	=$(SRCS:.c=.o)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

build-lib:
	$(MAKE) -C .. build-lib

$(BIN): $(OBJS) build-lib
	$(CC) -o $@ $(OBJS) ../*.o $(CFLAGS)

run: $(BIN)
	@echo "Running"
	./$(BIN) $(ARGS)
	@echo "The program ends."

clean:
	$(MAKE) -C .. clean
	rm -rfv $(BIN) log.txt
