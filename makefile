CXX      = g++
CXXFLAGS = -Wall -std=c++17
TARGET   = makeCave


SRCS = src/makeCave.cpp src/biomeData.cpp

# Build files
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)