#pragma once
#include <string>
#include <vector>
#include "Person.h"

using namespace std;

class Doctor : public Person {
private:
    string specializationDepartment;
    string assignedClinicCode;
    string contactNumber;
    string licenseNumber;
    vector<string> assignedPatientIds;

    static int doctorCount;
    static int nextDoctorNumber;

public:
    Doctor();
    Doctor(string name, string id, string specialization, string clinicCode = "UNASSIGNED");
    Doctor(string name, string id, string specialization, string clinicCode, string contact, string license);

    string getSpecializationDepartment() const;
    string getAssignedClinicCode() const;
    string getContactNumber() const;
    string getLicenseNumber() const;
    vector<string> getAssignedPatientIds() const;

    void setSpecializationDepartment(string dept);
    void setContactNumber(string contact);
    void setLicenseNumber(string license);

    void assignToClinic(string code);
    void assignPatient(string patientId);
    bool removePatientAssignment(string patientId);
    int getAssignedPatientCount() const;

    void displayRole() const override;
    string getType() const override;

    static int getDoctorCount();
    static std::string generateDoctorId();
    static void resetCounter();
    static void setNextDoctorNumber(int next);
};
