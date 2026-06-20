#include <iostream>
#include <stdexcept>
#include "HospitalSystem.h"
#include "UIHelper.h"

using namespace std;

int main() {
    try {
        HospitalSystem system("data");
        system.run();
    } catch (const exception& e) {
        cerr << "[FATAL ERROR] " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "[FATAL ERROR] An unknown error occurred." << endl;
        return 2;
    }
    return 0;
}
