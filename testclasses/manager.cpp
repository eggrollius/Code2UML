#include "manager.h"

Manager::Manager(const std::string& name, int id)
    : Employee(name, id) {}

void Manager::addTeamMember(Employee* e) {
    team.push_back(e);
}
