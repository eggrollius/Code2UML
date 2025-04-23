#ifndef PERSON_H
#define PERSON_H

#include <string>

class Person {
public:
    Person(const std::string& name);
    virtual ~Person();
    std::string getName() const;

protected:
    std::string name;
};

#endif
