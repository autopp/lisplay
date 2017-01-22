ifndef VALUE
VALUE = union
endif

.PHONY: all clean test

all:
	@$(MAKE) VALUE=$(VALUE) -C src all

test: all
	@test/run_test.sh src/lisplay

clean:
	$(MAKE) -C src clean
