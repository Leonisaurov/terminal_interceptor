DEV		?= FALSE
ARGS	?=
CFLAGS	?= -fPIC
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

run-test: 
	$(MAKE) -C test run $(ARGS) DEV=$(DEV)

build-lib: $(OBJS)

clean:
	rm -rfv $(OBJS)
