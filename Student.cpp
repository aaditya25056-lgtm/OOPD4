#include "Student.h"
#include <sstream>
#include <iomanip>

Student::Student(RollID r, string n, string br, int sy)
    : roll(move(r)), name(move(n)), branch(move(br)), startYear(sy) {}

void Student::add_course(const CourseID &c, Grade g, bool current) {
    if (current) courses.emplace_back(c, g);
    else prevCourses.emplace_back(c, g);
}

// CourseID Utils
size_t CourseIDHash::operator()(const CourseID &c) const noexcept {
    if (holds_alternative<int>(c))
        return hash<int>()(get<int>(c)) ^ 0x9e3779b97f4a7c15ULL;
    return hash<string>()(get<string>(c));
}
bool CourseIDEq::operator()(const CourseID &a, const CourseID &b) const noexcept {
    if (a.index() != b.index()) return false;
    if (holds_alternative<int>(a)) return get<int>(a) == get<int>(b);
    return get<string>(a) == get<string>(b);
}

optional<Grade> Student::grade_for_course(const CourseID &c) const {
    for (auto &p : courses) if (CourseIDEq{}(p.first, c)) return p.second;
    for (auto &p : prevCourses) if (CourseIDEq{}(p.first, c)) return p.second;
    return nullopt;
}

string Student::brief() const {
    stringstream ss;
    ss << "[" << to_string_variant(roll) << "] " << name
       << " (" << branch << ", " << startYear << ")";
    return ss.str();
}

string Student::full_display() const {
    stringstream ss;
    ss << brief() << "\n  Current courses:\n";
    for (auto &p : courses)
        ss << "    " << to_string_variant(p.first) << " : " << fixed << setprecision(1) << p.second << "\n";
    ss << "  Previous courses:\n";
    for (auto &p : prevCourses)
        ss << "    " << to_string_variant(p.first) << " : " << fixed << setprecision(1) << p.second << "\n";
    return ss.str();
}