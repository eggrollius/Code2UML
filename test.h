#include <iostream>

class Human {
public:
    Human();
    Human(const std::string& name, const int socialInsurance);
    ~Human();

    // Getters and Setters
    std::string getName() const { return name; }
    void setName(const std::string& name) { this->name = name; }
    int getSocialInsurance() const { return socialInsurance; }
    void setSocialInsurance(const int socialInsurance) { this->socialInsurance = socialInsurance; }

    void speak() const;
private:
    std::string name;
    int socialInsurance;
};