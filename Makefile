
CC = gcc
CFLAGS = -Wall -Wextra
LIBS =  -L ./NtyCo/ -lntyco -lpthread -ldl
INC = -I ./NtyCo/core

SRCS = kvstore.c rbtree.c hash.c skiptable.c dhash.c log.c
TESTCASE_SRCS = testcase.c

SUBDIR = NtyCo/
TESTCASE = testcase

OBJS = $(SRCS:.c=.o)

TARGET = kvstore

all: $(SUBDIR) $(TARGET) $(TESTCASE)
.PHONY : all

$(SUBDIR): ECHO
	make -C $@ 
ECHO:
	@echo $(SUBDIR)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(INC) $(LIBS)

$(TESTCASE): $(TESTCASE_SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(INC)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET) $(TESTCASE)
