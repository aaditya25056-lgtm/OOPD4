#include "CourseIndex.h"
#include <algorithm>

void CourseIndex::build_from(vector<Student> &students) {
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
            return false; 
        });
        vec.erase(unique(vec.begin(), vec.end()), vec.end());
    }
}

vector<Student*> CourseIndex::top_students_for_course(const CourseID &c, Grade threshold) {
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

vector<CourseID> CourseIndex::get_all_courses() const {
    vector<CourseID> courses;
    for (auto &kv : idx) courses.push_back(kv.first);
    sort(courses.begin(), courses.end(), [](const CourseID &a, const CourseID &b){
        return to_string_variant(a) < to_string_variant(b);
    });
    return courses;
}