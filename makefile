# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Target executable
TARGET = mst_project

# Source files
SRCS = Graph.cpp MSTree.cpp MSTStrategy.cpp union_find.cpp main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = Graph.hpp MSTree.hpp MSTStrategy.hpp union_find.hpp

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

# Clean up the build files
clean:
	rm -f $(OBJS) $(TARGET)
