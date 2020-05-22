#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP


#include<vector>
#include"Component.hpp"

struct Node{
        int index = 0;
        vector<Component&> components;
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
    std::vector<Nodes&> nodes;
    std::vector<Component&> components;
    std::vector<double> voltages;
public:
    void set_voltages(vector<double> _voltages){
        voltages = _voltages;
    }
    void add_component(const Component& _component)
    {
        components.push_back(&_component);
    }
    void add_node(const Node& _node)
    {
        components.push_back(&_node);
    }
    Node& get_node(int index) const{
        return nodes[index];
    }
    int get_size() const{
        return nodes.size();
    }
};

vector<Node> NodeGenerator(vector<Component*> components){
    vector<Node> Nodes;
    //iterate through the components
    int index = 0;
    for(Component* component: components){ 
        //ensure the node list has a node of high enough index 
        while(Nodes.size()<=component->get_cathode()){
            Node node;
            node.set_index(index);
            Nodes.push_back(node);
            index++;
        }
        while(Nodes.size()<=component->get_anode()){
            Node node;
            node.set_index(index);
            Nodes.push_back(node);
            index++;
        }
        //add the components to the nodes it is attached to
        Nodes[component->get_cathode()].add_component(component);
        Nodes[component->get_anode()].add_component(component);
    }
    return Nodes;
}

#endif