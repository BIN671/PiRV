#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>

class Person {
protected:
    std::string person_name;

public:
    explicit Person(const std::string& name);
    std::string getter();
    virtual void print();
};

#endif
