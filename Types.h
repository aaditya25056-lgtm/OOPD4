#ifndef TYPES_H
#define TYPES_H

#include <variant>
#include <string>
#include <iostream>
#include <cstdint> // <--- THIS WAS MISSING

using namespace std;

// Flexible types
using RollID = variant<uint64_t, string>;
using CourseID = variant<int, string>;
using Grade = double;

// Helpers
inline string to_string_variant(const RollID &r) {
    if (holds_alternative<uint64_t>(r)) return to_string(get<uint64_t>(r));
    return get<string>(r);
}
inline string to_string_variant(const CourseID &c) {
    if (holds_alternative<int>(c)) return to_string(get<int>(c));
    return get<string>(c);
}

#endif