# Variables
TESTBINS = test_parse_utils

# Implicit .o target

.c.o:
	gcc -c -g -Wall $<

# Explicit build targets

all: tests ${TESTBINS}

clean:
	rm -f *.o $(TESTBINS)

# Testing

tests: tests/test_parse_utils.o

test_parse_utils: test_parse_utils.o parse_utils.o
