CC=gcc
CXX=g++

SRC=$(filter-out mainParent.cpp,$(wildcard *.cpp))
STATIC_OBJ=$(SRC:%.cpp=%.o)
CFLAGS+=-O3
CXXFLAGS+=-std=c++11
INC_PATH=-I.


.PHONY:
all: mainParent ChildA ChildB

mainParent : mainParent.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

ChildA : childA.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

ChildB : childB.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean :
	rm mainParent ChildA ChildB
