#ifndef STUDENT_HPP
#define STUDENT_HPP

#include "Person.hpp"
#include "RecordBook.hpp"

class Student : public Person {
private:
    RecordBook book;

public:
    explicit Student(const std::string& name, int id);
    void addGrade(double numb);
    void setter_grades();
    double middle_grades();
    void print() override;
};

#endif
