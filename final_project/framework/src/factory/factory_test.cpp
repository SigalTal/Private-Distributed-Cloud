/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: factory_test.cpp
 * ******************************/

#include <iostream>
#include "factory.hpp"

using namespace ilrd;

class Animal
{
public:
    virtual void WhoAmI() = 0;
};

class Dog : public Animal
{
public:
    Dog(int ID){std::cout << "Huw Huw My ID: " << ID<<std::endl;}
    void WhoAmI(){std::cout << "Im A dog dont u get it";}
};

class Cat : public Animal
{
public:
    Cat(int ID){std::cout << "Mew Mew ID: " << ID<<std::endl;}
    void WhoAmI(){std::cout << "Im A Cat dont u get it";}
};

int main(void)
{
    Factory<Animal, int, int> factory;

    factory.Add<Cat>(0);
    factory.Add<Dog>(1);

    std::shared_ptr<Animal> cat = factory.Create(0, 1);
    std::shared_ptr<Animal> dog = factory.Create(1, 1);

    return 0;
}

