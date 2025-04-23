#include "employee.h"

int Employee::employeeCount = 0;

Employee::Employee(const std::string& name, int id)
    : Person(name), id(id) {
    employeeCount++;
}

int Employee::getId() const {
    return id;
}
