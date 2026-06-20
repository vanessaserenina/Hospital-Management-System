#pragma once
#include <string>

using namespace std;

class UIHelper {
public:
    static void printDivider(char ch = '=', int width = 60);
    static void printCenteredTitle(string title, int width = 60);
    static void printSectionHeader(string title);
    static void printSuccess(string message);
    static void printError(string message);
    static void printWarning(string message);
    static void printInfo(string message);
    static void clearScreen();
    static void pressEnterToContinue();
    static string getInput(string prompt);
    static int getIntInput(string prompt, int minVal, int maxVal);
    static double getDoubleInput(string prompt, double minVal, double maxVal);
    static bool getYesNo(string prompt);
    static string formatCurrency(double amount);
    static string padRight(string s, int width);
    static string padLeft(string s, int width);
    static void printTableRow(string col1, string col2, string col3, int w1 = 20, int w2 = 20, int w3 = 18);
    static void printTableHeader(string h1, string h2, string h3, int w1 = 20, int w2 = 20, int w3 = 18);

    static bool validateDate(const string& date);
    static string getDateInput(const string& prompt, bool allowTBD = false);
};