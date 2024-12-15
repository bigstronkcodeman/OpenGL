CXX = g++
CXXFLAGS = -Wall -g
LDFLAGS = -lglfw -lGL

TARGET = build-output/opengl-fun
SRCS = src/main.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean

