TARGET = lisplay
SRCS = lisplay.c context.c heap.c value.c parser.c scanner.c lexer.c builtin.c
VALUE_DECL_H = value_decl.h
VALUE_DEF_H = value_def.h
PREV_VALUE = prev_value.txt

SRCS += $(wildcard $(VALUE)/*.c)

OBJS = $(SRCS:%.c=%.o)
DEPS = $(SRCS:%.c=%.d)

FLEX_SRC = lexer.l
FLEX_TARGET = $(FLEX_SRC:%.l=%.c)
FLEX_HEADER = $(FLEX_SRC:%.l=%.h)

CC = gcc
CFLAGS = -c -std=gnu99 -Wall -Werror -MMD -MP -g -O0
LDFLAGS = -lm -g
FLEX = flex
FLEX_FLAGS = --header-file=$(FLEX_HEADER)

.PHONY: all check_prev_value clean

all: check_prev_value $(VALUE_DECL_H) $(VALUE_DEF_H) $(FLEX_TARGET) $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(VALUE_DECL_H): $(VALUE)/$(VALUE_DECL_H)
	cat value_header_comment.txt $^ > $@

$(VALUE_DEF_H): $(VALUE)/$(VALUE_DEF_H)
	cat value_header_comment.txt $^ > $@

$(FLEX_TARGET): $(FLEX_SRC)
	$(FLEX) $(FLEX_FLAGS) --outfile=$@ $^

check_prev_value:
	@if [ "`cat $(PREV_VALUE)`" != $(VALUE) ] ; then echo "value representation is changed" rm -f $(VALUE_DECL_H) $(VALUE_DEF_H); fi
	@echo $(VALUE) > $(PREV_VALUE)

$(PREV_VALUE):
	touch $(PREV_VALUE)

clean:
	rm -f $(TARGET) $(FLEX_TARGET) $(FLEX_HEADER) $(PREV_VALUE) $(VALUE_DECL_H) $(VALUE_DEF_H) $(OBJS) $(DEPS)
