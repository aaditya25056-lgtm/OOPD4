CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread -Wall -Wextra

# List of object files needed for the main program
OBJS = main.o Student.o InputValidator.o ERPUtils.o CourseIndex.o

# Default target
all: erp gen_students

# Main executable linkage
erp: $(OBJS)
	$(CXX) $(CXXFLAGS) -o erp $(OBJS)

# Generator executable
gen_students: gen_students.cpp
	$(CXX) $(CXXFLAGS) -o gen_students gen_students.cpp

# Individual File Compilations
main.o: main.cpp Types.h Student.h InputValidator.h ERPUtils.h CourseIndex.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Student.o: Student.cpp Student.h Types.h
	$(CXX) $(CXXFLAGS) -c Student.cpp

InputValidator.o: InputValidator.cpp InputValidator.h Types.h
	$(CXX) $(CXXFLAGS) -c InputValidator.cpp

ERPUtils.o: ERPUtils.cpp ERPUtils.h Student.h InputValidator.h
	$(CXX) $(CXXFLAGS) -c ERPUtils.cpp

CourseIndex.o: CourseIndex.cpp CourseIndex.h Student.h
	$(CXX) $(CXXFLAGS) -c CourseIndex.cpp

# Clean up
clean:
	rm -f erp gen_students *.o students.csv

# Quick Demo (50 Students)
quick: all
	./gen_students 50
	./erp students.csv

# Full Demo (3000 Students)
demo: all
	./gen_students 3000
	./erp students.csv