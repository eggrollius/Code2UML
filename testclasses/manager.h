#ifndef MANAGER_H
#define MANAGER_H

#include "employee.h"
#include <vector>

class Manager : public Employee {
public:
    Manager(const std::string& name, int id);
    void addTeamMember(Employee* e);

private:
    std::vector<Employee*> team;
};

#endif
