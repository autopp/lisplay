TARGET = lisplay
SRCS = lisplay.c context.c heap.c value.c parser.c scanner.c lexer.c
VALUE_DEF_H = value_def.h
PREV_VALUE = prev_value.txt
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

ifndef VALUE
	VALUE = union
endif

SRCS += $(wildcard $(VALUE)/*.c)

.PHONY: all check_prev_value clean

all: check_prev_value $(VALUE_DEF_H) $(FLEX_TARGET) $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CC) $(CFLAGS) $<

$(VALUE_DEF_H): $(VALUE)/$(VALUE_DEF_H)
	cp $^ $@

$(FLEX_TARGET): $(FLEX_SRC)
	$(FLEX) $(FLEX_FLAGS) --outfile=$@ $^

check_prev_value:
	if [ "`cat $(PREV_VALUE)`" != $(VALUE) ] ; then rm -f $(VALUE_DEF_H); fi
	echo $(VALUE) > $(PREV_VALUE)

$(PREV_VALUE):
	touch $(PREV_VALUE)

clean:
	rm -f $(TARGET) $(FLEX_TARGET) $(PREV_VALUE) $(VALUE_DEF_H) $(OBJS) $(DEPS)
