#pragma once
#include <string>
#include <vector>

using namespace std;

class FileManager {
public:
    static bool fileExists(const string& path);
    static bool createDirectory(const string& path);
    static bool writeToFile(const string& path, const string& content, bool append = false);
    static string readFromFile(const string& path);
    static vector<string> readLines(const string& path);
    static bool deleteFile(const string& path);
    static string getTimestamp();
    static void logActivity(const string& logPath, const string& activity);
};
