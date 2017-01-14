TARGET = lisplay
SRCS = lisplay.c context.c heap.c value.c parser.c scanner.c lexer.c
OBJS = $(SRCS:%.c=%.o)
DEPS = $(SRCS:%.c=%.d)

FLEX_SRC = lexer.l
FLEX_TARGET = $(FLEX_SRC:%.l=%.c)
FLEX_HEADER = $(FLEX_SRC:%.l=%.h)

CC = gcc
CFLAGS = -c -std=gnu99 -Wall -Werror -MMD -MP
LDFLAGS = -lm
FLEX = flex
FLEX_FLAGS = --header-file=$(FLEX_HEADER)

.PHONY: all clean

all: $(FLEX_TARGET) $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CC) $(CFLAGS) $<

$(FLEX_TARGET): $(FLEX_SRC)
	$(FLEX) $(FLEX_FLAGS) --outfile=$@ $^

clean:
	rm -f $(TARGET) $(FLEX_TARGET) $(OBJS) $(DEPS)
