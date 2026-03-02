#include <iostream>
#include <vector>
#include "Student.hpp"
#include "Teacher.hpp"
#include "Group.hpp"
#include "FileManager.hpp"

int main() {
    std::vector<Person*> people;
    Group iu("IU");

    Student* s1 = new Student("Kirill", 2345);
    Student* s2 = new Student("Foma", 3412);
    Teacher* t1 = new Teacher("Pavel");

    s2->addGrade(3.4);
    s2->addGrade(4.4);
    s2->addGrade(4.8);

    s1->addGrade(2.2);
    s1->addGrade(4.3);
    s1->addGrade(4.7);

    people.push_back(s1);
    people.push_back(s2);
    people.push_back(t1);

    iu.addStudent(s1);
    iu.addStudent(s2);

    std::cout << iu.middle_grades_in_group() << std::endl;

    for (Person* p : people) {
        p->print();
    }

    std::cout << "Middle of grades: " << s1->middle_grades() << std::endl;

    std::vector<double> allGrades;
    allGrades.insert(allGrades.end(), 3.4);
    allGrades.insert(allGrades.end(), 4.4);
    allGrades.insert(allGrades.end(), 4.8);
    allGrades.insert(allGrades.end(), 2.2);
    allGrades.insert(allGrades.end(), 4.3);
    allGrades.insert(allGrades.end(), 4.7);

    std::vector<Student*> students;
    students.push_back(s1);
    students.push_back(s2);

    FileManager::saveToFile("students.dat", students, allGrades);
    FileManager::loadFromFile("students.dat", students, allGrades);

    if (FileManager::checkSignature("students.dat")) {
        std::cout << "File signature is correct" << std::endl;
    }
    else {
        std::cout << "File signature is incorrect" << std::endl;
    }

    delete s1;
    delete s2;
    delete t1;

    return 0;
}