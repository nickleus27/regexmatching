CXX = clang++
CXXFLAGS = -std=c++17
CC = clang
CCFLAGS = -std=c17
RULES = regmatch2

ifneq (,$(wildcard ./reg_ex.c))
RULES+=reg_ex
endif

all: $(RULES)

reg_ex: reg_ex.c
	$(CC) $(CCFLAGS) -o reg_ex reg_ex.c

regmatch2: regmatch2.cpp
	$(CXX) $(CXXFLAGS) -o regmatch2 regmatch2.cpp

clean:
	rm -rf  reg_ex  regmatch2 reg_ex_debug regmatch2_debug