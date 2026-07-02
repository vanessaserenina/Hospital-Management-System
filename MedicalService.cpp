#include "MedicalService.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

int MedicalService::serviceCount = 0;
int MedicalService::nextServiceNumber = 1;

MedicalService::MedicalService()
    : serviceCode("SVC000"), serviceName("General Consultation"), baseFee(50.0) {
    serviceCount++;
}

MedicalService::MedicalService(string code, string name)
    : serviceCode(code), serviceName(name), baseFee(50.0) {
    serviceCount++;
}

MedicalService::MedicalService(string code, string name, double fee)
    : serviceCode(code), serviceName(name), baseFee(fee) {}

string MedicalService::getServiceCode() const { return serviceCode; }
string MedicalService::getServiceName() const { return serviceName; }
double MedicalService::getBaseFee() const { return baseFee; }

void MedicalService::setServiceCode(string code) { serviceCode = code; }
void MedicalService::setServiceName(string name) { serviceName = name; }
void MedicalService::setBaseFee(double fee) {
    if (fee < 0.0) {
        throw invalid_argument("Base fee cannot be negative.");
    }
    baseFee = fee;
}

void MedicalService::displayServiceDetails() const {
    cout << left
         << setw(12) << serviceCode
         << setw(35) << serviceName
         << "$" << fixed << setprecision(2) << baseFee
         << endl;
}

int MedicalService::getServiceCount() { return serviceCount; }

string MedicalService::generateServiceCode() {
    ostringstream oss;
    oss << "SVC" << setw(3) << setfill('0') << nextServiceNumber++;
    return oss.str();
}

void MedicalService::setNextServiceNumber(int next) {
    nextServiceNumber = next;
}