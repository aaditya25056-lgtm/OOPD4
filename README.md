University ERP System - OOPD Assignment

Aaditya Shinde
MT25056

📌 Overview

This is a robust, persistent, and modular Enterprise Resource Planning (ERP) system designed for university student management. Built using C++17, it demonstrates advanced Object-Oriented Programming concepts, multi-threading, and efficient data handling.

The system is designed to handle data from multiple universities (e.g., IIIT-Delhi and IIT-Delhi) with differing data formats for Roll Numbers and Course Codes, utilizing std::variant for type flexibility.

🚀 Key Features & Requirements Met

1. Flexible Data Types (Polymorphism via std::variant)

Roll Numbers: Supports both Integer (e.g., 2023001) and String (e.g., MT19001) formats.

Course Codes: Supports both String (e.g., CS101) and Integer (e.g., 101) formats.

Requirement: Implemented in Types.h and Student.h.

2. Parallel Processing (Multi-threading)

Implements a Parallel Merge Sort using std::thread.

Splits the dataset into two halves, sorts them concurrently on separate threads, and merges the results.

Includes a custom ThreadTimer to benchmark execution speed.

Requirement: Implemented in ERPUtils.cpp.

3. Fast Indexing & Search

Builds an in-memory Course Index (unordered_map) for O(1) access to course data.

Students within the index are pre-sorted by grade to allow rapid querying of top performers without linear scanning.

Requirement: Implemented in CourseIndex.cpp.

4. Robust Error Handling

A dedicated InputValidator class ensures the system never crashes due to bad input.

Validates integers, 4-digit years, non-empty strings, and grade ranges (0.0 - 10.0).

Feature: Implemented in InputValidator.cpp.

5. Data Persistence (CRUD Operations)

Create: Add new students manually.

Read: Load and display students from CSV.

Delete: Permanently remove students by Roll Number.

Auto-Save: All changes are immediately written to students.csv.

📂 Project Structure

The project follows a modular architecture to separate concerns:

ERP_Project/
├── Makefile             # Automation for building and running
├── main.cpp             # Application entry point and menu logic
├── gen_students.cpp     # Utility to generate dummy CSV data
├── Types.h              # Shared type definitions (RollID, CourseID)
├── Student.h/cpp        # Student class (Core Data)
├── CourseIndex.h/cpp    # Searching & Indexing Logic
├── ERPUtils.h/cpp       # File I/O and Parallel Sort Logic
└── InputValidator.h/cpp # Input sanitization and validation


⚙️ How to Build and Run

Prerequisites

Compiler: g++ with C++17 support.

OS: Linux / macOS / WSL (Windows).

1. Full Demo (3000 Records)

Use this command to generate a large dataset and start the system:

make demo


Generates 3000 random records and launches the menu.

2. Quick Test (50 Records)

Use this for quick debugging or small-scale testing:

make quick


3. Normal Run (Keep Data)

Important: The make commands above regenerate the CSV file (wiping old data). To run the program and keep your saved changes, use:

./erp students.csv


4. Cleanup

To remove compiled object files (.o) and executables:

make clean


📖 Usage Guide

The system presents an interactive menu:

Options 1-2: Create new students. The system will detect if you are in IIIT Mode (Int Roll) or IIT Mode (String Roll).

Option 3: Display all loaded students. You can limit the output (e.g., "Show top 10").

Option 6: Run the Parallel Sort and see the timing difference between threads.

Option 9: Run a fast query to find high-performing students in a specific course.

Option 11: Permanently delete a student by Roll Number.

📝 CSV Format

The system reads and writes to students.csv in the following format:

RollNumber, Name, Branch, StartYear, CurrentCourses, PreviousCourses


Courses are semicolon-separated: CS101:9.0;MATH101:8.5

Example:

2023001,John Doe,CSE,2023,CS101:9.5;CS102:8.0,MATH100:7.5


🛡️ OOP Design Decisions

Encapsulation: The Student class manages its own data. Private members are accessed only via getters/setters.

Single Responsibility Principle:

InputValidator handles only input.

ERPUtils handles only file/sort operations.

CourseIndex handles only search logic.

Abstraction: The main.cpp file doesn't know how sorting or file writing happens; it just calls high-level functions.

