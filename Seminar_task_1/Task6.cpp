// StudentsGrades5.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Student {
private:
    string name;
    std::vector<double> grades;

public:
    explicit Student(string name) : name(name){}

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
};

int main()
{

    Student s1("Kirill");

    s1.addGrade(2.2);
    s1.addGrade(4.3);
    s1.addGrade(4.7);

    s1.setter_grades();

    cout << s1.middle_grades();
}
