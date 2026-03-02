#include "RecordBook.hpp"

RecordBook::RecordBook(int id) : id(id) {}

std::vector<double>& RecordBook::get_grades() {
    return grades;
}

double RecordBook::middle_grades() {
    double sum = 0;
    if (grades.empty())
        return 0;
    for (int i = 0; i < grades.size(); i++) {
        sum += grades[i];
    }
    return sum / grades.size();
}