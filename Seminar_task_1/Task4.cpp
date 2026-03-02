// StudentsGrades4.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility> 
using namespace std;

int main()
{
    int N = 0;
    cout << "Enter number of students: ";
    cin >> N;

    std::vector<std::pair<int, double>> grades;

    if (N <= 0) {
        cout << "Not correct number" << endl;
    }
    else {
        for (int i = 0; i < N; i++) {

            double a;
            while (true) {
                cout << "Enter grade of " << i + 1 << " student: ";
                cin >> a;
                if (0 < a && a <= 5)
                    break;
            }

            grades.push_back(std::make_pair(i, a));
        }
    }

    double h;
    cout << "Enter min grade :";
    cin >> h;

    grades.erase(
        std::remove_if(grades.begin(), grades.end(),
            [h](const auto& p) { return p.second < h; }),
        grades.end()
    );

    for (const auto& item : grades) {
        std::cout << "(" << item.first << ", " << item.second << ")\n";
    }

    return 0;

}