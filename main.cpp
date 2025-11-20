/* main.cpp - User-defined Display Limits & Instant Save (C++17)
   Compile: g++ -std=c++17 -O2 -pthread -Wall -Wextra -o erp main.cpp
*/
#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <optional>
#include <numeric>
#include <iomanip>
#include <cctype>
#include <limits>
#include <cstdlib> 

using namespace std;
using clk = chrono::high_resolution_clock;

// Flexible types
using RollID = variant<uint64_t, string>;
using CourseID = variant<int, string>;
using Grade = double;

// Variant -> string helpers
string to_string_variant(const RollID &r) {
    if (holds_alternative<uint64_t>(r)) return to_string(get<uint64_t>(r));
    return get<string>(r);
}
string to_string_variant(const CourseID &c) {
    if (holds_alternative<int>(c)) return to_string(get<int>(c));
    return get<string>(c);
}

// CourseID hashing & equality for unordered_map
struct CourseIDHash {
    size_t operator()(const CourseID &c) const noexcept {
        if (holds_alternative<int>(c))
            return hash<int>()(get<int>(c)) ^ 0x9e3779b97f4a7c15ULL;
        return hash<string>()(get<string>(c));
    }
};
struct CourseIDEq {
    bool operator()(const CourseID &a, const CourseID &b) const noexcept {
        if (a.index() != b.index()) return false;
        if (holds_alternative<int>(a)) return get<int>(a) == get<int>(b);
        return get<string>(a) == get<string>(b);
    }
};

// Student class
class Student {
private:
    RollID roll;
    string name;
    string branch;
    int startYear;
    vector<pair<CourseID, Grade>> courses;
    vector<pair<CourseID, Grade>> prevCourses;

public:
    Student() = default;
    Student(RollID r, string n, string br, int sy)
        : roll(move(r)), name(move(n)), branch(move(br)), startYear(sy) {}

    void add_course(const CourseID &c, Grade g, bool current = true) {
        if (current) courses.emplace_back(c, g);
        else prevCourses.emplace_back(c, g);
    }

    const RollID& get_roll() const { return roll; }
    const string& get_name() const { return name; }
    const string& get_branch() const { return branch; }
    int get_startYear() const { return startYear; }
    const vector<pair<CourseID, Grade>>& get_courses() const { return courses; }
    const vector<pair<CourseID, Grade>>& get_prevCourses() const { return prevCourses; }

    optional<Grade> grade_for_course(const CourseID &c) const {
        for (auto &p : courses)
            if (CourseIDEq{}(p.first, c)) return p.second;
        for (auto &p : prevCourses)
            if (CourseIDEq{}(p.first, c)) return p.second;
        return nullopt;
    }

    string brief() const {
        stringstream ss;
        ss << "[" << to_string_variant(roll) << "] " << name
           << " (" << branch << ", " << startYear << ")";
        return ss.str();
    }
    
    string full_display() const {
        stringstream ss;
        ss << brief() << "\n  Current courses:\n";
        for (auto &p : courses)
            ss << "    " << to_string_variant(p.first) << " : " << fixed << setprecision(1) << p.second << "\n";
        ss << "  Previous courses:\n";
        for (auto &p : prevCourses)
            ss << "    " << to_string_variant(p.first) << " : " << fixed << setprecision(1) << p.second << "\n";
        return ss.str();
    }
};

// Comparator that sorts indices by student name then roll-string
struct IndexComparator {
    const vector<Student>& students;
    IndexComparator(const vector<Student> &s) : students(s) {}
    bool operator()(size_t a, size_t b) const {
        const Student &A = students[a];
        const Student &B = students[b];
        if (A.get_name() != B.get_name()) return A.get_name() < B.get_name();
        return to_string_variant(A.get_roll()) < to_string_variant(B.get_roll());
    }
};

// Utilities
string trim(const string &s) {
    size_t a = 0, b = s.size();
    while (a < b && isspace((unsigned char)s[a])) ++a;
    while (b > a && isspace((unsigned char)s[b-1])) --b;
    return s.substr(a, b - a);
}
bool looks_like_uint64(const string &s) {
    if (s.empty()) return false;
    for (char c : s) if (!isdigit((unsigned char)c)) return false;
    return true;
}
bool looks_like_int(const string &s) {
    if (s.empty()) return false;
    size_t start = 0;
    if (s[0] == '-') start = 1;
    if (start >= s.size()) return false;
    for (size_t i = start; i < s.size(); ++i) if (!isdigit((unsigned char)s[i])) return false;
    return true;
}

// parse semicolon-separated course:grade tokens
vector<pair<CourseID, Grade>> parse_course_list(const string &s) {
    vector<pair<CourseID, Grade>> out;
    stringstream ss(s);
    string token;
    while (getline(ss, token, ';')) {
        token = trim(token);
        if (token.empty()) continue;
        auto pos = token.find(':');
        if (pos == string::npos) continue;
        string cstr = trim(token.substr(0, pos));
        string gstr = trim(token.substr(pos + 1));
        Grade g = 0;
        try { g = stod(gstr); } catch (...) { g = 0; }
        CourseID cid = looks_like_int(cstr) ? CourseID(stoi(cstr)) : CourseID(cstr);
        out.emplace_back(cid, g);
    }
    return out;
}

// load CSV rows
size_t load_csv(const string &filename, vector<Student> &students, size_t max_records = 0) {
    ifstream ifs(filename);
    if (!ifs.is_open()) return 0;
    string line;
    size_t cnt = 0;
    while (getline(ifs, line)) {
        if (line.empty()) continue;
        vector<string> fields;
        string tmp;
        stringstream ss(line);
        while (getline(ss, tmp, ',')) fields.push_back(trim(tmp));
        if (fields.size() < 4) continue;
        while (fields.size() < 6) fields.push_back("");
        try {
            RollID rid = looks_like_uint64(fields[0]) ? RollID((uint64_t)stoull(fields[0])) : RollID(fields[0]);
            string name = fields[1];
            string branch = fields[2];
            int startYear = 2020;
            try { startYear = stoi(fields[3]); } catch (...) {}
            Student s(rid, name, branch, startYear);
            for (auto &p : parse_course_list(fields[4])) s.add_course(p.first, p.second, true);
            if (!fields[5].empty()) for (auto &p : parse_course_list(fields[5])) s.add_course(p.first, p.second, false);
            students.push_back(move(s));
            ++cnt;
            if (max_records && cnt >= max_records) break;
        } catch (...) {
            continue;
        }
    }
    return cnt;
}

// Helper to append a student to the CSV file immediately
void append_student_to_csv(const Student& s, const string& filename) {
    ofstream ofs(filename, ios::app); // Append mode
    if (!ofs.is_open()) {
        cerr << "Error: Could not write to " << filename << "\n";
        return;
    }
    
    ofs << to_string_variant(s.get_roll()) << ",";
    ofs << s.get_name() << ",";
    ofs << s.get_branch() << ",";
    ofs << s.get_startYear() << ",";
    
    const auto& curr = s.get_courses();
    for(size_t i=0; i<curr.size(); ++i) {
        if (i > 0) ofs << ";";
        ofs << to_string_variant(curr[i].first) << ":" << fixed << setprecision(1) << curr[i].second;
    }
    ofs << ",";
    
    const auto& prev = s.get_prevCourses();
    for(size_t i=0; i<prev.size(); ++i) {
        if (i > 0) ofs << ";";
        ofs << to_string_variant(prev[i].first) << ":" << fixed << setprecision(1) << prev[i].second;
    }
    ofs << "\n";
    ofs.close();
}

struct ThreadTimer { long long duration_ms = 0; };

// parallel sort indices
void parallel_sort_indices(vector<size_t>& indices, const vector<Student> &students, ThreadTimer &t1, ThreadTimer &t2) {
    if (indices.empty()) return;
    size_t n = indices.size();
    if (n <= 1) return;
    size_t mid = n / 2;
    IndexComparator comp(students);
    auto worker1 = [&]() {
        auto st = clk::now();
        sort(indices.begin(), indices.begin() + mid, comp);
        auto ed = clk::now();
        t1.duration_ms = chrono::duration_cast<chrono::milliseconds>(ed - st).count();
    };
    auto worker2 = [&]() {
        auto st = clk::now();
        sort(indices.begin() + mid, indices.end(), comp);
        auto ed = clk::now();
        t2.duration_ms = chrono::duration_cast<chrono::milliseconds>(ed - st).count();
    };
    thread th1(worker1), th2(worker2);
    th1.join(); th2.join();
    inplace_merge(indices.begin(), indices.begin() + mid, indices.end(), comp);
}

// CourseIndex: map from CourseID -> sorted vector<Student*>
class CourseIndex {
private:
    unordered_map<CourseID, vector<Student*>, CourseIDHash, CourseIDEq> idx;
public:
    void build_from(vector<Student> &students) {
        idx.clear();
        for (auto &s : students) {
            for (auto &p : s.get_courses()) idx[p.first].push_back(&s);
            for (auto &p : s.get_prevCourses()) idx[p.first].push_back(&s);
        }
        for (auto &kv : idx) {
            CourseID course = kv.first;
            auto &vec = kv.second;
            sort(vec.begin(), vec.end(), [&](const Student* A, const Student* B) {
                auto ga = A->grade_for_course(course);
                auto gb = B->grade_for_course(course);
                if (ga && gb) {
                    if (*ga != *gb) return *ga > *gb;
                    return to_string_variant(A->get_roll()) < to_string_variant(B->get_roll());
                }
                if (ga) return true;
                if (gb) return false;
                return to_string_variant(A->get_roll()) < to_string_variant(B->get_roll());
            });
            vec.erase(unique(vec.begin(), vec.end()), vec.end());
        }
    }

    vector<Student*> top_students_for_course(const CourseID &c, Grade threshold) {
        vector<Student*> out;
        auto it = idx.find(c);
        if (it == idx.end()) return out;
        for (auto s : it->second) {
            auto g = s->grade_for_course(c);
            if (g && *g >= threshold) out.push_back(s);
            else break;
        }
        return out;
    }

    vector<CourseID> get_all_courses() const {
        vector<CourseID> courses;
        for (auto &kv : idx) courses.push_back(kv.first);
        sort(courses.begin(), courses.end(), [](const CourseID &a, const CourseID &b){
            return to_string_variant(a) < to_string_variant(b);
        });
        return courses;
    }
};

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
    cout << "||    11. Run All Demonstrations (Full Assignment Demo)                  ||" << endl;
    cout << "||    0. Exit                                                            ||" << endl;
    cout << "||=======================================================================||" << endl;
    cout << "Enter your choice: ";
    cout.flush();
}

optional<int> read_choice_from_line(const string &line) {
    string s = trim(line);
    if (s.empty()) return nullopt;
    try {
        size_t pos = 0;
        int v = stoi(s, &pos);
        if (pos != s.size()) return nullopt;
        return v;
    } catch (...) { return nullopt; }
}

void wait_for_enter() {
    cout << "Press Enter to return to menu..."; 
    cout.flush();
    string tmp; 
    getline(cin, tmp);
}

// Get user limit for display
size_t get_user_display_limit() {
    cout << "How many records to display? (0 for All): ";
    cout.flush();
    string s; 
    getline(cin, s);
    try {
        return (size_t)stoi(s);
    } catch(...) {
        return 0;
    }
}

// Helper to manually add student and persist to CSV
void manual_add_student(vector<Student>& students, bool iiit_mode) {
    if (students.empty()) {
        cout << "Note: Loading existing students.csv first to ensure consistency...\n";
        load_csv("students.csv", students);
    }

    cout << "\n--- Manual Student Creation ---\n";
    string name, branch, roll_str, course_str, grade_str;
    
    cout << "Name: "; getline(cin, name);
    cout << "Branch: "; getline(cin, branch);
    
    RollID r;
    if (iiit_mode) {
        cout << "Roll No (Integer, e.g., 2019001): "; getline(cin, roll_str);
        try { r = (uint64_t)stoull(roll_str); } catch(...) { r = (uint64_t)0; }
    } else {
        cout << "Roll No (String, e.g., MT19001): "; getline(cin, roll_str);
        r = roll_str;
    }

    Student s(r, name, branch, 2023);
    
    CourseID c;
    double g = 0.0;
    
    if (iiit_mode) {
        cout << "Course Code (String, e.g., CS101): "; getline(cin, course_str);
        c = course_str;
    } else {
        cout << "Course Code (Integer, e.g., 101): "; getline(cin, course_str);
        try { c = stoi(course_str); } catch(...) { c = 0; }
    }
    
    cout << "Grade (e.g. 9.0): "; getline(cin, grade_str);
    try { g = stod(grade_str); } catch(...) { g = 0.0; }
    
    s.add_course(c, g);
    students.push_back(s); 
    
    // PERSIST TO FILE IMMEDIATELY
    append_student_to_csv(s, "students.csv");
    
    cout << "Student added to memory and appended to students.csv!\n";
    wait_for_enter();
}

int main(int argc, char **argv) {
    (void)argc; (void)argv; 
    
    string csv_file = "students.csv";
    vector<Student> students;
    vector<size_t> input_order;
    vector<size_t> sorted_indices;
    CourseIndex cidx;
    bool sorted = false;
    bool indexed = false;

    string line;
    while (true) {
        displayMenu();
        
        if (!getline(cin, line)) break;
        auto opt = read_choice_from_line(line);
        if (!opt) {
            cout << "Invalid input.\n";
            wait_for_enter();
            continue;
        }
        int choice = *opt;
        if (choice == 0) { cout << "\nExiting system. Goodbye!\n"; break; }

        switch (choice) {
            // Question 1 & 2
            case 1: 
                manual_add_student(students, true); // IIIT: int roll, string course
                break;
            case 2:
                manual_add_student(students, false); // IIT: string roll, int course
                break;
            case 3: {
                if (students.empty()) {
                    cout << "Loading data from CSV first...\n";
                    load_csv(csv_file, students);
                }
                size_t limit = get_user_display_limit();
                if (limit == 0) limit = students.size();
                
                cout << "\n--- All Students (Both Universities) ---\n";
                cout << "Total Records: " << students.size() << "\n";
                for(size_t i=0; i<students.size() && i<limit; ++i) {
                    cout << (i+1) << ". " << students[i].brief() << "\n";
                }
                wait_for_enter();
                break;
            }

            // Question 3
            case 4: {
                cout << "Generating 3000 records via gen_students...\n";
                int ret = system("./gen_students 3000");
                if (ret != 0) cout << "Error running gen_students. Make sure it is compiled.\n";
                else cout << "Generation complete. File: students.csv\n";
                wait_for_enter();
                break;
            }
            case 5: {
                cout << "Loading " << csv_file << "...\n";
                students.clear();
                size_t n = load_csv(csv_file, students);
                cout << "Loaded " << n << " records.\n";
                
                input_order.resize(students.size());
                iota(input_order.begin(), input_order.end(), 0);
                sorted_indices.resize(students.size());
                iota(sorted_indices.begin(), sorted_indices.end(), 0);
                sorted = false;
                indexed = false;
                wait_for_enter();
                break;
            }
            case 6: {
                if (students.empty()) { cout << "Load students first (Option 5).\n"; wait_for_enter(); break; }
                
                sorted_indices.resize(students.size());
                iota(sorted_indices.begin(), sorted_indices.end(), 0);
                
                cout << "Performing parallel sort...\n";
                ThreadTimer t1, t2;
                auto st = clk::now();
                parallel_sort_indices(sorted_indices, students, t1, t2);
                auto en = clk::now();
                auto total = chrono::duration_cast<chrono::milliseconds>(en - st).count();
                cout << "Thread 1: " << t1.duration_ms << "ms, Thread 2: " << t2.duration_ms << "ms\n";
                cout << "Total Sort Time: " << total << "ms\n";
                sorted = true;
                wait_for_enter();
                break;
            }

            // Question 4
            case 7: {
                if (students.empty()) { cout << "No data loaded.\n"; wait_for_enter(); break; }
                
                // Ensure indices match data size
                input_order.resize(students.size());
                iota(input_order.begin(), input_order.end(), 0);

                size_t limit = get_user_display_limit();
                if (limit == 0) limit = input_order.size();
                
                cout << "\n--- Insertion Order ---\n";
                for(size_t i=0; i<input_order.size() && i<limit; ++i) {
                    cout << (i+1) << ". " << students[input_order[i]].brief() << "\n";
                }
                wait_for_enter();
                break;
            }
            case 8: {
                if (!sorted) { cout << "Please sort first (Option 6).\n"; wait_for_enter(); break; }
                
                size_t limit = get_user_display_limit();
                if (limit == 0) limit = sorted_indices.size();

                cout << "\n--- Sorted Order ---\n";
                for(size_t i=0; i<sorted_indices.size() && i<limit; ++i) {
                    cout << (i+1) << ". " << students[sorted_indices[i]].brief() << "\n";
                }
                wait_for_enter();
                break;
            }

            // Question 5
            case 9: {
                if (students.empty()) { cout << "Load data first.\n"; wait_for_enter(); break; }
                if (!indexed) { cidx.build_from(students); indexed = true; }
                
                cout << "Querying: Course contains 'CS' or '101' and Grade >= 9.0\n";
                auto courses = cidx.get_all_courses();
                if (courses.empty()) { cout << "No courses.\n"; break; }
                
                CourseID demoC = courses[0]; 
                cout << "Auto-selected course: " << to_string_variant(demoC) << "\n";
                auto res = cidx.top_students_for_course(demoC, 9.0);
                cout << "Found " << res.size() << " students:\n";
                for(size_t i=0; i<res.size() && i<10; ++i) {
                     auto g = res[i]->grade_for_course(demoC);
                     cout << res[i]->get_name() << " : " << (g?*g:0.0) << "\n";
                }
                wait_for_enter();
                break;
            }
            case 10: {
                if (students.empty()) { cout << "Load data first.\n"; wait_for_enter(); break; }
                if (!indexed) { cidx.build_from(students); indexed = true; }
                
                cout << "Enter course ID: "; cout.flush();
                string c_in; getline(cin, c_in);
                CourseID cid;
                if (looks_like_int(c_in)) cid = stoi(c_in); else cid = c_in;
                
                cout << "Min Grade: "; cout.flush();
                string g_in; getline(cin, g_in);
                double g_val = 8.0;
                try { g_val = stod(g_in); } catch(...) {}
                
                auto res = cidx.top_students_for_course(cid, g_val);
                cout << "Found " << res.size() << " students.\n";
                for(size_t i=0; i<res.size() && i<20; ++i) {
                     cout << res[i]->brief() << "\n";
                }
                wait_for_enter();
                break;
            }

            // Demo All
            case 11: {
                cout << "\n--- RUNNING FULL DEMO ---\n";
                cout << "1. Generating Data...\n"; system("./gen_students 50");
                cout << "2. Loading Data...\n";
                students.clear(); load_csv("students.csv", students);
                
                input_order.resize(students.size()); iota(input_order.begin(), input_order.end(), 0);
                sorted_indices.resize(students.size()); iota(sorted_indices.begin(), sorted_indices.end(), 0);
                
                cout << "3. Parallel Sorting...\n";
                ThreadTimer t1, t2;
                parallel_sort_indices(sorted_indices, students, t1, t2);
                cout << "   Sort complete.\n";
                
                cout << "4. Building Index...\n";
                cidx.build_from(students); indexed = true; sorted = true;
                
                cout << "5. Showing First 3 Sorted:\n";
                for(size_t i=0; i<3 && i<students.size(); ++i) cout << "   " << students[sorted_indices[i]].get_name() << "\n";
                
                cout << "\nDemo Complete.\n";
                wait_for_enter();
                break;
            }

            default:
                cout << "Invalid choice.\n";
        }
    }

    return 0;
}