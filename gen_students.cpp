// gen_students.cpp
// Generates CSV file with student records for testing
// Usage: ./gen_students <count>   (default: 3000)
// Output: students.csv

#include <bits/stdc++.h>
#include <cstring>
using namespace std;

// Generate varied names
string rand_name(int id) {
    static vector<string> first = {
        "Aarav","Aditi","Advait","Aisha","Akash","Ananya","Ankit","Anushka",
        "Arjun","Divya","Gaurav","Isha","Karan","Kavya","Mihir","Neha",
        "Nikhil","Pooja","Priya","Rahul","Ravi","Rhea","Rohan","Sakshi",
        "Sameer","Shreya","Siddharth","Sneha","Tanvi","Vikram","Vivek","Yash"
    };
    static vector<string> last = {
        "Agarwal","Bhatt","Chopra","Desai","Gupta","Iyer","Joshi","Kapoor",
        "Kumar","Malhotra","Mehta","Nair","Patel","Rao","Reddy","Shah",
        "Sharma","Singh","Verma","Yadav"
    };
    
    int fn = (id * 7) % first.size();
    int ln = (id * 13) % last.size();
    
    if (id < 100) {
        return first[fn] + " " + last[ln];
    } else {
        return first[fn] + " " + last[ln] + " " + to_string((id % 100));
    }
}

// Random number generator
mt19937_64 global_rng((unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());

// Random boolean with probability p
bool coin(double p = 0.5) {
    uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(global_rng) < p;
}

int main(int argc, char** argv) {
    int N = 3000;
    if (argc >= 2) {
        try { 
            N = stoi(argv[1]); 
            if (N <= 0) N = 3000;
        }
        catch (...) { 
            N = 3000; 
        }
    }

    // Seed random
    srand((unsigned)time(nullptr));

    // University data
    vector<string> branches = {"CSE","ECE","CSAI","CSSS","CSD","CB","EVE","ME","HCD"};
    
    // IIIT-Delhi style course codes (strings)
    vector<string> courseStrings = {
        "CS101","CS102","CS201","CS301","MATH101","MATH102","MATH201",
        "PH101","PH102","EE101","EE102","AI201","AI301","ML201","ML301",
        "HSS101","HSS102","BIO101","ECO101","COM101"
    };
    
    // IIT-Delhi style course codes (integers)
    vector<int> courseInts = {101,102,201,202,301,302,401,501,502};

    // Random number generator setup
    uniform_int_distribution<int> branchDist(0, branches.size() - 1);
    uniform_real_distribution<double> gradeDist(6.0, 10.0);
    uniform_int_distribution<int> startYearDist(2018, 2023);
    uniform_int_distribution<int> courseIntDist(0, courseInts.size() - 1);
    uniform_int_distribution<int> courseStrDist(0, courseStrings.size() - 1);

    // Open output file
    ofstream fout("students.csv");
    if (!fout.is_open()) {
        cerr << "Error: Cannot write students.csv\n";
        return 1;
    }

    cout << "Generating " << N << " student records...\n";

    for (int i = 1; i <= N; i++) {
        // Generate roll number (70% numeric, 30% string format)
        string roll;
        if (coin(0.7)) {
            // Numeric roll number
            roll = to_string(2019000 + i);
        } else {
            // String roll number (BT-YY-XXX format)
            char buf[64];
            snprintf(buf, sizeof(buf), "BT-%02d-%04d", 
                    ((i % 5) + 20), i % 10000);
            roll = buf;
        }

        string name = rand_name(i);
        string branch = branches[branchDist(global_rng)];
        int startYear = startYearDist(global_rng);

        // Generate current courses (3-4 courses)
        vector<string> curr;
        int numCurrent = 3 + (i % 2);
        
        set<string> usedCourses; // Avoid duplicates
        
        for (int j = 0; j < numCurrent; j++) {
            double g = round(gradeDist(global_rng) * 10.0) / 10.0;
            
            string courseStr;
            // 50% chance of using string or int course ID
            if (coin()) {
                int cIdx = courseIntDist(global_rng);
                courseStr = to_string(courseInts[cIdx]) + ":" + to_string(g);
            } else {
                int cIdx = courseStrDist(global_rng);
                courseStr = courseStrings[cIdx] + ":" + to_string(g);
            }
            
            if (usedCourses.find(courseStr.substr(0, courseStr.find(':'))) == usedCourses.end()) {
                curr.push_back(courseStr);
                usedCourses.insert(courseStr.substr(0, courseStr.find(':')));
            }
        }

        // Generate previous courses (2-3 courses)
        vector<string> prev;
        int numPrev = 2 + (i % 2);
        
        for (int j = 0; j < numPrev; j++) {
            double g = round(gradeDist(global_rng) * 10.0) / 10.0;
            
            string courseStr;
            if (coin()) {
                int cIdx = courseIntDist(global_rng);
                courseStr = to_string(courseInts[cIdx]) + ":" + to_string(g);
            } else {
                int cIdx = courseStrDist(global_rng);
                courseStr = courseStrings[cIdx] + ":" + to_string(g);
            }
            
            if (usedCourses.find(courseStr.substr(0, courseStr.find(':'))) == usedCourses.end()) {
                prev.push_back(courseStr);
                usedCourses.insert(courseStr.substr(0, courseStr.find(':')));
            }
        }

        // Helper lambda to join vector with semicolons
        auto join = [&](vector<string>& v) {
            string s;
            for (size_t k = 0; k < v.size(); k++) {
                if (k > 0) s += ";";
                s += v[k];
            }
            return s;
        };

        // Write CSV line
        fout << roll << "," 
             << name << "," 
             << branch << "," 
             << startYear << "," 
             << join(curr) << "," 
             << join(prev) << "\n";

        // Progress indicator
        if (i % 500 == 0 || i == N) {
            cout << "Progress: " << i << "/" << N << " records\r" << flush;
        }
    }

    fout.close();

    cout << "\nSuccessfully generated students.csv with " << N << " records\n";
    cout << "File ready for use with ERP system.\n";
    
    return 0;
}