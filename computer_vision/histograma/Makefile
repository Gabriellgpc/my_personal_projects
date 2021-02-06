CXXFLAGS	:=	-Wall	`pkg-config --cflags opencv`\
										`pkg-config --cflags GraphicsMagick++`
CXX	=	g++
LDLIBS	:=	`pkg-config --libs opencv`\
						`pkg-config --libs GraphicsMagick++`

.cpp:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)
