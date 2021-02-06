CXXFLAGS	:=	-Wall	`pkg-config --cflags opencv`
CXX	=	g++
LDLIBS	:=	`pkg-config --libs opencv` #inclui todas as libs do opencv

.cpp:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)