BASE_OBJS = main.o circles.o

SRC_PATH = src

OBJS = $(addprefix $(SRC_PATH)/, $(BASE_OBJS))
CXX = g++ -std=c++17
CXXFLAGS = -g -Iinclude -Wall -Wextra
CXXFLAGS += `pkg-config --cflags opencv4`
LIBS = `pkg-config --libs opencv4`

Segment: $(OBJS)
	$(CXX) $^ -o $@ $(LIBS)

clean:
	rm $(OBJS)
	rm Segment
	rm *.txt