#include "Person.hpp"
#include <iostream>

Person::Person(const std::string& name) : person_name(name) {}

std::string Person::getter() {
    return person_name;
}

void Person::print() {}