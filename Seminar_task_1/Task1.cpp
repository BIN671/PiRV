// StudentsGrades.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
using namespace std;

double midArifm(double* grad, double N) {
    double sum = 0;
    for (int i = 0; i < N; i++) {
        sum += grad[i];
    }
    return sum / N;
}

double maximum(double* grad, int N) {
    double mm = grad[0];
    for (int i = 0; i < N; i++) {
        if (grad[i] > mm)
            mm = grad[i];
    }
    return mm;
}

double minimum(double* grad, int N) {
    double mn = grad[0];
    for (int i = 0; i < N; i++) {
        if (grad[i] < mn)
            mn = grad[i];
    }
    return mn;
}

double porog(double* grad, int N, double porog) {
    int sum = 0;
    for (int i = 0; i < N; i++) {
        if (porog <= grad[i])
            sum++;
    }
    return sum;
}

int main()
{
    int N = 0;
    cout << "Enter number of students: ";
    cin >> N;

    double* grades = new double[N];

    if (N <= 0) {
        cout << "Not correct number" << endl;
    }
    else {
        for (int i = 0; i < N; i++) {
            
            double a;
            while (true) {
                cout << "Enter grade of " << i + 1 << " student: ";
                cin >> a;
                if(0 < a && a <= 5)
                    break;
            }

            grades[i] = a;
        }
    }
    cout << "Middle arifm:";
    cout << midArifm(grades, N) << endl;

    cout << "Maximu:";
    cout << maximum(grades, N) << endl;

    cout << "Minimu:";
    cout << minimum(grades, N) << endl;
    double por;
    cout << "Porog start with:";
    cin >> por;
    cout << porog(grades, N, por) << endl;

    delete[] grades;
}


