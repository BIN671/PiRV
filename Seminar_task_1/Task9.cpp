// StudentsGrades7.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

// StudentsGrades5.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Person {
    string person_name;

public:
    explicit Person(const string& name) : person_name(name) {}

    string getter() {
        return person_name;
    }

    virtual void print() {}
};


class Teacher : public Person {
public:
    explicit Teacher(const string& name) : Person(name) {}

    void print() override {
        cout << "Teacher: " << getter() << endl;
    }
};


class RecordBook {
private:
    int id;
    vector<double> grades;

public:
    RecordBook(int id) : id(id) {}

    vector<double>& get_grades() {
        return grades;
    }

    double middle_grades() {
        double sum = 0;
        if (grades.empty())
            return 0;
        for (int i = 0; i < grades.size(); i++) {
            sum += grades[i];
        }
        return sum / grades.size();
    }

};



class Student : public Person {
private:
    RecordBook book;

public:

    explicit Student(const string& name, int id) : Person(name), book(id) {}

    void addGrade(double numb) {
        book.get_grades().push_back(numb);
    }

    void setter_grades() {
        int n = book.get_grades().size();
        for (int i = 0; i < n; i++) {
            cout << book.get_grades()[i] << endl;
        }
    }

    double middle_grades() {
        return book.middle_grades();
    }

    void print() override {
        cout << "Student: " << getter() << " and grades: " << endl;
        setter_grades();
    }
};

class Group {
private:
    string group_name;
    vector<Student*> group;
public:
    Group(const string& name) : group_name(name) {}

    void addStudent(Student* s) {

        group.push_back(s);
    }

    double middle_grades_in_group() {
        double sum = 0;
        if (group.empty())
            return 0;
        for (int i = 0; i < group.size(); i++) {
            sum += group[i]->middle_grades();
        }
        return sum / group.size();
    }
};

int main()
{
    vector<Person*> people;
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

    cout << iu.middle_grades_in_group() << endl;

    for (Person* p : people) {
        p->print();
    }

    cout << "Middle of grades: " << s1->middle_grades() << endl;
}
