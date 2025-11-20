CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread -Wall -Wextra

TARGET = erp
SRC = main.cpp

GEN = gen_students
GEN_SRC = gen_students.cpp

.PHONY: all build_all gen gen50 gen3000 demo quick clean

# Default target
all: $(TARGET)

# Build main ERP program
$(TARGET): $(SRC)
	@echo "Building ERP system..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)
	@echo "Build complete!"

# Build student generator
$(GEN): $(GEN_SRC)
	@echo "Building student generator..."
	$(CXX) $(CXXFLAGS) -o $(GEN) $(GEN_SRC)
	@echo "Generator build complete!"

# Build everything
build_all: $(TARGET) $(GEN)
	@echo "All builds complete!"

# Generate students
gen: $(GEN)
	@echo "Generating 3000 students..."
	./$(GEN) 3000
	@echo "Generated students.csv"

gen50: $(GEN)
	@echo "Generating 50 students..."
	./$(GEN) 50
	@echo "Generated students.csv"

gen3000: gen

# Quick demo (auto-run with small dataset)
quick: build_all gen50
	@echo "\n========================================="
	@echo "Running quick demo with 50 students..."
	@echo "========================================="
	./$(TARGET) students.csv

# Full demo (auto-run with 3000 students)
demo: build_all gen
	@echo "\n========================================="
	@echo "Running full demo with 3000 students..."
	@echo "========================================="
	./$(TARGET) students.csv

# Clean all generated files
clean:
	@echo "Cleaning..."
	rm -f $(TARGET) $(GEN) students.csv sorted_indices.txt *.o
	@echo "Clean complete!"