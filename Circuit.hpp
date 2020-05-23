#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP


#include<vector>
#include "Component.hpp"


struct Node{
    int index = 0;
    std::vector<Component> components_attached;
};


//helper function for creating Circuit::std::vector<Node> nodes;
std::vector<Node> NodeGenerator(std::vector<Component> components){
    std::vector<Node> Nodes;
    //iterate through the components
    int index = 0;
    for(Component component: components){ 
        //ensure the node list has a node of high enough index 
        while(Nodes.size()<=component.get_cathode()){
            Node node;
            node.index = index;
            Nodes.push_back(node);
            index++;
        }
        while(Nodes.size()<=component.get_anode()){
            Node node;
            node.index = index;
            Nodes.push_back(node);
            index++;
        }
        //add the components to the nodes it is attached to
        Nodes[component.get_cathode()].components_attached.push_back(component);
        Nodes[component.get_anode()].components_attached.push_back(component);
    }
    return Nodes;
}

class Circuit
{
protected:
    std::vector<Node> nodes;
    std::vector<Component> components;
    std::vector<double> voltages;
public:
    std::vector<double> get_voltages() const{
        return voltages;
    }    
    std::vector<Node> get_nodes() const{
        return nodes;
    }
    std::vector<Component> get_components() const{
        return components;
    }
    int get_number_of_nodes() const{
        return nodes.size();
    }
    void build_components(const std::vector<Component> _components){components = _components;}
    void build_nodes(const std::vector<Node> _nodes){nodes = _nodes;}
    void set_voltages(std::vector<double> _voltages){voltages = _voltages;}

    Node get_node(int _index){return nodes[_index];}        //might need improvements
};



#endif