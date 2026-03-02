#include "Group.hpp"

Group::Group(const std::string& name) : group_name(name) {}

void Group::addStudent(Student* s) {
    group.push_back(s);
}

double Group::middle_grades_in_group() {
    double sum = 0;
    if (group.empty())
        return 0;
    for (int i = 0; i < group.size(); i++) {
        sum += group[i]->middle_grades();
    }
    return sum / group.size();
}