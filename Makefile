.PHONY: all clean

all:
	$(MAKE) VALUE_DEF=$(VALUE_DEF) -C src all

clean:
	$(MAKE) -C src clean
