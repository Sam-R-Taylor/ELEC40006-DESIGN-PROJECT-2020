#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP


#include<vector>
#include "Component.hpp"


class Circuit
{
protected:
    std::vector<std::vector<Component*>> nodes;
    std::vector<Component> components;
    std::vector<double> voltages;
public:
    void set_voltages(std::vector<double> _voltages){
        voltages = _voltages;
    }
    void add_component(const Component& _component)
    {
        components.push_back(_component);
    }
    void add_node(Component _node_component)
    {
        int anode = _node_component.get_anode();
        int cathode = _node_component.get_cathode();
        if (anode > nodes.size())
        {
            nodes.resize(anode);
        }
        else if (cathode > nodes.size())
        {
            nodes.resize(cathode);
        }
        nodes[anode].push_back(&_node_component);
        nodes[cathode].push_back(&_node_component);
        
    }
    int get_number_of_nodes() const{
        return nodes.size();
    }
    std::vector<std::vector<Component*>> get_nodes() const{return nodes;}
    std::vector<Component> get_components() const{return components;}
    std::vector<double> get_voltages() const{return voltages;}
};



#endif