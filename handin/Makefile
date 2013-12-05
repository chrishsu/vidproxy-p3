# Variables
PROXY_TESTS = tests_proxy test_parse_utils test_stream test_bitrate_select test_proxy_logging
DNS_TESTS = tests_dns test_roundrobin
TESTBINS = ${PROXY_TESTS} ${DNS_TESTS}

# Implicit .o target

.c.o:
	gcc -c -g -Wall $<

# Explicit build targets

all: logs src_proxy proxy src_dns nameserver ${TESTBINS}

logs:
	gcc -c -g -Wall src/proxy/log.c -o proxy_log.o
	gcc -c -g -Wall src/dns/log.c -o dns_log.o


src_proxy: src/proxy/proxy.o src/proxy/parse_utils.o src/proxy/stream.o src/proxy/request.o

src_dns: src/dns/nameserver.o src/dns/roundrobin.o

proxy: proxy.o parse_utils.o stream.o request.o proxy_log.o

nameserver: nameserver.o roundrobin.o dns_log.o

clean:
	rm -f *.o proxy $(TESTBINS)

# Proxy Testing

tests_proxy: tests/test_parse_utils.o tests/test_stream.o tests/test_bitrate_select.o tests/test_proxy_logging.o

test_proxy_logging: test_proxy_logging.o proxy_log.o stream.o request.o

test_bitrate_select: test_bitrate_select.o parse_utils.o

test_parse_utils: test_parse_utils.o parse_utils.o

test_stream: test_stream.o stream.o request.o

# DNS Testing

tests_dns: tests/test_roundrobin.o

test_roundrobin: test_roundrobin.o roundrobin.o
