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
    int biggest_node_index() const
    {
        int tmp = 0;
        for(Component i: components)
            {
                if(i.get_anode() > tmp){tmp = i.get_anode();}
                if(i.get_cathode() > tmp){tmp = i.get_cathode();}
            }
        return tmp;
    }
    std::vector<std::vector<Component>> node_generator() const
    {
        std::vector<std::vector<Component>> tmp(this->biggest_node_index(),std::vector<Component>(5));  //might need optimization
    }

};





#endif