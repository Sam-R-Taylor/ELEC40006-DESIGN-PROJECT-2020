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
    int number_of_nodes()
    {
        int tmp = 0;
        for(Component i: components)
            {
                if(i.get_anode() > tmp){tmp = i.get_anode();}
                if (i.get_cathode() > tmp){tmp = i.get_cathode();}
            }
    }

};





#endif