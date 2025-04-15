#include <iostream>
#include <vector>
#include <string>

class Human {
public:
    Human();
    Human(const std::string& name, const int socialInsurance);
    ~Human();

    std::string getName() const { return name; }
    void setName(const std::string& name) { this->name = name; }
    int getSocialInsurance() const { return socialInsurance; }
    void setSocialInsurance(const int socialInsurance) { this->socialInsurance = socialInsurance; }

    void speak() const;

private:
    std::string name;
    int socialInsurance;
};

class Animal {
public:
    Animal(const std::string& species);
    virtual ~Animal();

    std::string getSpecies() const;
    void makeSound() const;

protected:
    std::string species;
};

class Dog : public Animal {
public:
    Dog(const std::string& name);
    void wagTail() const;

private:
    std::string name;
};

class University {
public:
    University(const std::string& name);
    void enrollStudent(const Human& student);

private:
    std::string name;
    std::vector<Human> students;
};

class Empty {};

