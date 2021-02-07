CXXFLAGS	:=	-Wall	`pkg-config --cflags opencv4`
CXX	=	g++
LDLIBS	:=	`pkg-config --libs opencv4` #inclui todas as libs do opencv

all:	main

main:	main.cpp	myUtils.o

myUtils.o:	myUtils.hpp	myUtils.cpp