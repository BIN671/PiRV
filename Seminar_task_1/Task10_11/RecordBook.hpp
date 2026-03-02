#ifndef RECORDBOOK_HPP
#define RECORDBOOK_HPP

#include <vector>

class RecordBook {
private:
    int id;
    std::vector<double> grades;

public:
    RecordBook(int id);
    std::vector<double>& get_grades();
    double middle_grades();
};

#endif
