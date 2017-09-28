CC=gcc
CXX=g++

SRC=$(filter-out mainParent.cpp,$(wildcard *.cpp))
STATIC_OBJ=$(SRC:%.cpp=%.o)
CFLAGS+=-O3
CXXFLAGS+=-O3
INC_PATH=-I.


.PHONY:
all: mainParent ChildA

mainParent : mainParent.cpp
	$(CXX) -o $@ $<

ChildA : childA.cpp
	$(CXX) -o $@ $<

clean :
	rm mainParent ChildA
