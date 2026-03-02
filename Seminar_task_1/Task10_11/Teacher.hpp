#ifndef TEACHER_HPP
#define TEACHER_HPP

#include "Person.hpp"

class Teacher : public Person {
public:
    explicit Teacher(const std::string& name);
    void print() override;
};

#endif
