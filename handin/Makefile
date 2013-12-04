# Variables
PROXY_TESTS = test_parse_utils test_stream test_bitrate_select test_proxy_logging
DNS_TESTS = 
TESTBINS = PROXY_TESTS DNS_TESTS

# Implicit .o target

.c.o:
	gcc -c -g -Wall $<

# Explicit build targets

all: src_proxy proxy tests ${TESTBINS}

proxy_log.o: src/proxy/log.o
dns_log.o: src/dns/log.o

src_proxy: src/proxy/proxy.o src/proxy/parse_utils.o src/proxy/stream.o src/proxy/request.o

proxy: proxy.o parse_utils.o stream.o request.o proxy_log.o

clean:
	rm -f *.o proxy $(TESTBINS)

# Testing

tests: tests/test_parse_utils.o tests/test_stream.o tests/test_bitrate_select.o tests/test_proxy_logging.o

test_proxy_logging: test_proxy_logging.o proxy_log.o stream.o request.o

test_bitrate_select: test_bitrate_select.o parse_utils.o

test_parse_utils: test_parse_utils.o parse_utils.o

test_stream: test_stream.o stream.o request.o
