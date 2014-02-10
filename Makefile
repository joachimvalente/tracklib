#-----------------------------------
PROGRAM = tracklib-demo

INCPATH = -I/opt/local/include

LIBPATH = -L/opt/local/lib
LIBS =  -lopencv_core -lopencv_highgui
#-----------------------------------

SRCS = $(wildcard *.cpp)

CXX = g++
CXXFLAGS = -O2 -g -Wall -fmessage-length=0 

OBJS = $(PROGRAM).o

TARGET = $(PROGRAM)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCPATH) $(LIBPATH) -o $(TARGET) $(SRCS) $(LIBS) 

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)