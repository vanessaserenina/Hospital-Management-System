HOSPITAL MANAGEMENT SYSTEM - Prawirayasa Vanessa Serenina (AIT2409118)

1. OVERVIEW
----------------------------------------------------------------
This is a Hospital Management System written in C++. It allows staff to manage Patients, Doctors, and Medical Services, schedule and bill appointments, and view system statistics. All data is persisted to plain text files on disk so information can be kept between runs.

Main features:
  - Add / edit / delete Patients, Doctors, and Medical Services
  - Schedule appointments
  - Assign doctors to appointments and patients to doctors
  - Track appointment status
  - Insurance tier billing (NONE / BASIC / PLUS / ELITE) with automatic coverage-rate calculation
  - View all users via runtime polymorphism
  - Search users by ID, view billing summaries
  - System statistics (user counts, session counts, etc.)
  - Activity logging with timestamps
  - Automatic data persistence (load on start, save on exit and after every change)

2. PROJECT STRUCTURE
----------------------------------------------------------------
main.cpp                  Program entry point
HospitalSystem.h/.cpp     Core application controller, menu system, file save/load logic
Person.h/.cpp             Abstract base class (name, id)
Patient.h/.cpp            Patient entity (extends Person)
Doctor.h/.cpp             Doctor entity (extends Person)
MedicalService.h/.cpp     Medical service catalog entry
AppointmentRecord.h/.cpp  Appointment + billing logic
UIHelper.h/.cpp           Console I/O, input validation, formatting utilities
FileManager.h/.cpp        File read/write, logging, directory helpers
hospital_app.exe          Pre-built Windows executable generated at runtime
data/users.dat            Saved patients, doctors, appointments
data/services.dat         Saved medical service catalog
data/activity.log         Timestamped activity log

3. REQUIREMENTS
----------------------------------------------------------------
- No external libraries are required (uses only the C++ standard library).

4. HOW TO RUN (Windows - prebuilt executable)
----------------------------------------------------------------
A ready-to-run executable is included: hospital_app.exe

1. Run from a terminal: 
	./hospital_app.exe
2. The program will automatically create a "data" folder next to the executable on first run to store its data files.

5. HOW TO BUILD FROM SOURCE
----------------------------------------------------------------
1. Compile all .cpp files together from a terminal:
	g++ -std=c++17 -o hospital_app.exe main.cpp HospitalSystem.cpp Person.cpp Patient.cpp Doctor.cpp MedicalService.cpp AppointmentRecord.cpp UIHelper.cpp FileManager.cpp
2. Run app:
  	./hospital_app.exe

6. USING THE APPLICATION
----------------------------------------------------------------
On launch, the Hospital Management System main menu have the following options:
    1.  Add New Patient
    2.  Add New Doctor
    3.  Add New Medical Service
    4.  Schedule Appointment for Patient
    5.  Set Appointment Status / Bill Modifier
    6.  View All Users (Runtime Polymorphism)
    7.  View Medical Services Catalog
    8.  Search User by ID
    9.  View Patient Billing Summary
    10. System Statistics
    11. Edit Records (Patient/Doctor/Service)
    12. Delete Records (Patient/Doctor/Service)
    13. View All Patients
    14. View All Doctors
    15. View All Appointments
    0.  Exit System

Enter the number corresponding to the choice and press Enter. Follow the on-screen prompts to enter details. Where noted, pressing [ENTER] on an edit prompt keeps the current value.

Patient IDs are auto-generated as PAT0001, PAT0002, ...
Doctor IDs are auto-generated as DOC0001, DOC0002, ...
Service codes are auto-generated as SVC001, SVC002, ...
Appointment IDs are auto-generated as APT00001, APT00002, ...

Dates must be entered in YYYY-MM-DD format (ex: 2025-06-21).

7. DATA PERSISTENCE
----------------------------------------------------------------
All data is automatically saved to the "data" folder:
  - data/users.dat     - patients, doctors, and appointments
  - data/services.dat  - medical service catalog
  - data/activity.log  - timestamped log of all actions

Data is loaded automatically when the program starts and saved after every add/edit/delete action, as well as on exit (option 0 or normal program termination). These files should not be edited while running.

8. NOTES
----------------------------------------------------------------
  - Exiting through  option 0 ensures a final save, closing the console window suddenly may also trigger a save through the destructor, but option 0 is the recommended way to exit.
  - Deleting a Patient automatically removes that patient's ID from any Doctor's assigned patient list.
  - Insurance tiers determine coverage rate applied to billing:
      NONE  = 0% covered
      BASIC = 50% covered
      PLUS  = 70% covered
      ELITE = 100% covered
  - Emergency appointments applies a 1.5x fee multiplier before insurance coverage is applied.
