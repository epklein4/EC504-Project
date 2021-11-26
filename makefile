BASE_OBJS = main.o

SRC_PATH = source

OBJS = $(addprefix $(SRC_PATH)/, $(BASE_OBJS))
CXX = g++ -std=c++17
CXXFLAGS = -g -Iinclude -Wall -Wextra
LDFLAGS =
LDLIBS =

Segment: $(OBJS)
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)	

clean:
	rm $(OBJS)
	rm Segment.o