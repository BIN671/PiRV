#include "Teacher.hpp"
#include <iostream>

Teacher::Teacher(const std::string& name) : Person(name) {}

void Teacher::print() {
    std::cout << "Teacher: " << getter() << std::endl;
}