#include "AppointmentRecord.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;

int AppointmentRecord::recordCounter = 0;

AppointmentRecord::AppointmentRecord()
    : service(MedicalService()), appointmentStatus("Scheduled"),
      appointmentDate("N/A"), isEmergency(false), insuranceTier(InsuranceTier::NONE),
      doctorId("") {
    recordCounter++;
    ostringstream oss;
    oss << "APT" << setw(5) << setfill('0') << recordCounter;
    appointmentId = oss.str();
}

AppointmentRecord::AppointmentRecord(MedicalService s, string date, bool emergency, InsuranceTier tier, string doctorId)
    : service(s), appointmentStatus("Scheduled"), appointmentDate(date),
      isEmergency(emergency), insuranceTier(tier), doctorId(doctorId) {
    recordCounter++;
    ostringstream oss;
    oss << "APT" << setw(5) << setfill('0') << recordCounter;
    appointmentId = oss.str();
}

string AppointmentRecord::getServiceCode() const { return service.getServiceCode(); }
string AppointmentRecord::getServiceName() const { return service.getServiceName(); }
string AppointmentRecord::getAppointmentStatus() const { return appointmentStatus; }
string AppointmentRecord::getAppointmentDate() const { return appointmentDate; }
string AppointmentRecord::getAppointmentId() const { return appointmentId; }
bool AppointmentRecord::getIsEmergency() const { return isEmergency; }
InsuranceTier AppointmentRecord::getInsuranceTier() const { return insuranceTier; }
MedicalService AppointmentRecord::getService() const { return service; }
string AppointmentRecord::getDoctorId() const { return doctorId; }

void AppointmentRecord::setAppointmentStatus(string status) {
    if (status != "Scheduled" && status != "Completed" && status != "Cancelled") {
        throw invalid_argument("Invalid appointment status: " + status);
    }
    appointmentStatus = status;
}

void AppointmentRecord::setInsuranceTier(InsuranceTier tier) {
    insuranceTier = tier;
}

void AppointmentRecord::setDoctorId(string doctorId) {
    this->doctorId = doctorId;
}

double AppointmentRecord::calculatePreCoverageAmount() const {
    if (appointmentStatus == "Cancelled") {
        return 0.0;
    }

    double amount = service.getBaseFee();

    if (isEmergency) {
        amount *= 1.5;
    }

    return amount;
}

double AppointmentRecord::calculateBillingAmount() const {
    double coverage = tierCoverageRate(insuranceTier);
    return calculatePreCoverageAmount() * (1.0 - coverage);
}

string AppointmentRecord::tierToString(InsuranceTier tier) {
    switch (tier) {
        case InsuranceTier::ELITE: return "ELITE";
        case InsuranceTier::PLUS:  return "PLUS";
        case InsuranceTier::BASIC: return "BASIC";
        case InsuranceTier::NONE:
        default:                   return "NONE";
    }
}

InsuranceTier AppointmentRecord::stringToTier(string s) {
    if (s == "ELITE")  return InsuranceTier::ELITE;
    if (s == "PLUS")   return InsuranceTier::PLUS;
    if (s == "BASIC")  return InsuranceTier::BASIC;
    if (s == "PREMIUM")  return InsuranceTier::ELITE;
    if (s == "STANDARD") return InsuranceTier::PLUS;
    return InsuranceTier::NONE;
}

double AppointmentRecord::tierCoverageRate(InsuranceTier tier) {
    switch (tier) {
        case InsuranceTier::ELITE: return 1.00;
        case InsuranceTier::PLUS:  return 0.70;
        case InsuranceTier::BASIC: return 0.50;
        case InsuranceTier::NONE:
        default:                   return 0.00;
    }
}

void AppointmentRecord::displayRecord() const {
    cout << "  " << left
         << setw(11) << ("[" + appointmentId + "]")
         << setw(14) << service.getServiceCode()
         << setw(26) << service.getServiceName()
         << setw(14) << appointmentDate
         << setw(12) << appointmentStatus
         << setw(10) << (isEmergency ? "YES" : "NO")
         << setw(8)  << tierToString(insuranceTier)
         << setw(22) << (doctorId.empty() ? "None" : doctorId)
         << "$" << fixed << setprecision(2) << calculateBillingAmount()
         << endl;
}

void AppointmentRecord::displayRecord(const string& doctorName) const {
    string doctorDisplay = doctorId.empty() ? "None" : (doctorId + " " + doctorName);
    cout << "  " << left
         << setw(11) << ("[" + appointmentId + "]")
         << setw(14) << service.getServiceCode()
         << setw(26) << service.getServiceName()
         << setw(14) << appointmentDate
         << setw(12) << appointmentStatus
         << setw(10) << (isEmergency ? "YES" : "NO")
         << setw(8)  << tierToString(insuranceTier)
         << setw(22) << doctorDisplay
         << "$" << fixed << setprecision(2) << calculateBillingAmount()
         << endl;
}