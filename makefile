CXX = clang++
CXXFLAGS = -std=c++17

all: reg_match

reg_match: reg_match.cpp
	$(CXX) $(CXXFLAGS) -o reg_match reg_match.cpp

clean:
	rm -rf reg_match reg_match_debug