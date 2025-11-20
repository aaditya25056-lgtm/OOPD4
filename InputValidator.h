#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include "Types.h"

class InputValidator {
public:
    static string trim(const string& s);
    static string readString(const string& prompt);
    static int readInt(const string& prompt);
    static int readYear(const string& prompt); // <--- NEW: 4 Digit Check
    static double readDouble(const string& prompt);
    static int readMenuChoice(int min, int max);
    static RollID readRollID(bool iiit_mode);
    static string readCourseStringOrDone(const string& prompt);
    static size_t readDisplayLimit();
};

#endif