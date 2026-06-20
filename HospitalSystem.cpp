#include "HospitalSystem.h"
#include "UIHelper.h"
#include "FileManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <stdexcept>

using namespace std;

int HospitalSystem::systemSessionCount = 0;

HospitalSystem::HospitalSystem()
    : dataDirectory("data") {
    systemSessionCount++;
    FileManager::createDirectory(dataDirectory);
    loadUsersFromFile();
    loadServicesFromFile();
}

HospitalSystem::HospitalSystem(string dataDir)
    : dataDirectory(dataDir) {
    systemSessionCount++;
    FileManager::createDirectory(dataDirectory);
    loadUsersFromFile();
    loadServicesFromFile();
}

HospitalSystem::~HospitalSystem() {
    saveUsersToFile();
    saveServicesToFile();
    for (Person* p : users) {
        delete p;
    }
    users.clear();
}

int HospitalSystem::getSessionCount() { return systemSessionCount; }
int HospitalSystem::getUserCount() const { return static_cast<int>(users.size()); }
int HospitalSystem::getServiceCount() const { return static_cast<int>(serviceCatalog.size()); }

Patient* HospitalSystem::findPatientById(string id) const {
    for (Person* p : users) {
        if (p->getType() == "Patient" && p->getId() == id) {
            return dynamic_cast<Patient*>(p);
        }
    }
    return nullptr;
}

Doctor* HospitalSystem::findDoctorById(string id) const {
    for (Person* p : users) {
        if (p->getType() == "Doctor" && p->getId() == id) {
            return dynamic_cast<Doctor*>(p);
        }
    }
    return nullptr;
}

MedicalService* HospitalSystem::findServiceByCode(string code) {
    for (auto& s : serviceCatalog) {
        if (s.getServiceCode() == code) return &s;
    }
    return nullptr;
}

string HospitalSystem::lookupDoctorName(const string& doctorId) const {
    if (doctorId.empty()) return "";
    const Doctor* d = findDoctorById(doctorId);
    return d ? d->getName() : "";
}

void HospitalSystem::displayMainMenu() const {
    UIHelper::printDivider('=', 60);
    UIHelper::printCenteredTitle("HOSPITAL MANAGEMENT SYSTEM MENU", 60);
    UIHelper::printDivider('=', 60);
    cout << endl;
    cout << "  1.  Add New Patient" << endl;
    cout << "  2.  Add New Doctor" << endl;
    cout << "  3.  Add New Medical Service" << endl;
    cout << "  4.  Schedule Appointment for Patient" << endl;
    cout << "  5.  Set Appointment Status / Bill Modifier" << endl;
    cout << "  6.  View All Users (Runtime Polymorphism)" << endl;
    cout << "  7.  View Medical Services Catalog" << endl;
    cout << "  8.  Search User by ID" << endl;
    cout << "  9.  View Patient Billing Summary" << endl;
    cout << "  10. System Statistics" << endl;
    cout << "  11. Edit Records (Patient/Doctor/Service)" << endl;
    cout << "  12. Delete Records (Patient/Doctor/Service)" << endl;
    cout << "  13. View All Patients" << endl;
    cout << "  14. View All Doctors" << endl;
    cout << "  15. View All Appointments" << endl;
    cout << "  0.  Exit System" << endl;
    cout << endl;
    UIHelper::printDivider('-', 60);
}

void HospitalSystem::menuAddPatient() {
    while (true) {
        UIHelper::printSectionHeader("--- Add New Patient ---");

        string id = Patient::generatePatientId();
        cout << "  Auto-generated Patient ID: " << id << endl << endl;

        string name = UIHelper::getInput("  Enter Patient Name          : ");
        if (name.empty()) throw invalid_argument("Patient name cannot be empty.");

        string insurance = UIHelper::getInput("  Enter Insurance Provider    : ");
        if (insurance.empty()) insurance = "None";

        string contact = UIHelper::getInput("  Enter Contact Number        : ");
        if (contact.empty()) contact = "N/A";

        string dob = UIHelper::getDateInput("  Enter Date of Birth (YYYY-MM-DD): ", true);

        string blood = UIHelper::getInput("  Enter Blood Type            : ");
        if (blood.empty()) blood = "Unknown";

        Patient* p = new Patient(name, id, insurance, contact, dob, blood);
        users.push_back(p);

        saveUsersToFile();
        FileManager::logActivity(dataDirectory + "/activity.log",
            "Patient added: " + name + " (ID: " + id + ")");

        cout << endl;
        UIHelper::printSuccess("Patient '" + name + "' added with ID: " + id);

        cout << endl;
        if (!UIHelper::getYesNo("  Add another patient?")) break;
        cout << endl;
    }
}

void HospitalSystem::menuAddDoctor() {
    while (true) {
        UIHelper::printSectionHeader("--- Add New Doctor ---");

        string id = Doctor::generateDoctorId();
        cout << "  Auto-generated Doctor ID: " << id << endl << endl;

        string name = UIHelper::getInput("  Enter Doctor Name           : ");
        if (name.empty()) throw invalid_argument("Doctor name cannot be empty.");

        string spec = UIHelper::getInput("  Enter Specialization Dept   : ");
        if (spec.empty()) spec = "General Medicine";

        string clinic = UIHelper::getInput("  Enter Clinic Code           : ");
        if (clinic.empty()) clinic = "UNASSIGNED";

        string contact = UIHelper::getInput("  Enter Contact Number        : ");
        if (contact.empty()) contact = "N/A";

        string license = UIHelper::getInput("  Enter License Number        : ");
        if (license.empty()) license = "N/A";

        Doctor* d = new Doctor(name, id, spec, clinic, contact, license);
        users.push_back(d);

        saveUsersToFile();
        FileManager::logActivity(dataDirectory + "/activity.log",
            "Doctor added: " + name + " (ID: " + id + ")");

        cout << endl;
        UIHelper::printSuccess("Doctor '" + name + "' added with ID: " + id);

        cout << endl;
        if (!UIHelper::getYesNo("  Add another doctor?")) break;
        cout << endl;
    }
}

void HospitalSystem::menuAddMedicalService() {
    while (true) {
        UIHelper::printSectionHeader("--- Add New Medical Service ---");

        string code = MedicalService::generateServiceCode();
        cout << "  Auto-generated Service Code: " << code << endl << endl;

        string name = UIHelper::getInput("  Enter Service Name          : ");
        if (name.empty()) throw invalid_argument("Service name cannot be empty.");

        bool useDefault = UIHelper::getYesNo("  Use default base fee ($50.00)?");
        MedicalService svc;
        if (useDefault) {
            svc = MedicalService(code, name);
        } else {
            double fee = UIHelper::getDoubleInput("  Enter Base Fee ($): ", 0.0, 99999.99);
            svc = MedicalService(code, name, fee);
        }

        serviceCatalog.push_back(svc);
        saveServicesToFile();
        FileManager::logActivity(dataDirectory + "/activity.log",
            "Service added: " + name + " (Code: " + code + ")");

        cout << endl;
        UIHelper::printSuccess("Medical service '" + name + "' (Code: " + code + ") added.");

        cout << endl;
        if (!UIHelper::getYesNo("  Add another medical service?")) break;
        cout << endl;
    }
}

void HospitalSystem::menuScheduleAppointment() {
    UIHelper::printSectionHeader("--- Schedule Appointment for Patient ---");

    if (serviceCatalog.empty()) {
        UIHelper::printWarning("No medical services available. Please add services first.");
        return;
    }

    string patientId = UIHelper::getInput("  Enter Patient ID (PAT####): ");
    Patient* patient = findPatientById(patientId);
    if (!patient) {
        throw runtime_error("Patient with ID '" + patientId + "' not found.");
    }

    cout << endl;
    cout << "  Patient: " << patient->getName() << " | Insurance: "
         << patient->getInsuranceProvider() << endl;
    cout << endl;

    cout << "  Available Medical Services:" << endl;
    UIHelper::printTableHeader("Code", "Service Name", "Base Fee", 12, 35, 12);
    for (const auto& s : serviceCatalog) {
        UIHelper::printTableRow(s.getServiceCode(), s.getServiceName(),
            UIHelper::formatCurrency(s.getBaseFee()), 12, 35, 12);
    }
    UIHelper::printDivider('-', 61);

    string svcCode = UIHelper::getInput("\n  Enter Service Code (SVC###) to schedule: ");
    MedicalService* svc = findServiceByCode(svcCode);
    if (!svc) {
        throw runtime_error("Service '" + svcCode + "' not found in catalog.");
    }

    for (const auto& appt : patient->getAppointments()) {
        if (appt.getServiceCode() == svcCode && appt.getAppointmentStatus() == "Scheduled") {
            throw runtime_error("Patient already has an active scheduled appointment for this service.");
        }
    }

    string date = UIHelper::getDateInput("  Enter Appointment Date (YYYY-MM-DD): ", true);

    bool emergency = UIHelper::getYesNo("  Is this an emergency appointment?");

    string doctorId = "";
    if (UIHelper::getYesNo("  Assign a doctor to this appointment?")) {
        doctorId = UIHelper::getInput("  Enter Doctor ID (DOC####): ");
        Doctor* doctor = findDoctorById(doctorId);
        if (!doctor) {
            throw runtime_error("Doctor with ID '" + doctorId + "' not found.");
        }
        try {
            doctor->assignPatient(patientId);
        } catch (const runtime_error&) {
        }
    }

    patient->scheduleAppointment(*svc, date, emergency, doctorId);
    saveUsersToFile();

    double estimatedBill = svc->getBaseFee();
    if (emergency) estimatedBill *= 1.5;

    FileManager::logActivity(dataDirectory + "/activity.log",
        "Appointment scheduled: Patient " + patientId + " | Service: " + svcCode
        + (doctorId.empty() ? "" : (" | Doctor: " + doctorId)));

    cout << endl;
    UIHelper::printSuccess("Appointment scheduled for " + patient->getName() + ".");
    cout << "  Service     : " << svc->getServiceName() << endl;
    cout << "  Date        : " << date << endl;
    cout << "  Emergency   : " << (emergency ? "YES" : "NO") << endl;
    cout << "  Doctor      : " << (doctorId.empty() ? "None" : (doctorId + " " + lookupDoctorName(doctorId))) << endl;
    cout << "  Est. Bill   : " << UIHelper::formatCurrency(estimatedBill) << " (before insurance)" << endl;
}

void HospitalSystem::menuSetAppointmentStatus() {
    UIHelper::printSectionHeader("--- Set Appointment Status / Bill Modifier ---");

    string patientId = UIHelper::getInput("  Enter Patient ID (PAT####): ");
    Patient* patient = findPatientById(patientId);
    if (!patient) {
        throw runtime_error("Patient with ID '" + patientId + "' not found.");
    }

    if (patient->getAppointments().empty()) {
        UIHelper::printWarning("Patient has no appointments.");
        return;
    }

    cout << endl;
    cout << "  Appointments for " << patient->getName() << ":" << endl;
    cout << "  " << string(129, '-') << endl;
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
    for (const auto& appt : patient->getAppointments()) {
        appt.displayRecord(lookupDoctorName(appt.getDoctorId()));
    }
    cout << "  " << string(129, '-') << endl;

    string apptId = UIHelper::getInput("\n  Enter Appointment ID (APT#####) to update: ");

    bool found = false;
    for (const auto& appt : patient->getAppointments()) {
        if (appt.getAppointmentId() == apptId) { found = true; break; }
    }
    if (!found) {
        throw runtime_error("No appointment found with ID '" + apptId + "'.");
    }

    cout << endl;
    cout << "  Update options:" << endl;
    cout << "    1. Change Status" << endl;
    cout << "    2. Change Insurance Tier (Bill Modifier)" << endl;
    cout << "    3. Both" << endl;
    int updateChoice = UIHelper::getIntInput("  Select (1-3): ", 1, 3);

    if (updateChoice == 1 || updateChoice == 3) {
        cout << "  New Status:" << endl;
        cout << "    1. Scheduled" << endl;
        cout << "    2. Completed" << endl;
        cout << "    3. Cancelled" << endl;
        int statusChoice = UIHelper::getIntInput("  Select (1-3): ", 1, 3);
        string status;
        switch (statusChoice) {
            case 1: status = "Scheduled"; break;
            case 2: status = "Completed"; break;
            case 3: status = "Cancelled"; break;
            default: status = "Scheduled"; break;
        }
        if (!patient->setAppointmentStatus(apptId, status)) {
            UIHelper::printWarning("Status update failed for appointment: " + apptId);
        } else {
            UIHelper::printSuccess("Status updated to: " + status);
        }
    }

    if (updateChoice == 2 || updateChoice == 3) {
        cout << "  New Insurance Tier:" << endl;
        cout << "    1. NONE (no coverage)" << endl;
        cout << "    2. BASIC (50% coverage)" << endl;
        cout << "    3. PLUS (70% coverage)" << endl;
        cout << "    4. ELITE (100% coverage)" << endl;
        int tierChoice = UIHelper::getIntInput("  Select (1-4): ", 1, 4);
        InsuranceTier tier;
        switch (tierChoice) {
            case 2: tier = InsuranceTier::BASIC; break;
            case 3: tier = InsuranceTier::PLUS;  break;
            case 4: tier = InsuranceTier::ELITE; break;
            default: tier = InsuranceTier::NONE; break;
        }
        if (!patient->setAppointmentBillingTier(apptId, tier)) {
            UIHelper::printWarning("Tier update failed.");
        } else {
            UIHelper::printSuccess("Insurance tier updated to: " + AppointmentRecord::tierToString(tier));
        }
    }

    saveUsersToFile();
    FileManager::logActivity(dataDirectory + "/activity.log",
        "Appointment updated: Patient " + patientId + " | Appointment: " + apptId);
}

void HospitalSystem::menuEditRecords() {
    while (true) {
        UIHelper::clearScreen();
        UIHelper::printSectionHeader("--- Edit Records Menu ---");
        cout << "  1. Edit Patient" << endl;
        cout << "  2. Edit Doctor" << endl;
        cout << "  3. Edit Medical Service" << endl;
        cout << "  0. Return to Main Menu" << endl;
        cout << endl;
        UIHelper::printDivider('-', 60);

        int choice = -1;
        try {
            choice = UIHelper::getIntInput("  Enter choice: ", 0, 3);
        } catch (...) {
            UIHelper::printError("Invalid menu choice.");
            UIHelper::pressEnterToContinue();
            continue;
        }

        if (choice == 0) return;

        cout << endl;
        try {
            if (choice == 1) {
                string patientId = UIHelper::getInput("  Enter Patient ID (PAT####): ");
                Patient* patient = findPatientById(patientId);
                if (!patient) {
                    throw runtime_error("Patient with ID '" + patientId + "' not found.");
                }
                editPatient(patient);
            } else if (choice == 2) {
                string doctorId = UIHelper::getInput("  Enter Doctor ID (DOC####): ");
                Doctor* doctor = findDoctorById(doctorId);
                if (!doctor) {
                    throw runtime_error("Doctor with ID '" + doctorId + "' not found.");
                }
                editDoctor(doctor);
            } else if (choice == 3) {
                string serviceCode = UIHelper::getInput("  Enter Medical Service Code (SVC###): ");
                MedicalService* service = findServiceByCode(serviceCode);
                if (!service) {
                    throw runtime_error("Medical Service with Code '" + serviceCode + "' not found.");
                }
                editMedicalService(service);
            }
        } catch (const exception& e) {
            UIHelper::printError(e.what());
            UIHelper::pressEnterToContinue();
            continue;
        }
        UIHelper::pressEnterToContinue();
    }
}

void HospitalSystem::menuDeleteRecords() {
    while (true) {
        UIHelper::clearScreen();
        UIHelper::printSectionHeader("--- Delete Records Menu ---");
        cout << "  1. Delete Patient" << endl;
        cout << "  2. Delete Doctor" << endl;
        cout << "  3. Delete Medical Service" << endl;
        cout << "  0. Return to Main Menu" << endl;
        cout << endl;
        UIHelper::printDivider('-', 60);

        int choice = -1;
        try {
            choice = UIHelper::getIntInput("  Enter choice: ", 0, 3);
        } catch (...) {
            UIHelper::printError("Invalid menu choice.");
            UIHelper::pressEnterToContinue();
            continue;
        }

        if (choice == 0) return;

        cout << endl;
        try {
            if (choice == 1) {
                string patientId = UIHelper::getInput("  Enter Patient ID (PAT####): ");
                Patient* patient = findPatientById(patientId);
                if (!patient) {
                    throw runtime_error("Patient with ID '" + patientId + "' not found.");
                }
                if (UIHelper::getYesNo("  Are you sure you want to delete patient '" + patient->getName() + "'?")) {
                    for (Person* p : users) {
                        if (p->getType() == "Doctor") {
                            Doctor* doc = dynamic_cast<Doctor*>(p);
                            if (doc) {
                                doc->removePatientAssignment(patientId);
                            }
                        }
                    }
                    auto it = find(users.begin(), users.end(), patient);
                    if (it != users.end()) {
                        users.erase(it);
                    }
                    string name = patient->getName();
                    delete patient;
                    saveUsersToFile();
                    FileManager::logActivity(dataDirectory + "/activity.log",
                        "Patient deleted: " + name + " (ID: " + patientId + ")");
                    UIHelper::printSuccess("Patient '" + name + "' successfully deleted.");
                } else {
                    UIHelper::printInfo("Deletion cancelled.");
                }
            } else if (choice == 2) {
                string doctorId = UIHelper::getInput("  Enter Doctor ID (DOC####): ");
                Doctor* doctor = findDoctorById(doctorId);
                if (!doctor) {
                    throw runtime_error("Doctor with ID '" + doctorId + "' not found.");
                }
                if (UIHelper::getYesNo("  Are you sure you want to delete doctor '" + doctor->getName() + "'?")) {
                    auto it = find(users.begin(), users.end(), doctor);
                    if (it != users.end()) {
                        users.erase(it);
                    }
                    string name = doctor->getName();
                    delete doctor;
                    saveUsersToFile();
                    FileManager::logActivity(dataDirectory + "/activity.log",
                        "Doctor deleted: " + name + " (ID: " + doctorId + ")");
                    UIHelper::printSuccess("Doctor '" + name + "' successfully deleted.");
                } else {
                    UIHelper::printInfo("Deletion cancelled.");
                }
            } else if (choice == 3) {
                string serviceCode = UIHelper::getInput("  Enter Medical Service Code (SVC###): ");
                MedicalService* service = findServiceByCode(serviceCode);
                if (!service) {
                    throw runtime_error("Medical Service with Code '" + serviceCode + "' not found.");
                }
                if (UIHelper::getYesNo("  Are you sure you want to delete medical service '" + service->getServiceName() + "'?")) {
                    auto it = find_if(serviceCatalog.begin(), serviceCatalog.end(),
                        [&serviceCode](const MedicalService& s) { return s.getServiceCode() == serviceCode; });
                    if (it != serviceCatalog.end()) {
                        string name = it->getServiceName();
                        serviceCatalog.erase(it);
                        saveServicesToFile();
                        FileManager::logActivity(dataDirectory + "/activity.log",
                            "Medical Service deleted: " + name + " (Code: " + serviceCode + ")");
                        UIHelper::printSuccess("Medical Service '" + name + "' successfully deleted.");
                    }
                } else {
                    UIHelper::printInfo("Deletion cancelled.");
                }
            }
        } catch (const exception& e) {
            UIHelper::printError(e.what());
            UIHelper::pressEnterToContinue();
            continue;
        }
        UIHelper::pressEnterToContinue();
    }
}

void HospitalSystem::editPatient(Patient* p) {
    UIHelper::printSectionHeader("--- Edit Patient: " + p->getName() + " (" + p->getId() + ") ---");
    cout << "  (Press [ENTER] to keep the current value)" << endl << endl;

    string newName = UIHelper::getInput("  Enter new Name [" + p->getName() + "]: ");
    if (!newName.empty()) {
        p->setName(newName);
    }

    string newInsurance = UIHelper::getInput("  Enter new Insurance Provider [" + p->getInsuranceProvider() + "]: ");
    if (!newInsurance.empty()) {
        p->setInsuranceProvider(newInsurance);
    }
    string newContact = UIHelper::getInput("  Enter new Contact Number [" + p->getContactNumber() + "]: ");
    if (!newContact.empty()) {
        p->setContactNumber(newContact);
    }

    string newDob = UIHelper::getDateInput("  Enter new Date of Birth (YYYY-MM-DD) [" + p->getDateOfBirth() + "]: ", true);
    if (newDob != "TBD") {
        p->setDateOfBirth(newDob);
    }

    string newBlood = UIHelper::getInput("  Enter new Blood Type [" + p->getBloodType() + "]: ");
    if (!newBlood.empty()) {
        p->setBloodType(newBlood);
    }

    saveUsersToFile();
    FileManager::logActivity(dataDirectory + "/activity.log",
        "Patient edited: " + p->getName() + " (ID: " + p->getId() + ")");
    UIHelper::printSuccess("Patient details updated successfully.");
}

void HospitalSystem::editDoctor(Doctor* d) {
    UIHelper::printSectionHeader("--- Edit Doctor: " + d->getName() + " (" + d->getId() + ") ---");
    cout << "  (Press [ENTER] to keep the current value)" << endl << endl;

    string newName = UIHelper::getInput("  Enter new Name [" + d->getName() + "]: ");
    if (!newName.empty()) {
        d->setName(newName);
    }

    string newSpec = UIHelper::getInput("  Enter new Specialization [" + d->getSpecializationDepartment() + "]: ");
    if (!newSpec.empty()) {
        d->setSpecializationDepartment(newSpec);
    }

    string newClinic = UIHelper::getInput("  Enter new Clinic Code [" + d->getAssignedClinicCode() + "]: ");
    if (!newClinic.empty()) {
        d->assignToClinic(newClinic);
    }

    string newContact = UIHelper::getInput("  Enter new Contact Number [" + d->getContactNumber() + "]: ");
    if (!newContact.empty()) {
        d->setContactNumber(newContact);
    }

    string newLicense = UIHelper::getInput("  Enter new License Number [" + d->getLicenseNumber() + "]: ");
    if (!newLicense.empty()) {
        d->setLicenseNumber(newLicense);
    }

    saveUsersToFile();
    FileManager::logActivity(dataDirectory + "/activity.log",
        "Doctor edited: " + d->getName() + " (ID: " + d->getId() + ")");
    UIHelper::printSuccess("Doctor details updated successfully.");
}

void HospitalSystem::editMedicalService(MedicalService* s) {
    UIHelper::printSectionHeader("--- Edit Medical Service: " + s->getServiceName() + " (" + s->getServiceCode() + ") ---");
    cout << "  (Press [ENTER] to keep the current value)" << endl << endl;

    string newName = UIHelper::getInput("  Enter new Service Name [" + s->getServiceName() + "]: ");
    if (!newName.empty()) {
        s->setServiceName(newName);
    }

    string newFeeStr = UIHelper::getInput("  Enter new Base Fee [" + UIHelper::formatCurrency(s->getBaseFee()) + "]: ");
    if (!newFeeStr.empty()) {
        try {
            double newFee = stod(newFeeStr);
            if (newFee < 0.0 || newFee > 99999.99) {
                throw out_of_range("Fee must be between 0.0 and 99999.99");
            }
            s->setBaseFee(newFee);
        } catch (...) {
            UIHelper::printError("Invalid fee input. Fee was not updated.");
        }
    }

    saveServicesToFile();
    FileManager::logActivity(dataDirectory + "/activity.log",
        "Medical Service edited: " + s->getServiceName() + " (Code: " + s->getServiceCode() + ")");
    UIHelper::printSuccess("Medical Service details updated successfully.");
}