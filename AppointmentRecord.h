#pragma once
#include <string>
#include "MedicalService.h"

using namespace std;

enum class InsuranceTier {
    NONE,
    BASIC,
    PLUS,
    ELITE
};

class AppointmentRecord {
private:
    MedicalService service;
    string appointmentStatus;
    string appointmentDate;
    bool isEmergency;
    InsuranceTier insuranceTier;
    string appointmentId;
    string doctorId;

    static int recordCounter;

public:
    AppointmentRecord();
    AppointmentRecord(MedicalService s, string date, bool emergency = false, InsuranceTier tier = InsuranceTier::NONE, string doctorId = "");

    string getServiceCode() const;
    string getServiceName() const;
    string getAppointmentStatus() const;
    string getAppointmentDate() const;
    string getAppointmentId() const;
    bool getIsEmergency() const;
    InsuranceTier getInsuranceTier() const;
    MedicalService getService() const;
    string getDoctorId() const;

    void setAppointmentStatus(string status);
    void setInsuranceTier(InsuranceTier tier);
    void setDoctorId(string doctorId);

    double calculateBillingAmount() const;
    double calculatePreCoverageAmount() const;

    static string tierToString(InsuranceTier tier);
    static InsuranceTier stringToTier(string s);
    static double tierCoverageRate(InsuranceTier tier);

    void displayRecord() const;
    void displayRecord(const string& doctorName) const;
};