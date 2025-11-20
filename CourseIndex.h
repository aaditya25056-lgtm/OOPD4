#ifndef COURSEINDEX_H
#define COURSEINDEX_H

#include "Student.h"
#include <unordered_map>
#include <vector>

class CourseIndex {
private:
    unordered_map<CourseID, vector<Student*>, CourseIDHash, CourseIDEq> idx;
public:
    void build_from(vector<Student> &students);
    vector<Student*> top_students_for_course(const CourseID &c, Grade threshold);
    vector<CourseID> get_all_courses() const;
};

#endif