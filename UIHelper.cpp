#include "UIHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <stdexcept>

using namespace std;

void UIHelper::printDivider(char ch, int width) {
    cout << string(width, ch) << endl;
}

void UIHelper::printCenteredTitle(string title, int width) {
    int padding = (width - static_cast<int>(title.size())) / 2;
    if (padding < 0) padding = 0;
    cout << string(padding, ' ') << title << endl;
}

void UIHelper::printSectionHeader(string title) {
    cout << endl;
    printDivider('=', 60);
    printCenteredTitle(title, 60);
    printDivider('=', 60);
}

void UIHelper::printSuccess(string message) {
    cout << "[SUCCESS] " << message << endl;
}

void UIHelper::printError(string message) {
    cout << "[ERROR]   " << message << endl;
}

void UIHelper::printWarning(string message) {
    cout << "[WARNING] " << message << endl;
}

void UIHelper::printInfo(string message) {
    cout << "[INFO]    " << message << endl;
}

// Source:
// Samual Sam (2026)
// TutorialsPoint, "How to clear console in C?"
// https://www.tutorialspoint.com/article/how-to-clear-console-in-c
void UIHelper::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void UIHelper::pressEnterToContinue() {
    cout << endl;
    cout << "Press [ENTER] to continue...";
    string dummy;
    getline(cin, dummy);
}

string UIHelper::getInput(string prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

int UIHelper::getIntInput(string prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) {
            if (cin.eof()) {
                return minVal;
            }
            cin.clear();
            continue;
        }
        if (line.empty()) {
            printError("Input cannot be empty. Please enter a number between "
                       + to_string(minVal) + " and " + to_string(maxVal) + ".");
            continue;
        }
        // Source:
        // DANIWEB (2013).
        // daniweb.com, "Updates version Age Calculator"
        // https://www.daniweb.com/programming/software-development/threads/434926/updates-version-age-calculator
        try {
            size_t pos;
            value = stoi(line, &pos);
            if (pos != line.size()) throw invalid_argument("trailing chars");
            if (value < minVal || value > maxVal) {
                printError("Value must be between " + to_string(minVal) + " and " + to_string(maxVal) + ".");
                continue;
            }
            break;
        } catch (...) {
            printError("Invalid input. Please enter an integer.");
        }
    }
    return value;
}

double UIHelper::getDoubleInput(string prompt, double minVal, double maxVal) {
    double value;
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        if (line.empty()) {
            printError("Input cannot be empty.");
            continue;
        }
        // Source:
        // DANIWEB (2013).
        // daniweb.com, "Updates version Age Calculator"
        // https://www.daniweb.com/programming/software-development/threads/434926/updates-version-age-calculator
        try {
            size_t pos;
            value = stod(line, &pos);
            if (pos != line.size()) throw invalid_argument("trailing chars");
            if (value < minVal || value > maxVal) {
                printError("Value must be between " + to_string(minVal) + " and " + to_string(maxVal) + ".");
                continue;
            }
            break;
        } catch (...) {
            printError("Invalid input. Please enter a numeric value.");
        }
    }
    return value;
}

bool UIHelper::getYesNo(string prompt) {
    while (true) {
        cout << prompt << " (y/n): ";
        string line;
        getline(cin, line);
        if (!line.empty()) {
            char ch = tolower(line[0]);
            if (ch == 'y') return true;
            if (ch == 'n') return false;
            printError("Please enter 'y' for yes or 'n' for no.");
        } else {
            printError("Please enter 'y' for yes or 'n' for no.");
        }
    }
}

string UIHelper::formatCurrency(double amount) {
    ostringstream oss;
    oss << "$" << fixed << setprecision(2) << amount;
    return oss.str();
}

// Source:
// Stephens, D. R., Diggins, C., Turkanis, J., & Cogswell, J. (November, 2005).
// C++ Cookbook. O'Reilly Media "4.1. Padding a String"
// https://www.oreilly.com/library/view/c-cookbook/0596007612/ch04s02.html
string UIHelper::padRight(string s, int width) {
    if (static_cast<int>(s.size()) >= width) return s.substr(0, width);
    return s + string(width - s.size(), ' ');
}

string UIHelper::padLeft(string s, int width) {
    if (static_cast<int>(s.size()) >= width) return s.substr(0, width);
    return string(width - s.size(), ' ') + s;
}

void UIHelper::printTableRow(string col1, string col2, string col3, int w1, int w2, int w3) {
    cout << "| " << padRight(col1, w1)
         << "| " << padRight(col2, w2)
         << "| " << padRight(col3, w3)
         << "|" << endl;
}

void UIHelper::printTableHeader(string h1, string h2, string h3, int w1, int w2, int w3) {
    int total = w1 + w2 + w3 + 7;
    printDivider('-', total);
    printTableRow(h1, h2, h3, w1, w2, w3);
    printDivider('-', total);
}

bool UIHelper::validateDate(const string& date) {
    if (date.size() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;

    for (int i : {0,1,2,3,5,6,8,9}) {
        if (!isdigit(date[i])) return false;
    }

    int year  = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day   = stoi(date.substr(8, 2));

    if (year < 1900 || year > 2100) return false;
    if (month < 1   || month > 12)  return false;
    if (day   < 1)                  return false;

    // Source:
    // Barb Cutler (2006).
    // CSCI-1200 Computer Science II - Lecture 1: Introduction and Background (Fall 2006)
    // https://www.cs.rpi.edu/academics/courses/fall06/cs2/lectures/01_intro.pdf
    int daysInMonth[] = {0, 31,28,31,30,31,30,31,31,30,31,30,31};
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (month == 2 && leap) daysInMonth[2] = 29;

    if (day > daysInMonth[month]) return false;
    return true;
}

string UIHelper::getDateInput(const string& prompt, bool allowTBD) {
    while (true) {
        string input = getInput(prompt);

        if (allowTBD && input.empty()) return "TBD";

        if (!allowTBD && input.empty()) {
            printError("Date cannot be empty. Please enter a date in YYYY-MM-DD format.");
            continue;
        }

        if (validateDate(input)) return input;

        printError("Invalid date '" + input + "'. Please use YYYY-MM-DD format "
                   "(e.g. 2025-06-21) with a valid calendar date.");
    }
}