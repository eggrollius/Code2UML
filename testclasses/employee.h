#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "person.h"
#include <string>

class Employee : public Person {
public:
    Employee(const std::string& name, int id);
    int getId() const;

private:
    int id;
    static int employeeCount;
};

#endif
