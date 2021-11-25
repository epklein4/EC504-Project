BASE_OBJS = main.o

SRC_PATH = source

OBJS = $(addprefix $(SRC_PATH)/, $(BASE_OBJS))
CXX = g++
CXXFLAGS = -g -Iinclude
LDFLAGS =
LDLIBS =

Segment: $(OBJS)
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)	

clean:
	rm $(OBJS)