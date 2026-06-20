#pragma once
#include <string>
#include <vector>
#include <functional>
#include "Person.h"
#include "MedicalService.h"
#include "AppointmentRecord.h"

using namespace std;

class Patient : public Person {
private:
    string insuranceProvider;
    vector<AppointmentRecord> appointments;
    string contactNumber;
    string dateOfBirth;
    string bloodType;

    static int patientCount;
    static int nextPatientNumber;

public:
    Patient();
    Patient(string name, string id, string insurance);
    Patient(string name, string id, string insurance, string contact, string dob, string blood);

    string getInsuranceProvider() const;
    string getContactNumber() const;
    string getDateOfBirth() const;
    string getBloodType() const;
    vector<AppointmentRecord>& getAppointments();
    const vector<AppointmentRecord>& getAppointments() const;

    void setInsuranceProvider(string provider);
    void setContactNumber(string contact);
    void setDateOfBirth(string dob);
    void setBloodType(string blood);

    void scheduleAppointment(MedicalService s, string date, bool emergency = false, string doctorId = "");
    bool setAppointmentStatus(string appointmentId, string status);
    bool setAppointmentBillingTier(string appointmentId, InsuranceTier tier);
    bool setAppointmentDoctor(string appointmentId, string doctorId);
    double calculateTotalMedicalExpenses() const;
    double calculateTotalPreCoverageExpenses() const;
    int getAppointmentCount() const;

    void displayRole() const override;
    void displayRole(const function<string(const string&)>& doctorLookup) const;
    string getType() const override;

    static int getPatientCount();
    static string generatePatientId();
    static void resetCounter();
    static void setNextPatientNumber(int next);

};