#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP


#include<vector>
#include "Component.hpp"


struct Node{
        int index = 0;
        std::vector<Component> components_attached;
        /*
        void set_index(int _index){index = _index;}
        int get_index(){return index;}
        void add_component(Component* component){
            components.push_back(component);
        }
        vector<Component*> get_components(){
            return components;
        }
        */
};
class Circuit
{
protected:
    std::vector<Node> nodes;
    std::vector<Component> components;
    std::vector<double> voltages;
public:
    void set_voltages(std::vector<double> _voltages){
        voltages = _voltages;
    }
    std::vector<double> get_voltages() const{
        return voltages;
    }
    void add_component(const Component& _component)
    {
        components.push_back(_component);
    }
    void add_node(const Node& _node)
    {
        nodes.push_back(_node);
    }
    Node get_node(int index) const{
        return nodes[index];
    }
    int get_number_of_nodes() const{
        return nodes.size();
    }
    std::vector<Node> get_nodes() const{
        return nodes;
    }
    std::vector<Component> get_components() const{
        return components;
    }
};



#endif