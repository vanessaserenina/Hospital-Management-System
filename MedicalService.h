#pragma once
#include <string>

using namespace std;

class MedicalService {
private:
    string serviceCode;
    string serviceName;
    double baseFee;

    static int serviceCount;
    static int nextServiceNumber;

public:
    MedicalService();
    MedicalService(string code, string name);
    MedicalService(string code, string name, double fee);

    string getServiceCode() const;
    string getServiceName() const;
    double getBaseFee() const;

    void setServiceCode(string code);
    void setServiceName(string name);
    void setBaseFee(double fee);

    void displayServiceDetails() const;

    static int getServiceCount();
    static string generateServiceCode();
    static void resetCounter();
    static void setNextServiceNumber(int next);
};