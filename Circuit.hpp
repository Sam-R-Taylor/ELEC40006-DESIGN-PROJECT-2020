#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP


#include<vector>
#include"Component.hpp"

class Circuit
{
protected:
    std::vector<Component> components;
public:
    Circuit();
    virtual ~Circuit();
};


Circuit::Circuit()      //might take a vector<string> as a parameter ?
{
}

Circuit::~Circuit()
{
}


#endif