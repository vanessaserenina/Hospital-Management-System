#include "Doctor.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <stdexcept>

using namespace std;

int Doctor::doctorCount = 0;
int Doctor::nextDoctorNumber = 1;

Doctor::Doctor()
    : Person("Unknown Doctor", "DOC0001"),
      specializationDepartment("General Medicine"),
      assignedClinicCode("UNASSIGNED"),
      contactNumber("N/A"),
      licenseNumber("N/A") {
    doctorCount++;
}

Doctor::Doctor(string name, string id, string specialization, string clinicCode)
    : Person(name, id), specializationDepartment(specialization),
      assignedClinicCode(clinicCode), contactNumber("N/A"), licenseNumber("N/A") {
    doctorCount++;
}

Doctor::Doctor(string name, string id, string specialization, string clinicCode, string contact, string license)
    : Person(name, id), specializationDepartment(specialization),
      assignedClinicCode(clinicCode), contactNumber(contact), licenseNumber(license) {
    doctorCount++;
}

string Doctor::getSpecializationDepartment() const { return specializationDepartment; }
string Doctor::getAssignedClinicCode() const { return assignedClinicCode; }
string Doctor::getContactNumber() const { return contactNumber; }
string Doctor::getLicenseNumber() const { return licenseNumber; }
vector<string> Doctor::getAssignedPatientIds() const { return assignedPatientIds; }

void Doctor::setSpecializationDepartment(string dept) { specializationDepartment = dept; }
void Doctor::setContactNumber(string contact) { contactNumber = contact; }
void Doctor::setLicenseNumber(string license) { licenseNumber = license; }

void Doctor::assignToClinic(string code) {
    if (code.empty()) throw invalid_argument("Clinic code cannot be empty.");
    assignedClinicCode = code;
}

void Doctor::assignPatient(string patientId) {
    for (const auto& pid : assignedPatientIds) {
        if (pid == patientId) {
            throw runtime_error("Patient " + patientId + " is already assigned to this doctor.");
        }
    }
    assignedPatientIds.push_back(patientId);
}

bool Doctor::removePatientAssignment(string patientId) {
    auto it = find(assignedPatientIds.begin(), assignedPatientIds.end(), patientId);
    if (it != assignedPatientIds.end()) {
        assignedPatientIds.erase(it);
        return true;
    }
    return false;
}

int Doctor::getAssignedPatientCount() const {
    return static_cast<int>(assignedPatientIds.size());
}

void Doctor::displayRole() const {
    cout << string(60, '-') << endl;
    cout << " [DOCTOR]  " << name << "  (ID: " << id << ")" << endl;
    cout << string(60, '-') << endl;
    cout << "  Specialization     : " << specializationDepartment << endl;
    cout << "  Assigned Clinic    : " << assignedClinicCode << endl;
    cout << "  Contact Number     : " << contactNumber << endl;
    cout << "  License Number     : " << licenseNumber << endl;
    cout << "  Assigned Patients  : " << assignedPatientIds.size() << endl;

    if (!assignedPatientIds.empty()) {
        cout << "  Patient IDs        : ";
        for (size_t i = 0; i < assignedPatientIds.size(); ++i) {
            if (i > 0) cout << ", ";
            cout << assignedPatientIds[i];
        }
        cout << endl;
    }
}

string Doctor::getType() const { return "Doctor"; }

int Doctor::getDoctorCount() { return doctorCount; }

std::string Doctor::generateDoctorId() {
    ostringstream oss;
    oss << "DOC" << setw(4) << setfill('0') << nextDoctorNumber++;
    return oss.str();
}

void Doctor::resetCounter() {
    doctorCount = 0;
    nextDoctorNumber = 1;
}

void Doctor::setNextDoctorNumber(int next) {
    nextDoctorNumber = next;
}
