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

        string name = UIHelper::getInput("  Enter Patient Name          : ");
        if (name.empty()) {
            throw invalid_argument("Patient name cannot be empty. No patient was added.");
        }

        recalculateIdCounters();
        string id = Patient::generatePatientId();
        cout << "  Auto-generated Patient ID: " << id << endl << endl;

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

        string name = UIHelper::getInput("  Enter Doctor Name           : ");
        if (name.empty()) {
            throw invalid_argument("Doctor name cannot be empty. No doctor was added.");
        }

        recalculateIdCounters();
        string id = Doctor::generateDoctorId();
        cout << "  Auto-generated Doctor ID: " << id << endl << endl;

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

        string name = UIHelper::getInput("  Enter Service Name          : ");
        if (name.empty()) {
            throw invalid_argument("Service name cannot be empty. No service was added.");
        }

        recalculateIdCounters();
        string code = MedicalService::generateServiceCode();
        cout << "  Auto-generated Service Code: " << code << endl << endl;

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
                    recalculateIdCounters();
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
                    recalculateIdCounters();
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
                        recalculateIdCounters();
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

void HospitalSystem::menuViewAllUsers() const {
    UIHelper::printSectionHeader("--- All System Users (Runtime Polymorphism) ---");

    if (users.empty()) {
        UIHelper::printWarning("No users registered in the system.");
        return;
    }

    int patientCount = 0, doctorCount = 0;
    for (const Person* p : users) {
        if (p->getType() == "Patient") patientCount++;
        else doctorCount++;
    }

    cout << "  Total Users: " << users.size()
         << "  (Patients: " << patientCount
         << " | Doctors: " << doctorCount << ")" << endl;
    cout << endl;

    for (const Person* p : users) {
        if (p->getType() == "Patient") {
            const Patient* pat = dynamic_cast<const Patient*>(p);
            pat->displayRole([this](const string& id) { return lookupDoctorName(id); });
        } else {
            p->displayRole();
        }
        cout << endl;
    }
}

void HospitalSystem::menuViewServiceCatalog() const {
    UIHelper::printSectionHeader("--- Medical Services Catalog ---");

    if (serviceCatalog.empty()) {
        UIHelper::printWarning("No services available in the catalog.");
        return;
    }

    cout << "  Total Services: " << serviceCatalog.size() << endl << endl;
    UIHelper::printTableHeader("Service Code", "Service Name", "Base Fee", 14, 33, 12);
    for (const auto& s : serviceCatalog) {
        UIHelper::printTableRow(s.getServiceCode(), s.getServiceName(),
            UIHelper::formatCurrency(s.getBaseFee()), 14, 33, 12);
    }
    UIHelper::printDivider('-', 61);
}

void HospitalSystem::menuSearchUser() const {
    UIHelper::printSectionHeader("--- Search User by ID ---");

    string id = UIHelper::getInput("  Enter User ID (PAT#### or DOC####): ");
    if (id.empty()) {
        UIHelper::printError("ID cannot be empty.");
        return;
    }

    bool found = false;
    for (const Person* p : users) {
        if (p->getId() == id) {
            cout << endl;
            if (p->getType() == "Patient") {
                const Patient* pat = dynamic_cast<const Patient*>(p);
                pat->displayRole([this](const string& did) { return lookupDoctorName(did); });
            } else {
                p->displayRole();
                const Doctor* doc = dynamic_cast<const Doctor*>(p);
                displayDoctorAppointments(doc);
            }
            found = true;
            break;
        }
    }

    if (!found) {
        UIHelper::printWarning("No user found with ID: " + id);
    }
}

void HospitalSystem::displayDoctorAppointments(const Doctor* doctor) const {
    struct DoctorApptEntry {
        string patientId;
        string patientName;
        string appointmentId;
        string serviceName;
        string date;
        string status;
    };

    vector<DoctorApptEntry> scheduledList;
    vector<DoctorApptEntry> otherList;

    for (const Person* p : users) {
        if (p->getType() != "Patient") continue;
        const Patient* pat = dynamic_cast<const Patient*>(p);
        for (const auto& appt : pat->getAppointments()) {
            if (appt.getDoctorId() != doctor->getId()) continue;
            DoctorApptEntry entry{
                pat->getId(), pat->getName(), appt.getAppointmentId(),
                appt.getServiceName(), appt.getAppointmentDate(), appt.getAppointmentStatus()
            };
            if (entry.status == "Scheduled") {
                scheduledList.push_back(entry);
            } else {
                otherList.push_back(entry);
            }
        }
    }

    cout << endl;
    cout << "  --- Assigned Appointments ---" << endl;
    cout << "  Total Assigned Appointments : " << (scheduledList.size() + otherList.size()) << endl;

    if (scheduledList.empty() && otherList.empty()) {
        UIHelper::printInfo("No appointments are currently assigned to this doctor.");
        return;
    }

    cout << endl;
    cout << "  " << left
         << setw(11) << "Appt ID"
         << setw(11) << "Patient ID"
         << setw(24) << "Patient Name"
         << setw(26) << "Service Name"
         << setw(14) << "Date"
         << setw(12) << "Status"
         << endl;
    cout << "  " << string(98, '-') << endl;

    for (const auto& e : scheduledList) {
        cout << "  " << left
             << setw(11) << ("[" + e.appointmentId + "]")
             << setw(11) << e.patientId
             << setw(24) << e.patientName
             << setw(26) << e.serviceName
             << setw(14) << e.date
             << setw(12) << e.status
             << endl;
    }
    for (const auto& e : otherList) {
        cout << "  " << left
             << setw(11) << ("[" + e.appointmentId + "]")
             << setw(11) << e.patientId
             << setw(24) << e.patientName
             << setw(26) << e.serviceName
             << setw(14) << e.date
             << setw(12) << e.status
             << endl;
    }
    cout << "  " << string(98, '-') << endl;
}

void HospitalSystem::menuViewPatientBill() const {
    UIHelper::printSectionHeader("--- Patient Billing Summary ---");

    string patientId = UIHelper::getInput("  Enter Patient ID (PAT####): ");
    const Patient* patient = nullptr;

    for (const Person* p : users) {
        if (p->getType() == "Patient" && p->getId() == patientId) {
            patient = dynamic_cast<const Patient*>(p);
            break;
        }
    }

    if (!patient) {
        throw runtime_error("Patient with ID '" + patientId + "' not found.");
    }

    cout << endl;
    UIHelper::printDivider('=', 60);
    UIHelper::printCenteredTitle("BILLING STATEMENT", 60);
    UIHelper::printDivider('=', 60);
    cout << "  Patient Name  : " << patient->getName() << endl;
    cout << "  Patient ID    : " << patient->getId() << endl;
    cout << "  Insurance     : " << patient->getInsuranceProvider() << endl;
    UIHelper::printDivider('-', 60);

    const auto& appointments = patient->getAppointments();
    if (appointments.empty()) {
        UIHelper::printInfo("No appointments on record.");
    } else {
        cout << "  " << left
             << setw(13) << "Service Code"
             << setw(22) << "Service Name"
             << setw(11) << "Status"
             << setw(7)  << "Tier"
             << setw(25) << "Doctor"
             << setw(13) << "Before Ins."
             << setw(13) << "After Ins."
             << endl;
        cout << "  " << string(104, '-') << endl;
        for (const auto& appt : appointments) {
            string docDisplay = appt.getDoctorId().empty()
                ? "None"
                : (appt.getDoctorId() + " " + lookupDoctorName(appt.getDoctorId()));
            cout << "  " << left
                 << setw(13) << appt.getServiceCode()
                 << setw(22) << appt.getServiceName()
                 << setw(11) << appt.getAppointmentStatus()
                 << setw(7)  << AppointmentRecord::tierToString(appt.getInsuranceTier())
                 << setw(25) << docDisplay
                 << setw(13) << UIHelper::formatCurrency(appt.calculatePreCoverageAmount())
                 << setw(13) << UIHelper::formatCurrency(appt.calculateBillingAmount())
                 << endl;
        }
        cout << "  " << string(104, '-') << endl;
        cout << "  " << right << setw(53) << "TOTAL BEFORE INSURANCE: "
             << setw(13) << UIHelper::formatCurrency(patient->calculateTotalPreCoverageExpenses()) << endl;
        cout << "  " << right << setw(53) << "TOTAL DUE (AFTER INSURANCE): "
             << setw(13) << UIHelper::formatCurrency(patient->calculateTotalMedicalExpenses()) << endl;
    }
    UIHelper::printDivider('=', 60);
}

void HospitalSystem::menuSystemStatistics() const {
    UIHelper::printSectionHeader("--- System Statistics ---");

    int patientCount = 0, doctorCount = 0;
    double totalRevenue = 0.0;
    int totalAppointments = 0;
    int scheduledCount = 0, completedCount = 0, cancelledCount = 0;

    for (const Person* p : users) {
        if (p->getType() == "Patient") {
            patientCount++;
            const Patient* pat = dynamic_cast<const Patient*>(p);
            totalRevenue += pat->calculateTotalPreCoverageExpenses();
            for (const auto& appt : pat->getAppointments()) {
                totalAppointments++;
                string status = appt.getAppointmentStatus();
                if (status == "Scheduled")   scheduledCount++;
                else if (status == "Completed")  completedCount++;
                else if (status == "Cancelled")  cancelledCount++;
            }
        } else {
            doctorCount++;
        }
    }

    cout << "  Registered Patients      : " << patientCount << endl;
    cout << "  Registered Doctors       : " << doctorCount << endl;
    cout << "  Medical Services         : " << serviceCatalog.size() << endl;
    cout << "  Total Appointments       : " << totalAppointments << endl;
    UIHelper::printDivider('-', 60);
    cout << "  Scheduled                : " << scheduledCount << endl;
    cout << "  Completed                : " << completedCount << endl;
    cout << "  Cancelled                : " << cancelledCount << endl;
    UIHelper::printDivider('-', 60);
    cout << "  Total Projected Revenue  : " << UIHelper::formatCurrency(totalRevenue) << endl;
    cout << "  System Session Number    : " << systemSessionCount << endl;
    UIHelper::printDivider('=', 60);
}

void HospitalSystem::saveUsersToFile() const {
    string path = dataDirectory + "/users.dat";
    ostringstream oss;

    for (const Person* p : users) {
        if (p->getType() == "Patient") {
            const Patient* pat = dynamic_cast<const Patient*>(p);
            oss << "PATIENT|"
                << pat->getId() << "|"
                << pat->getName() << "|"
                << pat->getInsuranceProvider() << "|"
                << pat->getContactNumber() << "|"
                << pat->getDateOfBirth() << "|"
                << pat->getBloodType() << "\n";

            for (const auto& appt : pat->getAppointments()) {
                oss << "APPOINTMENT|"
                    << pat->getId() << "|"
                    << appt.getServiceCode() << "|"
                    << appt.getServiceName() << "|"
                    << appt.getService().getBaseFee() << "|"
                    << appt.getAppointmentStatus() << "|"
                    << appt.getAppointmentDate() << "|"
                    << (appt.getIsEmergency() ? "1" : "0") << "|"
                    << AppointmentRecord::tierToString(appt.getInsuranceTier()) << "|"
                    << appt.getDoctorId() << "\n";
            }
        } else if (p->getType() == "Doctor") {
            const Doctor* doc = dynamic_cast<const Doctor*>(p);
            oss << "DOCTOR|"
                << doc->getId() << "|"
                << doc->getName() << "|"
                << doc->getSpecializationDepartment() << "|"
                << doc->getAssignedClinicCode() << "|"
                << doc->getContactNumber() << "|"
                << doc->getLicenseNumber();
            for (const auto& pid : doc->getAssignedPatientIds()) {
                oss << "|" << pid;
            }
            oss << "\n";
        }
    }

    FileManager::writeToFile(path, oss.str());
}

void HospitalSystem::saveServicesToFile() const {
    string path = dataDirectory + "/services.dat";
    ostringstream oss;
    for (const auto& s : serviceCatalog) {
        oss << s.getServiceCode() << "|"
            << s.getServiceName() << "|"
            << s.getBaseFee() << "\n";
    }
    FileManager::writeToFile(path, oss.str());
}

void HospitalSystem::loadUsersFromFile() {
    string path = dataDirectory + "/users.dat";
    if (!FileManager::fileExists(path)) return;

    vector<string> lines = FileManager::readLines(path);
    map<string, Patient*> patientMap;

    for (const string& line : lines) {
        vector<string> fields;
        stringstream ss(line);
        string field;
        while (getline(ss, field, '|')) fields.push_back(field);
        if (fields.empty()) continue;
        try {
            if (fields[0] == "PATIENT" && fields.size() >= 7) {
                Patient* p = new Patient(fields[2], fields[1], fields[3], fields[4], fields[5], fields[6]);
                users.push_back(p);
                patientMap[fields[1]] = p;
            } else if (fields[0] == "APPOINTMENT" && fields.size() >= 9) {
                string patId = fields[1];
                if (patientMap.count(patId)) {
                    double fee = stod(fields[4]);
                    MedicalService svc(fields[2], fields[3], fee);
                    InsuranceTier tier = AppointmentRecord::stringToTier(fields[8]);
                    bool emergency = (fields[7] == "1");
                    string doctorId = (fields.size() >= 10) ? fields[9] : "";
                    patientMap[patId]->scheduleAppointment(svc, fields[6], emergency, doctorId);
                    auto& appts = patientMap[patId]->getAppointments();
                    if (!appts.empty()) {
                        string status = fields[5];
                        if (status == "In Progress") status = "Scheduled";
                        appts.back().setAppointmentStatus(status);
                        appts.back().setInsuranceTier(tier);
                    }
                }
            } else if (fields[0] == "DOCTOR" && fields.size() >= 7) {
                Doctor* d = new Doctor(fields[2], fields[1], fields[3], fields[4], fields[5], fields[6]);
                for (size_t i = 7; i < fields.size(); ++i) {
                    if (!fields[i].empty()) d->assignPatient(fields[i]);
                }
                users.push_back(d);
            }
        } catch (const exception& e) {
            cerr << "[LOAD WARNING] Skipping corrupted record: " << e.what() << endl;
        }
    }
    int maxPatientNum = 0, maxDoctorNum = 0;
    for (Person* p : users) {
        if (p->getType() == "Patient") {
            string id = p->getId();
            if (id.rfind("PAT", 0) == 0) {
                int num = stoi(id.substr(3));
                maxPatientNum = max(maxPatientNum, num);
            }
        } else if (p->getType() == "Doctor") {
            string id = p->getId();
            if (id.rfind("DOC", 0) == 0) {
                int num = stoi(id.substr(3));
                maxDoctorNum = max(maxDoctorNum, num);
            }
        }
    }
    Patient::setNextPatientNumber(maxPatientNum + 1);
    Doctor::setNextDoctorNumber(maxDoctorNum + 1);
}

void HospitalSystem::loadServicesFromFile() {
    string path = dataDirectory + "/services.dat";
    if (!FileManager::fileExists(path)) return;

    vector<string> lines = FileManager::readLines(path);
    int maxServiceNum = 0;
    for (const string& line : lines) {
        vector<string> fields;
        stringstream ss(line);
        string field;
        while (getline(ss, field, '|')) fields.push_back(field);

        if (fields.size() < 3) continue;
        try {
            double fee = stod(fields[2]);
            serviceCatalog.push_back(MedicalService(fields[0], fields[1], fee));
            string code = fields[0];
            if (code.rfind("SVC", 0) == 0) {
                int num = stoi(code.substr(3));
                maxServiceNum = max(maxServiceNum, num);
            }
        } catch (const exception& e) {
            cerr << "[LOAD WARNING] Skipping corrupted service: " << e.what() << endl;
        }
    }
    MedicalService::setNextServiceNumber(maxServiceNum + 1);
}

void HospitalSystem::recalculateIdCounters() {
    auto smallestMissing = [](vector<int> nums) -> int {
        sort(nums.begin(), nums.end());
        int expected = 1;
        for (int n : nums) {
            if (n == expected) {
                expected++;
            } else if (n > expected) {
                break;
            }
        }
        return expected;
    };

    vector<int> patientNums, doctorNums, serviceNums;

    for (const Person* p : users) {
        if (p->getType() == "Patient") {
            string id = p->getId();
            if (id.rfind("PAT", 0) == 0) {
                try {
                    patientNums.push_back(stoi(id.substr(3)));
                } catch (...) { /* ignore malformed ID */ }
            }
        } else if (p->getType() == "Doctor") {
            string id = p->getId();
            if (id.rfind("DOC", 0) == 0) {
                try {
                    doctorNums.push_back(stoi(id.substr(3)));
                } catch (...) { /* ignore malformed ID */ }
            }
        }
    }

    for (const auto& s : serviceCatalog) {
        string code = s.getServiceCode();
        if (code.rfind("SVC", 0) == 0) {
            try {
                serviceNums.push_back(stoi(code.substr(3)));
            } catch (...) { /* ignore malformed code */ }
        }
    }

    Patient::setNextPatientNumber(smallestMissing(patientNums));
    Doctor::setNextDoctorNumber(smallestMissing(doctorNums));
    MedicalService::setNextServiceNumber(smallestMissing(serviceNums));
}

void HospitalSystem::menuViewAllPatients() const {
    UIHelper::printSectionHeader("--- All Patient Records ---");

    vector<const Patient*> patients;
    for (const Person* p : users) {
        if (p->getType() == "Patient") {
            patients.push_back(dynamic_cast<const Patient*>(p));
        }
    }

    if (patients.empty()) {
        UIHelper::printWarning("No patients registered in the system.");
        return;
    }

    cout << "  Total Patients: " << patients.size() << endl << endl;

    for (const Patient* pat : patients) {
        pat->displayRole([this](const string& did) { return lookupDoctorName(did); });
        cout << endl;
    }
}

void HospitalSystem::menuViewAllDoctors() const {
    UIHelper::printSectionHeader("--- All Doctor Records ---");

    vector<const Doctor*> doctors;
    for (const Person* p : users) {
        if (p->getType() == "Doctor") {
            doctors.push_back(dynamic_cast<const Doctor*>(p));
        }
    }

    if (doctors.empty()) {
        UIHelper::printWarning("No doctors registered in the system.");
        return;
    }

    cout << "  Total Doctors: " << doctors.size() << endl << endl;

    for (const Doctor* doc : doctors) {
        cout << string(60, '-') << endl;
        cout << " [DOCTOR]  " << doc->getName() << "  (ID: " << doc->getId() << ")" << endl;
        cout << string(60, '-') << endl;
        cout << "  Specialization     : " << doc->getSpecializationDepartment() << endl;
        cout << "  Assigned Clinic    : " << doc->getAssignedClinicCode() << endl;
        cout << "  Contact Number     : " << doc->getContactNumber() << endl;
        cout << "  License Number     : " << doc->getLicenseNumber() << endl;
        cout << "  Assigned Patients  : " << doc->getAssignedPatientCount() << endl;

        const vector<string>& patIds = doc->getAssignedPatientIds();
        if (!patIds.empty()) {
            cout << "  Patient IDs        : ";
            for (size_t i = 0; i < patIds.size(); ++i) {
                if (i > 0) cout << ", ";
                cout << patIds[i];
            }
            cout << endl;
        }
        cout << endl;
    }
}

void HospitalSystem::menuViewAllAppointments() const {
    UIHelper::printSectionHeader("--- All Appointments ---");

    struct ApptEntry {
        string patientId;
        string patientName;
        string appointmentId;
        string serviceCode;
        string serviceName;
        string date;
        string status;
        bool isEmergency;
        string insuranceTier;
        string doctorDisplay;
        double billingAmount;
    };

    vector<ApptEntry> scheduled, completed, cancelled;

    for (const Person* p : users) {
        if (p->getType() != "Patient") continue;
        const Patient* pat = dynamic_cast<const Patient*>(p);
        for (const auto& appt : pat->getAppointments()) {
            ApptEntry e;
            e.patientId     = pat->getId();
            e.patientName   = pat->getName();
            e.appointmentId = appt.getAppointmentId();
            e.serviceCode   = appt.getServiceCode();
            e.serviceName   = appt.getServiceName();
            e.date          = appt.getAppointmentDate();
            e.status        = appt.getAppointmentStatus();
            e.isEmergency   = appt.getIsEmergency();
            e.insuranceTier = AppointmentRecord::tierToString(appt.getInsuranceTier());
            if (appt.getDoctorId().empty()) {
                e.doctorDisplay = "None";
            } else {
                string docName = lookupDoctorName(appt.getDoctorId());
                e.doctorDisplay = appt.getDoctorId() + (docName.empty() ? "" : (" " + docName));
            }
            e.billingAmount = appt.calculateBillingAmount();

            if (e.status == "Scheduled")  scheduled.push_back(e);
            else if (e.status == "Completed") completed.push_back(e);
            else if (e.status == "Cancelled") cancelled.push_back(e);
        }
    }

    auto dateSort = [](const ApptEntry& a, const ApptEntry& b) {
        bool aValid = (a.date != "TBD" && a.date != "N/A");
        bool bValid = (b.date != "TBD" && b.date != "N/A");
        if (aValid && bValid) return a.date > b.date;
        if (aValid) return true;
        return false;
    };
    sort(scheduled.begin(),  scheduled.end(),  dateSort);
    sort(completed.begin(),  completed.end(),  dateSort);
    sort(cancelled.begin(),  cancelled.end(),  dateSort);

    int total = static_cast<int>(scheduled.size() + completed.size() + cancelled.size());
    if (total == 0) {
        UIHelper::printWarning("No appointments found in the system.");
        return;
    }

    cout << "  Total Appointments : " << total
         << "  (Scheduled: " << scheduled.size()
         << " | Completed: " << completed.size()
         << " | Cancelled: " << cancelled.size() << ")" << endl;

    auto printHeader = [&]() {
        cout << "  " << left
             << setw(11) << "Appt ID"
             << setw(11) << "Pat ID"
             << setw(20) << "Patient Name"
             << setw(12) << "Svc Code"
             << setw(24) << "Service Name"
             << setw(13) << "Date"
             << setw(10) << "Emergency"
             << setw(8)  << "Tier"
             << setw(26) << "Doctor"
             << "Bill"
             << endl;
        cout << "  " << string(145, '-') << endl;
    };

    auto printRow = [&](const ApptEntry& e) {
        cout << "  " << left
             << setw(11) << ("[" + e.appointmentId + "]")
             << setw(11) << e.patientId
             << setw(20) << e.patientName
             << setw(12) << e.serviceCode
             << setw(24) << e.serviceName
             << setw(13) << e.date
             << setw(10) << (e.isEmergency ? "YES" : "NO")
             << setw(8)  << e.insuranceTier
             << setw(26) << e.doctorDisplay
             << "$" << fixed << setprecision(2) << e.billingAmount
             << endl;
    };

    cout << endl;
    UIHelper::printDivider('-', 60);
    cout << "  [SCHEDULED]  (" << scheduled.size() << " appointment(s))" << endl;
    UIHelper::printDivider('-', 60);
    if (scheduled.empty()) {
        UIHelper::printInfo("  No scheduled appointments.");
    } else {
        printHeader();
        for (const auto& e : scheduled) printRow(e);
        cout << "  " << string(145, '-') << endl;
    }

    cout << endl;
    UIHelper::printDivider('-', 60);
    cout << "  [COMPLETED]  (" << completed.size() << " appointment(s))" << endl;
    UIHelper::printDivider('-', 60);
    if (completed.empty()) {
        UIHelper::printInfo("  No completed appointments.");
    } else {
        printHeader();
        for (const auto& e : completed) printRow(e);
        cout << "  " << string(145, '-') << endl;
    }

    cout << endl;
    UIHelper::printDivider('-', 60);
    cout << "  [CANCELLED]  (" << cancelled.size() << " appointment(s))" << endl;
    UIHelper::printDivider('-', 60);
    if (cancelled.empty()) {
        UIHelper::printInfo("  No cancelled appointments.");
    } else {
        printHeader();
        for (const auto& e : cancelled) printRow(e);
        cout << "  " << string(145, '-') << endl;
    }
}

void HospitalSystem::run() {
    FileManager::logActivity(dataDirectory + "/activity.log", "System started. Session #" + to_string(systemSessionCount));

    while (true) {
        UIHelper::clearScreen();
        displayMainMenu();

        int choice = -1;
        try {
            choice = UIHelper::getIntInput("  Enter choice: ", 0, 15);
        } catch (...) {
            UIHelper::printError("Invalid menu choice.");
            UIHelper::pressEnterToContinue();
            continue;
        }

        if (choice == 0) {
            UIHelper::clearScreen();
            UIHelper::printDivider('=', 60);
            UIHelper::printCenteredTitle("Thank you for using Hospital Management System", 60);
            UIHelper::printCenteredTitle("All data has been saved. Goodbye!", 60);
            UIHelper::printDivider('=', 60);
            cout << endl;
            FileManager::logActivity(dataDirectory + "/activity.log", "System exited normally.");
            break;
        }

        cout << endl;
        try {
            switch (choice) {
                case 1:  menuAddPatient();             break;
                case 2:  menuAddDoctor();              break;
                case 3:  menuAddMedicalService();      break;
                case 4:  menuScheduleAppointment();    break;
                case 5:  menuSetAppointmentStatus();   break;
                case 6:  menuViewAllUsers();           break;
                case 7:  menuViewServiceCatalog();     break;
                case 8:  menuSearchUser();             break;
                case 9:  menuViewPatientBill();        break;
                case 10: menuSystemStatistics();       break;
                case 11: menuEditRecords();            break;
                case 12: menuDeleteRecords();          break;
                case 13: menuViewAllPatients();        break;
                case 14: menuViewAllDoctors();         break;
                case 15: menuViewAllAppointments();    break;
                default: UIHelper::printWarning("Invalid option."); break;
            }
        } catch (const invalid_argument& e) {
            cout << endl;
            UIHelper::printError("Input Error: " + string(e.what()));
        } catch (const runtime_error& e) {
            cout << endl;
            UIHelper::printError("Runtime Error: " + string(e.what()));
        } catch (const out_of_range& e) {
            cout << endl;
            UIHelper::printError("Out of Range Error: " + string(e.what()));
        } catch (const exception& e) {
            cout << endl;
            UIHelper::printError("Unexpected Error: " + string(e.what()));
        }

        UIHelper::pressEnterToContinue();
    }
}