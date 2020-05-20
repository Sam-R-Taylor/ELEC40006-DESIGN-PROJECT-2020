#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP


#include<vector>
#include"Component.hpp"

class Circuit
{
protected:
    std::vector<Component> components;
public:
    void add_component(const Component& _component)
    {
        components.push_back(_component);
    }


};





#endif