# University ERP System - OOPD Assignment

## Overview
This is a comprehensive Enterprise Resource Planning (ERP) system for university student management, built with C++17. The system demonstrates advanced object-oriented programming concepts including data abstraction, polymorphism, parallel processing, and efficient data structures.

## Features

### 1. Flexible Data Types (Requirement 1)
- **Student Class**: Supports multiple data types for roll numbers (uint64_t or string) and course IDs (int or string)
- Uses C++17 `std::variant` for type flexibility
- Implements proper data abstraction and hiding principles
- Encapsulates student information: name, roll number, branch, start year, courses, and grades

### 2. Multi-University Support (Requirement 2)
- Handles IIIT-Delhi's string-based course codes (e.g., "CS101", "MATH101")
- Handles IIT-Delhi's integer-based course numbers (e.g., 101, 202)
- Seamlessly works with mixed course ID types in the same system

### 3. Parallel Sorting (Requirement 3)
- Uses multi-threading (2 threads) to sort 3000+ student records
- No race conditions - thread-safe implementation
- Logs individual thread execution times and total time with merge
- Sorts by student name (primary) and roll number (secondary)

### 4. Multiple Iterator Types (Requirement 4)
- **const_iterator**: For displaying records in input order
- **const_iterator**: For displaying records in sorted order
- **const_reverse_iterator**: For displaying records in reverse sorted order
- No data copying - uses index vectors for efficiency

### 5. Fast Course-Grade Queries (Requirement 5)
- **CourseIndex**: Hash-based data structure for O(1) course lookup
- Pre-sorted student lists per course by grade (descending)
- Quickly finds students with grade ≥ threshold without linear search
- Ideal for placement company queries

## File Structure

```
.
├── main.cpp              # Main ERP system implementation
├── gen_students.cpp      # CSV generator for test data
├── Makefile             # Build automation
└── README.md            # This file
```

## Requirements

- **Compiler**: g++ with C++17 support
- **Libraries**: Standard C++ library, pthread
- **OS**: Linux/Unix (tested on WSL)

## Building the Project

### Quick Build
```bash
make
```

### Build Everything (including generator)
```bash
make build_all
```

## Running the System

### For Demo (Minimal Commands)

#### Quick Demo (50 students)
```bash
make quick
```
This single command will:
1. Build all executables
2. Generate 50 student records
3. Run the ERP system with menu interface

#### Full Demo (3000 students)
```bash
make demo
```
This single command will:
1. Build all executables
2. Generate 3000 student records
3. Run the ERP system with menu interface

### Manual Run
```bash
# Generate student data
make gen              # Generates 3000 students
# OR
make gen50            # Generates 50 students

# Run the ERP system
./erp students.csv
```

## Menu Options

Once the system starts, you'll see an interactive menu:

```
========================================
       UNIVERSITY ERP SYSTEM
========================================
1. Show records in input order
2. Show records in sorted order
3. Show records in reverse sorted order
4. Query students by course and grade
5. Show full details of a student
6. Export sorted data
7. Show statistics
0. Exit
========================================
```

### Option Descriptions

1. **Show records in input order**: Displays students as they appear in the CSV file (uses const_iterator)
2. **Show records in sorted order**: Displays students sorted alphabetically by name (uses const_iterator)
3. **Show records in reverse sorted order**: Displays students in reverse alphabetical order (uses const_reverse_iterator)
4. **Query students by course and grade**: Find students with grade ≥ threshold in a specific course (demonstrates fast lookup)
5. **Show full details of a student**: View complete information including all courses and grades
6. **Export sorted data**: Save sorted student list to `sorted_indices.txt`
7. **Show statistics**: Display system statistics (total students, courses, branches)

## Example Usage

### Query Students by Course (Requirement 5)
```
Enter choice: 4

Available courses:
  CS101
  MATH101
  101
  202

Enter course ID (e.g., CS101 or 101): CS101
Enter minimum grade (e.g., 9.0): 9.0

Students with grade >= 9.0 in course CS101:
1. [2019001] Aaditya Sharma (CSE, 2019) - Grade: 9.5
2. [2019045] Ravi Kumar (CSE, 2019) - Grade: 9.2
...
```

## CSV Format

The system expects CSV files with the following format:
```
roll_number,name,branch,start_year,current_courses,previous_courses
```

Example:
```csv
2019001,Aaditya Shinde,CSE,2019,CS101:9.2;MATH101:8.5,PHYS101:7.8
BT-20-005,Neha Sharma,EE,2020,101:9.5;202:8.0,101:8.7
```

Course format: `CourseID:Grade;CourseID:Grade`

## Key Design Decisions

### Data Abstraction
- All member variables are private
- Public interface provides controlled access through getters
- Implementation details hidden from users

### Type Flexibility
- `std::variant` allows roll numbers to be either uint64_t or string
- `std::variant` allows course IDs to be either int or string
- Custom hash and equality functors for unordered_map support

### Performance Optimization
- Index vectors avoid copying entire Student objects
- Course index provides O(1) lookup by course ID
- Pre-sorted course lists enable fast threshold queries
- Parallel sorting reduces computation time

### Thread Safety
- Each thread sorts independent data ranges
- No shared mutable state during parallel sort
- Join before merge ensures proper synchronization

## Cleaning Up

Remove all generated files:
```bash
make clean
```

This removes:
- Compiled executables (erp, gen_students)
- Generated CSV files
- Output files
- Object files

## Assignment Requirements Checklist

- ✅ **Q1**: Single Student class with flexible types (variant)
- ✅ **Q2**: Handles both IIIT-Delhi (string) and IIT-Delhi (int) course codes
- ✅ **Q3**: Parallel sorting with 2+ threads, timing logs, no race conditions
- ✅ **Q4**: Multiple iterator types (const, const_reverse) for different orderings
- ✅ **Q5**: Fast course-grade query using CourseIndex (no linear search)
- ✅ Data abstraction and hiding principles applied
- ✅ Menu-driven interface for easy demonstration
- ✅ Comprehensive README
- ✅ Makefile for easy building
- ✅ No binary files in repository

## Testing

The system has been tested with:
- Small datasets (10-50 records) for quick validation
- Large datasets (3000+ records) for performance testing
- Mixed course ID types (string and integer)
- Mixed roll number types (string and uint64_t)
- Various course grade queries

## Performance Notes

On a typical modern system with 3000 records:
- Thread 1 sort time: ~15-30 ms
- Thread 2 sort time: ~15-30 ms
- Total time (with merge): ~30-50 ms
- Course query: < 1 ms (O(1) lookup + O(k) where k = results)

## Author Notes

This implementation emphasizes:
- Clean, maintainable code
- Proper OOP principles
- Efficient algorithms and data structures
- User-friendly interface
- Comprehensive documentation

Ready for demonstration with minimal setup commands!



# Demo Guide - University ERP System

## Quick Start (Minimal Commands)

### Option 1: Fully Automated Demo
```bash
# For quick demo with 50 students
make quick

# OR for full demo with 3000 students
make demo
```

**That's it!** This single command does everything:
- Builds all executables
- Generates student data
- Launches the interactive menu

---

## What to Show During Demo

### 1. Start the System
```bash
make demo
```

You'll see:
```
Building ERP system...
Building student generator...
Generating 3000 students...

========================================
    LOADING STUDENT DATA
========================================
CSV File: students.csv
Successfully loaded 3000 records.

========================================
    PERFORMING PARALLEL SORT
========================================
Thread 1 time: 25 ms
Thread 2 time: 27 ms
Total time (including merge): 54 ms
Sort completed successfully.
```

**Explain**: This demonstrates **Requirement 3** - parallel sorting with 2 threads

---

### 2. Show Input Order (Requirement 4 - Iterator Type 1)
```
Enter choice: 1
```

This displays records as they were loaded from CSV using **const_iterator**

**Explain**: "We use const_iterator to traverse the input order without copying data"

---

### 3. Show Sorted Order (Requirement 4 - Iterator Type 2)
```
Enter choice: 2
```

This displays records sorted alphabetically by name using **const_iterator**

**Explain**: "After parallel sorting, we use const_iterator to show sorted records efficiently"

---

### 4. Show Reverse Order (Requirement 4 - Iterator Type 3)
```
Enter choice: 3
```

This displays records in reverse sorted order using **const_reverse_iterator**

**Explain**: "We use const_reverse_iterator to traverse in reverse without copying"

---

### 5. Query by Course and Grade (Requirement 5)
```
Enter choice: 4
Enter course ID: CS101
Enter minimum grade: 9.0
```

This shows students with grade ≥ 9.0 in CS101 **instantly** (no linear search!)

**Explain**: 
- "We built a CourseIndex - a hash map with O(1) lookup"
- "Students are pre-sorted by grade for each course"
- "This allows placement companies to quickly find qualified students"
- "No linear search needed - much faster than scanning all 3000 records"

**Demonstrate it works with both course types:**
```
Try again with: 101 (integer course ID from IIT-Delhi)
Enter minimum grade: 8.5
```

This shows **Requirement 2** - handling both IIIT-Delhi (string) and IIT-Delhi (int) course IDs

---

### 6. Show Full Student Details (Requirement 1)
```
Enter choice: 5
Enter student index: 1
```

This displays complete information for a student

**Explain**:
- "The Student class uses std::variant for flexible data types"
- "Roll numbers can be uint64_t or string"
- "Course IDs can be int or string"
- "This single class works for all universities (Requirement 1)"

---

### 7. Show Statistics
```
Enter choice: 7
```

Quick overview of the system

---

### 8. Exit
```
Enter choice: 0
```

---

## Key Points to Emphasize

### ✅ Requirement 1: Flexible Data Types
- **Show**: Student with numeric roll (2019001) and string roll (BT-20-0045)
- **Show**: Course CS101 (string) and course 101 (int)
- **Code**: Point to `variant<uint64_t, string>` and `variant<int, string>`

### ✅ Requirement 2: Multi-University Support
- **Show**: Query CS101 (IIIT-Delhi format)
- **Show**: Query 101 (IIT-Delhi format)
- **Explain**: "Same system handles both universities seamlessly"

### ✅ Requirement 3: Parallel Sorting
- **Show**: Thread timing output at startup
- **Code**: Point to `parallel_sort_indices()` function with 2 threads
- **Explain**: "No race conditions - each thread sorts independent range, then merge"

### ✅ Requirement 4: Different Iterators
- **Show**: Menu options 1, 2, 3
- **Code**: Point to `cbegin()`, `cend()`, `crbegin()`, `crend()`
- **Explain**: "No data copying - we use index vectors"

### ✅ Requirement 5: Fast Course Query
- **Show**: Instant results for course query
- **Code**: Point to `CourseIndex` class
- **Explain**: "O(1) lookup instead of O(n) linear search"

---

## Code Walkthrough Points

### Data Abstraction (Throughout)
```cpp
class Student {
private:  // Data hiding
    RollID roll;
    string name;
    // ...
public:   // Controlled interface
    const RollID& get_roll() const;
    const string& get_name() const;
    // ...
};
```

### Variant Usage (Requirement 1)
```cpp
using RollID = variant<uint64_t, string>;
using CourseID = variant<int, string>;
```

### Parallel Sort (Requirement 3)
```cpp
thread th1(worker1);  // First half
thread th2(worker2);  // Second half
th1.join();
th2.join();
inplace_merge(...);   // Combine results
```

### Iterator Types (Requirement 4)
```cpp
// Input order - const_iterator
for (auto it = input_order.cbegin(); it != input_order.cend(); ++it)

// Sorted order - const_iterator
for (auto it = sorted_indices.cbegin(); it != sorted_indices.cend(); ++it)

// Reverse order - const_reverse_iterator
for (auto it = sorted_indices.crbegin(); it != sorted_indices.crend(); ++it)
```

### Course Index (Requirement 5)
```cpp
unordered_map<CourseID, vector<Student*>, CourseIDHash, CourseIDEq> idx;
// O(1) lookup by course
// Pre-sorted by grade for fast threshold queries
```

---

## Common Demo Questions & Answers

**Q: Why use variant instead of templates?**
A: Variant allows mixing different types in the same container. We can have students with numeric AND string roll numbers in one vector.

**Q: How do you avoid race conditions?**
A: Each thread works on an independent data range. No shared mutable state. Join before merge ensures synchronization.

**Q: Why use indices instead of copying?**
A: Efficiency. With 3000 students, copying would be expensive. Indices are just integers.

**Q: How is the course query so fast?**
A: Hash map provides O(1) lookup. Pre-sorting by grade means we can stop scanning once we hit the threshold.

**Q: Does this work with more universities?**
A: Yes! The variant approach works with any roll number or course ID format without code changes.

---

## If Demo Fails

### CSV file not found
```bash
make gen  # Regenerate students.csv
```

### Compilation errors
```bash
make clean
make build_all
```

### Start fresh
```bash
make clean
make demo
```

---

## Time Management

- **Total demo time**: 10-15 minutes
- **Build + load**: 1 minute
- **Show 4-5 menu options**: 8-10 minutes
- **Code walkthrough**: 3-4 minutes
- **Questions**: 2-3 minutes

---

## Final Checklist

Before demo:
- [ ] Code compiles without warnings
- [ ] All 5 requirements clearly demonstrated
- [ ] Know where each requirement is in code
- [ ] Prepared to explain OOP concepts
- [ ] GitHub repo ready (no binaries!)
- [ ] README.md is comprehensive

During demo:
- [ ] Show automated build (make demo)
- [ ] Demonstrate all menu options
- [ ] Explain parallel sort output
- [ ] Show fast course query
- [ ] Walk through key code sections
- [ ] Answer questions confidently

Good luck! 🚀