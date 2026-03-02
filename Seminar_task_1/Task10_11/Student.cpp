#include "Student.hpp"
#include <iostream>

Student::Student(const std::string& name, int id) : Person(name), book(id) {}

void Student::addGrade(double numb) {
    book.get_grades().push_back(numb);
}

void Student::setter_grades() {
    int n = book.get_grades().size();
    for (int i = 0; i < n; i++) {
        std::cout << book.get_grades()[i] << std::endl;
    }
}

double Student::middle_grades() {
    return book.middle_grades();
}

void Student::print() {
    std::cout << "Student: " << getter() << " and grades: " << std::endl;
    setter_grades();
}