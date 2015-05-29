# Elisei Alexandru 316CC
# Structuri de Date
# Tema 2

CC = "gcc"
override CFLAGS += "-Wall"
PROG = "words"

HEADERS = common.h			\
	  list.h			\
	  wgraph.h			\
	  pqueue.h			\
	  tree.h

SOURCES = main.c			\
	  $(HEADERS:%.h=%.c)

OBJS = $(SOURCES:%.c=%.o)

.PHONY: build clean

build: $(PROG)

$(PROG): $(OBJS) $(HEADERS)
	$(CC) $(OBJS) -lm -o $(PROG) $(CFLAGS)
	
%.o: %.c
	$(CC) -g -c $^ -o $@ $(CFLAGS) -O0

clean:
	rm -rf a.out $(PROG) $(OBJS)
