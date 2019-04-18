CC=g++
CPPFLAGS=-std=c++11 -O3
DBGCPPFLAGS=-std=c++11 -g -DDEBUG
SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
DBGOBJS=$(patsubst %.cpp,%.dbg.o,$(SRCS))

.PHONY: test clean

test: $(OBJS)
	$(CC) -o $@ $(OBJS) world_amazon.pb.cc -L/usr/local/lib -lprotobuf -std=c++11
%.o:%.cpp
	$(CC) $(CPPFLAGS) -o $@ -c $<
debug: $(DBGOBJS)
	$(CC) -o $@ $(DBGOBJS) world_amazon.pb.cc -L/usr/local/lib -lprotobuf -std=c++11
%.dbg.o:%.cpp
	$(CC) $(DBGCPPFLAGS) -o $@ -c $<
clean:
	rm *.o *.dbg.o test debug
format:
	../script/format.sh .

