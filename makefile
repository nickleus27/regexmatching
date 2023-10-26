CXX = clang++
CXXFLAGS = -std=c++17
CC = clang
CCFLAGS = -std=c17

all: reg_match reg_ex

reg_match: reg_match.cpp
	$(CXX) $(CXXFLAGS) -o reg_match reg_match.cpp

reg_ex: reg_ex.c
	$(CC) $(CCFLAGS) -o reg_ex reg_ex.c

clean:
	rm -rf reg_match reg_ex reg_match_debug reg_ex_debug