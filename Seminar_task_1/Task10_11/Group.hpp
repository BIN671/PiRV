#ifndef GROUP_HPP
#define GROUP_HPP

#include <string>
#include <vector>
#include "Student.hpp"

class Group {
private:
    std::string group_name;
    std::vector<Student*> group;

public:
    Group(const std::string& name);
    void addStudent(Student* s);
    double middle_grades_in_group();
};

#endif
