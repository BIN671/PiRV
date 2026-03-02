// StudentsGrades.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
using namespace std;

void midArifm(std::vector<std::vector<double>>* grades, int N, int M) {
    
    for (int i = 0; i < N; i++) {
        double sum = 0;
        for (int j = 0; j < M; j++) {
            sum += grades->at(i).at(j);
        }
        cout << "The " << i+1 << " student's arithmetic average: " << sum / M << endl;
    }
}

double midArifmOnLessons(std::vector<std::vector<double>>* grades, int N, int M, int id) {
    double sum = 0;
    for (int i = 0; i < N; i++) {
        
        for (int j = 0; j < M; j++) {
            if (j == id) {
                sum += grades->at(i).at(j);
                break;
            }
        }
    }
    return sum / N;
}

int MaxMidArifm(std::vector<std::vector<double>>* grades, int N, int M) {
    double mm = 0;
    int ind = 0;
    for (int i = 0; i < N; i++) {
        double sum = 0;
        for (int j = 0; j < M; j++) {
            sum += grades->at(i).at(j);
        }
        if (sum / M > mm) {
            mm = sum / M;
            ind = i;
        }
    }
    return ind+1;
}


int main()
{
    int N = 0;
    int M = 0;
    cout << "Enter number of students: ";
    cin >> N;
    cout << "Enter number of lessons: ";
    cin >> M;

    std::vector<std::vector<double>> grades(N, std::vector<double>(M));

    if (N <= 0) {
        cout << "Not correct number" << endl;
    }
    else {
        for (int i = 0; i < N; i++) {

            for (int j = 0; j < M; j++) {
                double a;
                while (true) {
                    cout << "Enter grade of " << i + 1 << " student and " << j+1 << " lesson: ";
                    cin >> a;
                    if (0 < a && a <= 5)
                        break;
                }

                grades[i][j] = a;
            }
        }
    }
    cout << "Middle arifm:" << endl;
    midArifm(&grades, N, M);

    cout << "Middle arifm on lesson: ";
    cout << midArifmOnLessons(&grades, N, M, 1) << endl;

    cout << "The best student: ";
    cout << MaxMidArifm(&grades, N, M) << endl;

    delete[] &grades;
    
    return 0;
}

