#include "ERPUtils.h"
#include "InputValidator.h" // for trim
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <algorithm>
#include <chrono>

using namespace std;
using clk = chrono::high_resolution_clock;

// Internal Helpers
static bool looks_like_uint64(const string &s) {
    if (s.empty()) return false;
    for (char c : s) if (!isdigit((unsigned char)c)) return false;
    return true;
}

static bool looks_like_int(const string &s) {
    if (s.empty()) return false;
    size_t start = 0;
    if (s[0] == '-') start = 1;
    if (start >= s.size()) return false;
    for (size_t i = start; i < s.size(); ++i) if (!isdigit((unsigned char)s[i])) return false;
    return true;
}

static vector<pair<CourseID, Grade>> parse_course_list(const string &s) {
    vector<pair<CourseID, Grade>> out;
    stringstream ss(s);
    string token;
    while (getline(ss, token, ';')) {
        token = InputValidator::trim(token);
        if (token.empty()) continue;
        auto pos = token.find(':');
        if (pos == string::npos) continue;
        string cstr = InputValidator::trim(token.substr(0, pos));
        string gstr = InputValidator::trim(token.substr(pos + 1));
        Grade g = 0;
        try { g = stod(gstr); } catch (...) { g = 0; }
        CourseID cid = looks_like_int(cstr) ? CourseID(stoi(cstr)) : CourseID(cstr);
        out.emplace_back(cid, g);
    }
    return out;
}

size_t ERPUtils::load_csv(const string &filename, vector<Student> &students, size_t max_records) {
    ifstream ifs(filename);
    if (!ifs.is_open()) return 0;
    string line;
    size_t cnt = 0;
    while (getline(ifs, line)) {
        if (line.empty()) continue;
        vector<string> fields;
        string tmp;
        stringstream ss(line);
        while (getline(ss, tmp, ',')) fields.push_back(InputValidator::trim(tmp));
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
        } catch (...) { continue; }
    }
    return cnt;
}

static void write_student_to_stream(ostream& ofs, const Student& s) {
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
}

void ERPUtils::append_student_to_csv(const Student& s, const string& filename) {
    ofstream ofs(filename, ios::app);
    if (ofs.is_open()) { write_student_to_stream(ofs, s); ofs.close(); }
}

void ERPUtils::save_all_students_to_csv(const vector<Student>& students, const string& filename) {
    ofstream ofs(filename, ios::trunc);
    if (ofs.is_open()) {
        for(const auto& s : students) write_student_to_stream(ofs, s);
        ofs.close();
    }
}

// Sorting Comparator
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

void ERPUtils::parallel_sort_indices(vector<size_t>& indices, const vector<Student> &students, ThreadTimer &t1, ThreadTimer &t2) {
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