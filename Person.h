#pragma once
#include <string>

using namespace std;

class Person {
protected:
    string name;
    string id;

public:
    Person();
    Person(string name, string id);
    virtual ~Person();

    string getName() const;
    string getId() const;

    void setName(string n);
    void setId(string i);

    virtual void displayRole() const = 0;
    virtual string getType() const = 0;
};
