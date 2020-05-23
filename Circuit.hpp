#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP


#include<vector>
#include "Component.hpp"


struct Node{
    int index = 0;
    std::vector<Component*> components_attached;
    
    void set_index(int _index){index = _index;}
    int get_index(){return index;}
    void add_component(Component* component){
        components_attached.push_back(component);
    }
    std::vector<Component*> get_components(){
        return components_attached;
    }       
};



std::vector<Node> NodeGenerator(std::vector<Component> components){
    std::vector<Node> Nodes;
    //iterate through the components
    int index = 0;
    for(Component component: components){ 
        //ensure the node list has a node of high enough index 
        while(Nodes.size()<=component.get_cathode()){
            Node node;
            node.set_index(index);
            Nodes.push_back(node);
            index++;
        }
        while(Nodes.size()<=component.get_anode()){
            Node node;
            node.set_index(index);
            Nodes.push_back(node);
            index++;
        }
        //add the components to the nodes it is attached to
        Nodes[component.get_cathode()].add_component(&component);
        Nodes[component.get_anode()].add_component(&component);
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
    void set_voltages(std::vector<double> _voltages){
        voltages = _voltages;
    }
    void add_component(const Component& _component)
    {
        components.push_back(_component);
    }

    /*
    after a circuit object is constructed by the parser
    updates vector<Node> nodes
    */
    void build_nodes()
    {
        nodes = NodeGenerator(components);
    }
    int get_number_of_nodes() const{
        return nodes.size();
    }
    std::vector<Node> get_nodes() const{return nodes;}
    std::vector<Component> get_components() const{return components;}
    std::vector<double> get_voltages() const{return voltages;}
};



#endif