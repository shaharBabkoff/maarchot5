# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -O0

# Directories
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/mst_project

# Source files
SRCS = Graph.cpp MSTree.cpp MSTStrategy.cpp union_find.cpp main.cpp pollserver.cpp listner.cpp execute_commands.cpp tcp_client_thread_pool.cpp pipeline.cpp LeaderFollowerThreadPool.cpp

# Object files (placed in bin/)
OBJS = $(SRCS:%.cpp=$(BIN_DIR)/%.o)

# Header files
HEADERS = Graph.hpp MSTree.hpp MSTStrategy.hpp union_find.hpp LeaderFollowerThreadPool.hpp listner.hpp pipeline.hpp execute_commands.hpp tcp_client_thread_pool.hpp

# Ensure the bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Default target
all: $(BIN_DIR) $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files (in bin/)
$(BIN_DIR)/%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(BIN_DIR)/*.o $(TARGET)
