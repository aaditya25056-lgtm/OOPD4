#include "InputValidator.h"
#include <iostream>
#include <algorithm>

string InputValidator::trim(const string& s) {
    size_t a = 0, b = s.size();
    while (a < b && isspace((unsigned char)s[a])) ++a;
    while (b > a && isspace((unsigned char)s[b-1])) --b;
    return s.substr(a, b - a);
}

string InputValidator::readString(const string& prompt) {
    string line;
    while (true) {
        cout << prompt; cout.flush();
        if (!getline(cin, line)) exit(0);
        line = trim(line);
        if (!line.empty()) return line;
        cout << "Error: Input cannot be empty.\n";
    }
}

int InputValidator::readInt(const string& prompt) {
    string line;
    while (true) {
        cout << prompt; cout.flush();
        if (!getline(cin, line)) exit(0);
        line = trim(line);
        try {
            size_t pos;
            int val = stoi(line, &pos);
            if (pos == line.size()) return val;
        } catch (...) {}
        cout << "Error: Invalid integer.\n";
    }
}

// NEW: Enforce 4-digit year
int InputValidator::readYear(const string& prompt) {
    while(true) {
        int y = readInt(prompt);
        if (y >= 1000 && y <= 9999) return y;
        cout << "Error: Year must be a 4-digit integer (e.g., 2023).\n";
    }
}

double InputValidator::readDouble(const string& prompt) {
    string line;
    while (true) {
        cout << prompt; cout.flush();
        if (!getline(cin, line)) exit(0);
        line = trim(line);
        try {
            size_t pos;
            double val = stod(line, &pos);
            if (pos == line.size() && val >= 0.0 && val <= 10.0) return val;
        } catch (...) {}
        cout << "Error: Invalid grade (0.0 - 10.0).\n";
    }
}

int InputValidator::readMenuChoice(int min, int max) {
    string line;
    while (true) {
        cout << "Enter your choice: "; cout.flush();
        if (!getline(cin, line)) exit(0);
        line = trim(line);
        try {
            size_t pos;
            int val = stoi(line, &pos);
            if (pos == line.size() && val >= min && val <= max) return val;
        } catch (...) {}
        cout << "Invalid choice.\n";
    }
}

RollID InputValidator::readRollID(bool iiit_mode) {
    string line;
    while(true) {
        if (iiit_mode) cout << "Roll No (Integer, e.g., 2019001): ";
        else cout << "Roll No (String, e.g., MT19001): ";
        cout.flush();
        if (!getline(cin, line)) exit(0);
        line = trim(line);
        if (line.empty()) continue;

        if (iiit_mode) {
            bool isNum = !line.empty() && all_of(line.begin(), line.end(), ::isdigit);
            if (isNum) return (uint64_t)stoull(line);
            cout << "Error: Expected an integer roll number.\n";
        } else {
            return line;
        }
    }
}

string InputValidator::readCourseStringOrDone(const string& prompt) {
    string line;
    cout << prompt; cout.flush();
    if (!getline(cin, line)) exit(0);
    return trim(line);
}

size_t InputValidator::readDisplayLimit() {
    string line;
    cout << "How many records to display? (0 for All): "; cout.flush();
    if (!getline(cin, line)) exit(0);
    line = trim(line);
    if (line.empty()) return 0;
    try { return (size_t)stoul(line); } catch (...) { return 0; }
}