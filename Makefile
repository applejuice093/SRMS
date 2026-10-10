CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2

SRMS: SRMS.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test: SRMS
	./tests/smoke_test.sh

clean:
	rm -f SRMS SRMS.exe

.PHONY: test clean
