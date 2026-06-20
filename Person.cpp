#include "Person.h"
#include <stdexcept>

using namespace std;

Person::Person() : name("Unknown"), id("P000") {}

Person::Person(string name, string id) : name(name), id(id) {
    if (name.empty()) throw invalid_argument("Person name cannot be empty.");
    if (id.empty())   throw invalid_argument("Person ID cannot be empty.");
}

Person::~Person() {}

string Person::getName() const { return name; }
string Person::getId() const   { return id; }

void Person::setName(string n) {
    if (n.empty()) throw invalid_argument("Name cannot be empty.");
    name = n;
}

void Person::setId(string i) {
    if (i.empty()) throw invalid_argument("ID cannot be empty.");
    id = i;
}
