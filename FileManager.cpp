#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <cerrno>
#include <stdexcept>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;

bool FileManager::fileExists(const string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

// Source:
// ggml-org (n.d.).
// llama.cpp - vulkan-shaders-gen.cpp
// https://github.com/ggml-org/llama.cpp/blob/master/ggml/src/ggml-vulkan/vulkan-shaders/vulkan-shaders-gen.cpp
bool FileManager::createDirectory(const string& path) {
#ifdef _WIN32
    int result = _mkdir(path.c_str());
#else
    int result = mkdir(path.c_str(), 0755);
#endif
    return (result == 0 || errno == EEXIST);
}

bool FileManager::writeToFile(const string& path, const string& content, bool append) {
    ios_base::openmode mode = append ? (ios::out | ios::app) : ios::out;
    ofstream file(path, mode);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    file.close();
    return true;
}

string FileManager::readFromFile(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + path);
    }
    ostringstream oss;
    oss << file.rdbuf();
    file.close();
    return oss.str();
}

vector<string> FileManager::readLines(const string& path) {
    vector<string> lines;
    ifstream file(path);
    if (!file.is_open()) {
        return lines;
    }
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    file.close();
    return lines;
}

bool FileManager::deleteFile(const string& path) {
    return (remove(path.c_str()) == 0);
}

// Source:
// The University of Western Australia (2002).
// CITS2002 System Programming - Lecture 16
// https://teaching.csse.uwa.edu.au/units/CITS2002/lectures/lecture16/singlepage.html
string FileManager::getTimestamp() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return string(buf);
}

void FileManager::logActivity(const string& logPath, const string& activity) {
    string entry = "[" + getTimestamp() + "] " + activity + "\n";
    writeToFile(logPath, entry, true);
}
