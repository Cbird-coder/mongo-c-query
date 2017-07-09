CXX=g++

MONGOFACEINC = -I./include

MONGOINC=$(shell pkg-config --cflags libmongoc-1.0)
MONGOLIB=$(shell pkg-config --libs libmongoc-1.0)

CVINC=$(shell pkg-config --cflags opencv)
CVLIB=$(shell pkg-config --libs opencv)

SRC=$(wildcard src/*.cpp) 
#SRC=$(wildcard gridfs_test.cpp) 
all:
	$(CXX) -std=c++11 $(CVINC) $(MONGOFACEINC) $(MONGOINC) $(CVLIB) $(MONGOLIB)  $(SRC) -o mongodb_test
clean:
	rm -rf mongodb_test
