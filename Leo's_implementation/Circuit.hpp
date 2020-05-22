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
    int number_of_nodes() const
    {
        int tmp = 0;
        for(Component i: components)
            {
                if(i.get_anode() > tmp){tmp = i.get_anode();}
                if(i.get_cathode() > tmp){tmp = i.get_cathode();}
            }
        return tmp;
    }
    int biggest_node_index() const{return this->number_of_nodes()+1;}
    /*
    creates an alternative representation of circuit. 
    Every node (outer vector) has a vector of references to components attached to it.
    */
    std::vector<std::vector<Component&>> node_circuit_generator() const //might need optimization to remove node 0
    {
        std::vector<std::vector<Component&>> tmp(this->number_of_nodes());
        for(Component i: components)
            {
                tmp[i.get_anode()].push_back(&i);
                tmp[i.get_anode()].push_back(&i);
            }
        return tmp;
    }

};





#endif