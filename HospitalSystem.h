#pragma once
#include <string>
#include <vector>
#include <map>
#include "Person.h"
#include "Patient.h"
#include "Doctor.h"
#include "MedicalService.h"

using namespace std;

class HospitalSystem {
private:
    vector<Person*> users;
    vector<MedicalService> serviceCatalog;
    string dataDirectory;

    static int systemSessionCount;

    void saveUsersToFile() const;
    void saveServicesToFile() const;
    void loadUsersFromFile();
    void loadServicesFromFile();
    void recalculateIdCounters();

    Patient* findPatientById(string id) const;
    Doctor* findDoctorById(string id) const;
    MedicalService* findServiceByCode(string code);
    string lookupDoctorName(const string& doctorId) const;

    void displayDivider(char ch = '=', int width = 60) const;
    void displayTitle(string title) const;
    void displaySubtitle(string subtitle) const;
    void pauseAndContinue() const;
    string getValidatedInput(string prompt) const;
    int getValidatedInt(string prompt, int min, int max) const;
    double getValidatedDouble(string prompt, double min, double max) const;
    bool confirmAction(string message) const;

    void menuAddPatient();
    void menuAddDoctor();
    void menuAddMedicalService();
    void menuScheduleAppointment();
    void menuSetAppointmentStatus();
    void menuViewAllUsers() const;
    void menuViewServiceCatalog() const;
    void menuSearchUser() const;
    void displayDoctorAppointments(const Doctor* doctor) const;
    void menuViewPatientBill() const;
    void menuSystemStatistics() const;
    void menuEditRecords();
    void menuDeleteRecords();
    void menuViewAllPatients() const;
    void menuViewAllDoctors() const;
    void menuViewAllAppointments() const;
    void editPatient(Patient* p);
    void editDoctor(Doctor* d);
    void editMedicalService(MedicalService* s);

public:
    HospitalSystem();
    HospitalSystem(string dataDir);
    ~HospitalSystem();

    void run();
    void displayMainMenu() const;

    int getUserCount() const;
    int getServiceCount() const;

    static int getSessionCount();
};