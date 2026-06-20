#include "HospitalSystem.h"
#include "UIHelper.h"
#include "FileManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdexcept>

using namespace std;

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