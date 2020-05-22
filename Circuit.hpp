#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP
#include <vector>
#include "Component.hpp"
using namespace std;
struct Node{
        int index = 0;
        vector<Component*> components;
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
    vector<Node> nodes;
    vector<Component*> components;
    vector<double> voltages;
public:
    void set_voltages(vector<double> _voltages){
        voltages = _voltages;
    }
    vector<double> get_voltages(){
        return voltages;
    }
    void add_component(Component* _component)
    {
        components.push_back(_component);
    }
    void add_node(Node _node)
    {
        nodes.push_back(_node);
    }
    Node get_node(int index) const{
        return nodes[index];
    }
    int get_number_of_nodes() const{
        return nodes.size();
    }
    vector<Node>* get_nodes(){
        return &nodes;
    }
    Node* get_node(int index){
        return &nodes[index];
    }
    vector<Component*> get_components(){
        return components;
    }
};

//takes a vector of objects (should be components)
//outputs a vector of objects (should be nodes)
//each node has a correct vector of components
vector<Node> NodeGenerator(vector<Component*> components){
    vector<Node> Nodes;
    //iterate through the components
    int index = 0;
    for(Component* component: components){ 
        //ensure the node list has a node of high enough index 
        while(Nodes.size()<=component->get_cathode()){
            Node node;
            node.index = (index);
            Nodes.push_back(node);
            index++;
        }
        while(Nodes.size()<=component->get_anode()){
            Node node;
            node.index = index;
            Nodes.push_back(node);
            index++;
        }
        //add the components to the nodes it is attached to
        Nodes[component->get_cathode()].components.push_back(component);
        Nodes[component->get_anode()].components.push_back(component);
    }
    return Nodes;
}

#endif