#ifndef STUDENT_H
#define STUDENT_H

#include "Types.h"
#include <vector>
#include <optional>

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
    Student(RollID r, string n, string br, int sy);

    void add_course(const CourseID &c, Grade g, bool current = true);

    const RollID& get_roll() const { return roll; }
    const string& get_name() const { return name; }
    const string& get_branch() const { return branch; }
    int get_startYear() const { return startYear; }
    const vector<pair<CourseID, Grade>>& get_courses() const { return courses; }
    const vector<pair<CourseID, Grade>>& get_prevCourses() const { return prevCourses; }

    optional<Grade> grade_for_course(const CourseID &c) const;
    string brief() const;
    string full_display() const;
};

// Hashing Helpers for Maps
struct CourseIDHash {
    size_t operator()(const CourseID &c) const noexcept;
};
struct CourseIDEq {
    bool operator()(const CourseID &a, const CourseID &b) const noexcept;
};

#endif