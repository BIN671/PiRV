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
    explicit Person(const string& name) : person_name(name){}

    string getter() {
        return person_name;
    }

    virtual void print(){}
};


class Teacher : public Person {
public:
    explicit Teacher(const string& name) : Person(name){}

    void print() override {
        cout << "Teacher: " << getter() << endl;
    }
};


class Student : public Person {
private:
    std::vector<double> grades;

public:

    explicit Student(const string& name) : Person(name){}

    void addGrade(double numb) {
        grades.push_back(numb);
    }

    void setter_grades() {
        int n = grades.size();
        for (int i = 0; i < n; i++) {
            cout << grades[i] << endl;
        }
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

    void print() override {
        cout << "Student: " << getter() << ", grades count: "
            << grades.size() << ", average: " << middle_grades() << endl;
    }
};

int main()
{
    vector<Person*> people;
    Student* s1 = new Student("Kirill");
    Teacher* t1 = new Teacher("Pavel");

    s1->addGrade(2.2);
    s1->addGrade(4.3);
    s1->addGrade(4.7);

    people.push_back(s1); 
    people.push_back(t1);

    for (Person* p : people) {
        p->print();
    }

    cout << s1->middle_grades();
}
