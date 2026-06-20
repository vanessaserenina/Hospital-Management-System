#include "Patient.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <functional>

using namespace std;

int Patient::patientCount = 0;
int Patient::nextPatientNumber = 1;

Patient::Patient()
    : Person("Unknown Patient", "PAT0001"),
      insuranceProvider("None"),
      contactNumber("N/A"), dateOfBirth("N/A"), bloodType("Unknown") {
    patientCount++;
}

Patient::Patient(string name, string id, string insurance)
    : Person(name, id), insuranceProvider(insurance),
      contactNumber("N/A"), dateOfBirth("N/A"), bloodType("Unknown") {
    patientCount++;
}

Patient::Patient(string name, string id, string insurance, string contact, string dob, string blood)
    : Person(name, id), insuranceProvider(insurance),
      contactNumber(contact), dateOfBirth(dob), bloodType(blood) {
    patientCount++;
}

string Patient::getInsuranceProvider() const { return insuranceProvider; }
string Patient::getContactNumber() const { return contactNumber; }
string Patient::getDateOfBirth() const { return dateOfBirth; }
string Patient::getBloodType() const { return bloodType; }
vector<AppointmentRecord>& Patient::getAppointments() { return appointments; }
const vector<AppointmentRecord>& Patient::getAppointments() const { return appointments; }

void Patient::setInsuranceProvider(string provider) { insuranceProvider = provider; }
void Patient::setContactNumber(string contact) { contactNumber = contact; }
void Patient::setDateOfBirth(string dob) { dateOfBirth = dob; }
void Patient::setBloodType(string blood) { bloodType = blood; }

void Patient::scheduleAppointment(MedicalService s, string date, bool emergency, string doctorId) {
    AppointmentRecord record(s, date, emergency, InsuranceTier::NONE, doctorId);
    appointments.push_back(record);
}

bool Patient::setAppointmentStatus(string appointmentId, string status) {
    for (auto& appt : appointments) {
        if (appt.getAppointmentId() == appointmentId) {
            appt.setAppointmentStatus(status);
            return true;
        }
    }
    return false;
}

bool Patient::setAppointmentBillingTier(string appointmentId, InsuranceTier tier) {
    for (auto& appt : appointments) {
        if (appt.getAppointmentId() == appointmentId) {
            appt.setInsuranceTier(tier);
            return true;
        }
    }
    return false;
}

bool Patient::setAppointmentDoctor(string appointmentId, string doctorId) {
    for (auto& appt : appointments) {
        if (appt.getAppointmentId() == appointmentId) {
            appt.setDoctorId(doctorId);
            return true;
        }
    }
    return false;
}

double Patient::calculateTotalMedicalExpenses() const {
    double total = 0.0;
    for (const auto& appt : appointments) {
        if (appt.getAppointmentStatus() != "Cancelled") {
            total += appt.calculateBillingAmount();
        }
    }
    return total;
}

double Patient::calculateTotalPreCoverageExpenses() const {
    double total = 0.0;
    for (const auto& appt : appointments) {
        if (appt.getAppointmentStatus() != "Cancelled") {
            total += appt.calculatePreCoverageAmount();
        }
    }
    return total;
}

int Patient::getAppointmentCount() const {
    return static_cast<int>(appointments.size());
}

void Patient::displayRole() const {
    cout << string(60, '-') << endl;
    cout << " [PATIENT] " << name << "  (ID: " << id << ")" << endl;
    cout << string(60, '-') << endl;
    cout << "  Insurance Provider : " << insuranceProvider << endl;
    cout << "  Contact Number     : " << contactNumber << endl;
    cout << "  Date of Birth      : " << dateOfBirth << endl;
    cout << "  Blood Type         : " << bloodType << endl;
    cout << "  Total Appointments : " << appointments.size() << endl;
    cout << "  Total Expenses     : $" << fixed << setprecision(2) << calculateTotalMedicalExpenses() << endl;

    if (!appointments.empty()) {
        cout << endl;
        cout << "  --- Appointment Records ---" << endl;
        cout << "  " << left
             << setw(11) << "Appt ID"
             << setw(14) << "Service Code"
             << setw(26) << "Service Name"
             << setw(14) << "Date"
             << setw(12) << "Status"
             << setw(10) << "Emergency"
             << setw(8)  << "Tier"
             << setw(22) << "Doctor"
             << setw(12) << "Bill"
             << endl;
        cout << "  " << string(129, '-') << endl;
        for (const auto& appt : appointments) {
            appt.displayRecord();
        }
    }
}

void Patient::displayRole(const function<string(const string&)>& doctorLookup) const {
    cout << string(60, '-') << endl;
    cout << " [PATIENT] " << name << "  (ID: " << id << ")" << endl;
    cout << string(60, '-') << endl;
    cout << "  Insurance Provider : " << insuranceProvider << endl;
    cout << "  Contact Number     : " << contactNumber << endl;
    cout << "  Date of Birth      : " << dateOfBirth << endl;
    cout << "  Blood Type         : " << bloodType << endl;
    cout << "  Total Appointments : " << appointments.size() << endl;
    cout << "  Total Expenses     : $" << fixed << setprecision(2) << calculateTotalMedicalExpenses() << endl;

    if (!appointments.empty()) {
        cout << endl;
        cout << "  --- Appointment Records ---" << endl;
        cout << "  " << left
             << setw(11) << "Appt ID"
             << setw(14) << "Service Code"
             << setw(26) << "Service Name"
             << setw(14) << "Date"
             << setw(12) << "Status"
             << setw(10) << "Emergency"
             << setw(8)  << "Tier"
             << setw(22) << "Doctor"
             << setw(12) << "Bill"
             << endl;
        cout << "  " << string(129, '-') << endl;
        for (const auto& appt : appointments) {
            string docName = doctorLookup(appt.getDoctorId());
            appt.displayRecord(docName);
        }
    }
}

string Patient::getType() const { return "Patient"; }

int Patient::getPatientCount() { return patientCount; }

string Patient::generatePatientId() {
    ostringstream oss;
    oss << "PAT" << setw(4) << setfill('0') << nextPatientNumber++;
    return oss.str();
}

void Patient::resetCounter() {
    patientCount = 0;
    nextPatientNumber = 1;
}

void Patient::setNextPatientNumber(int next) {
    nextPatientNumber = next;
}