/* main.cpp - Modularized Entry Point */
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cstdlib>
#include "Types.h"
#include "Student.h"
#include "CourseIndex.h"
#include "InputValidator.h"
#include "ERPUtils.h"

using namespace std;

void displayMenu() {
    cout << "\n";
    cout << "||=======================================================================||" << endl;
    cout << "||               UNIVERSITY ERP SYSTEM - OOPD ASSIGNMENT                 ||" << endl;
    cout << "||=======================================================================||" << endl;
    cout << "|| Question 1 & 2: Template-based Student Management                     ||" << endl;
    cout << "||    1. Create IIIT-Delhi Student (int roll, string course)             ||" << endl;
    cout << "||    2. Create IIT-Delhi Student (string roll, int course)              ||" << endl;
    cout << "||    3. Display All Students (Both Universities)                        ||" << endl;
    cout << "||=======================================================================||" << endl;
    cout << "|| Question 3: Parallel Sorting with Threads                             ||" << endl;
    cout << "||    4. Generate Sample CSV (3000 records)                              ||" << endl;
    cout << "||    5. Load Students from CSV                                          ||" << endl;
    cout << "||    6. Perform Parallel Sort (with timing)                             ||" << endl;
    cout << "||=======================================================================||" << endl;
    cout << "|| Question 4: Different Iterator Types                                  ||" << endl;
    cout << "||    7. Display Records in Insertion Order                              ||" << endl;
    cout << "||    8. Display Records in Sorted Order                                 ||" << endl;
    cout << "||=======================================================================||" << endl;
    cout << "|| Question 5: Fast Query by Course Grade                                ||" << endl;
    cout << "||    9. Query Students (Grade >= 9 in specific course)                  ||" << endl;
    cout << "||    10. Custom Query (Choose course and grade threshold)               ||" << endl;
    cout << "||=======================================================================||" << endl;
    cout << "||    11. DELETE STUDENT (Permanent)                                     ||" << endl;
    cout << "||    0. Exit                                                            ||" << endl;
    cout << "||=======================================================================||" << endl;
}

void wait_for_enter() {
    cout << "Press Enter to return to menu..."; 
    cout.flush();
    string tmp; 
    getline(cin, tmp);
}

// Helpers moved from monolithic main
void manual_add_student(vector<Student>& students, bool iiit_mode) {
    if (students.empty()) {
        cout << "Note: Loading existing students.csv first...\n";
        ERPUtils::load_csv("students.csv", students);
    }
    cout << "\n--- Manual Student Creation ---\n";
    
    RollID r = InputValidator::readRollID(iiit_mode);
    string name = InputValidator::readString("Name: ");
    string branch = InputValidator::readString("Branch: ");
    int year = InputValidator::readYear("Start Year (YYYY): "); // Uses new 4-digit check

    Student s(r, name, branch, year);
    
    // Add courses loop logic...
    cout << "--- Adding CURRENT Courses (Type 'done' to finish) ---\n";
    while(true) {
        string prompt = iiit_mode ? "Current Code (String): " : "Current Code (Int): ";
        string input = InputValidator::readCourseStringOrDone(prompt);
        if (input == "done" || input == "") break;
        
        CourseID c;
        if (iiit_mode) c = input;
        else {
             try { c = stoi(input); } catch(...) { c = input; }
        }
        double g = InputValidator::readDouble("  Grade (0.0-10.0): ");
        s.add_course(c, g, true);
    }
    
    cout << "--- Adding PREVIOUS Courses (Type 'done' to finish) ---\n";
    while(true) {
        string input = InputValidator::readCourseStringOrDone("Previous Code: ");
        if (input == "done" || input == "") break;
        CourseID c;
        if (iiit_mode) c = input;
        else {
             try { c = stoi(input); } catch(...) { c = input; }
        }
        double g = InputValidator::readDouble("  Grade (0.0-10.0): ");
        s.add_course(c, g, false);
    }

    students.push_back(s);
    ERPUtils::append_student_to_csv(s, "students.csv");
    cout << "Student saved!\n";
    wait_for_enter();
}

void delete_student(vector<Student>& students) {
    if (students.empty()) ERPUtils::load_csv("students.csv", students);
    cout << "\n--- DELETE STUDENT ---\n";
    string roll_in = InputValidator::readString("Enter Roll Number: ");

    auto it = find_if(students.begin(), students.end(), [&](const Student& s) {
        return to_string_variant(s.get_roll()) == roll_in;
    });

    if (it != students.end()) {
        cout << "Found: " << it->brief() << "\n";
        string confirm = InputValidator::readString("Delete Permanently? (y/n): ");
        if (confirm == "y" || confirm == "Y") {
            students.erase(it);
            ERPUtils::save_all_students_to_csv(students, "students.csv");
            cout << "Deleted.\n";
        }
    } else {
        cout << "Not found.\n";
    }
    wait_for_enter();
}

int main() {
    string csv_file = "students.csv";
    vector<Student> students;
    vector<size_t> sorted_indices, input_order;
    CourseIndex cidx;
    bool sorted = false, indexed = false;

    while (true) {
        displayMenu();
        int choice = InputValidator::readMenuChoice(0, 11);
        if (choice == 0) break;

        switch (choice) {
            case 1: manual_add_student(students, true); break;
            case 2: manual_add_student(students, false); break;
            case 3: {
                if (students.empty()) ERPUtils::load_csv(csv_file, students);
                size_t limit = InputValidator::readDisplayLimit();
                if (limit == 0) limit = students.size();
                cout << "Total: " << students.size() << "\n";
                for(size_t i=0; i<students.size() && i<limit; ++i)
                    cout << (i+1) << ". " << students[i].brief() << "\n";
                wait_for_enter();
                break;
            }
            case 4: 
                system("./gen_students 3000"); 
                wait_for_enter(); 
                break;
            case 5: 
                students.clear(); 
                cout << "Loaded " << ERPUtils::load_csv(csv_file, students) << " records.\n"; 
                wait_for_enter(); 
                break;
            case 6: {
                if (students.empty()) { cout << "Load first.\n"; wait_for_enter(); break; }
                sorted_indices.resize(students.size());
                iota(sorted_indices.begin(), sorted_indices.end(), 0);
                ThreadTimer t1, t2;
                ERPUtils::parallel_sort_indices(sorted_indices, students, t1, t2);
                cout << "T1: " << t1.duration_ms << "ms, T2: " << t2.duration_ms << "ms\n";
                sorted = true;
                wait_for_enter();
                break;
            }
            case 7: {
                if (students.empty()) { cout << "No data.\n"; wait_for_enter(); break; }
                input_order.resize(students.size());
                iota(input_order.begin(), input_order.end(), 0);
                size_t limit = InputValidator::readDisplayLimit();
                if (limit==0) limit = input_order.size();
                for(size_t i=0; i<input_order.size() && i<limit; ++i)
                    cout << (i+1) << ". " << students[input_order[i]].brief() << "\n";
                wait_for_enter();
                break;
            }
            case 8: {
                if (!sorted) { cout << "Sort first.\n"; wait_for_enter(); break; }
                size_t limit = InputValidator::readDisplayLimit();
                if (limit==0) limit = sorted_indices.size();
                for(size_t i=0; i<sorted_indices.size() && i<limit; ++i)
                    cout << (i+1) << ". " << students[sorted_indices[i]].brief() << "\n";
                wait_for_enter();
                break;
            }
            case 9: // Fallthrough to 10 logic for simplicity or keep distinct
            case 10: {
                if (students.empty()) { cout << "Load data first.\n"; wait_for_enter(); break; }
                if (!indexed) { cidx.build_from(students); indexed = true; }
                
                if (choice == 10) {
                     cout << "--- Available Courses ---\n";
                     auto ac = cidx.get_all_courses();
                     for(auto& c : ac) cout << to_string_variant(c) << " ";
                     cout << "\n";
                }

                string c_in = (choice == 9) ? "CS101" : InputValidator::readString("Enter Course ID: ");
                // Auto-detect int vs string for CourseID
                CourseID cid;
                try { cid = stoi(c_in); } catch(...) { cid = c_in; } // Simple auto-detection
                
                double g = (choice == 9) ? 9.0 : InputValidator::readDouble("Min Grade: ");
                auto res = cidx.top_students_for_course(cid, g);
                
                size_t limit = InputValidator::readDisplayLimit();
                if(limit==0) limit = res.size();
                cout << "Found " << res.size() << " students.\n";
                for(size_t i=0; i<res.size() && i<limit; ++i)
                    cout << res[i]->brief() << " [" << *(res[i]->grade_for_course(cid)) << "]\n";
                wait_for_enter();
                break;
            }
            case 11: 
                delete_student(students); 
                sorted = false; indexed = false; 
                break;
        }
    }
    return 0;
}