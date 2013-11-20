# Variables
TESTBINS = test_parse_utils test_stream

# Implicit .o target

.c.o:
	gcc -c -g -Wall $<

# Explicit build targets

all: proxy tests ${TESTBINS}

proxy: proxy.o

clean:
	rm -f *.o proxy $(TESTBINS)

# Testing

tests: tests/test_parse_utils.o tests/test_stream.o

test_parse_utils: test_parse_utils.o parse_utils.o

test_stream: test_stream.o stream.o request.o