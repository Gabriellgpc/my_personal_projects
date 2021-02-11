CXXFLAGS	:=	-Wall -O2 `pkg-config --cflags opencv`
CXX	=	g++	--std=c++11
LDLIBS	:=	`pkg-config --libs opencv` #inclui todas as libs do opencv

all: main

main:	main.cpp	artist.o

artist.o:	artist.hpp artist.cpp

