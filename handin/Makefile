# Variables
TESTBINS = test_parse_utils test_stream test_bitrate_select test_logging

# Implicit .o target

.c.o:
	gcc -c -g -Wall $<

# Explicit build targets

all: src proxy tests ${TESTBINS}

src: src/proxy.o src/parse_utils.o src/stream.o src/request.o src/log.o

proxy: proxy.o parse_utils.o stream.o request.o log.o

clean:
	rm -f *.o proxy $(TESTBINS)

# Testing

tests: tests/test_parse_utils.o tests/test_stream.o tests/test_bitrate_select.o tests/test_logging.o

test_logging: test_logging.o log.o stream.o request.o

test_bitrate_select: test_bitrate_select.o parse_utils.o

test_parse_utils: test_parse_utils.o parse_utils.o

test_stream: test_stream.o stream.o request.o
