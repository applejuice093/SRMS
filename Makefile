CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -O2

all: SRMS

SRMS: SRMS.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Run through bash so the test works even if the exec bit was lost
# (for example after copying the repo from Windows).
test: SRMS
	bash tests/smoke_test.sh

clean:
	rm -f SRMS SRMS.exe

.PHONY: all test clean
