CXX = clang++
CXXFLAGS = -std=c++17
CC = clang
CCFLAGS = -std=c17
RULES = regmatch

ifneq (,$(wildcard ./reg_ex.c))
RULES+=reg_ex
endif

all: $(RULES)

reg_ex: reg_ex.c
	$(CC) $(CCFLAGS) -o reg_ex reg_ex.c

regmatch: regmatch.cpp
	$(CXX) $(CXXFLAGS) -o regmatch regmatch.cpp

clean:
	rm -rf reg_ex regmatch reg_ex_debug regmatch_debug