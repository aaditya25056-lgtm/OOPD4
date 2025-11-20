#ifndef ERPUTILS_H
#define ERPUTILS_H

#include "Student.h"
#include <vector>
#include <string>

struct ThreadTimer { long long duration_ms = 0; };

namespace ERPUtils {
    size_t load_csv(const string &filename, vector<Student> &students, size_t max_records = 0);
    void append_student_to_csv(const Student& s, const string& filename);
    void save_all_students_to_csv(const vector<Student>& students, const string& filename);
    void parallel_sort_indices(vector<size_t>& indices, const vector<Student> &students, ThreadTimer &t1, ThreadTimer &t2);
}

#endif