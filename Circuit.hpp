#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP


#include<vector>
#include "Component.hpp"


struct Node{
    int index = 0;
    std::vector<Component*> components_attached;
          
};



std::vector<Node> NodeGenerator(std::vector<Component*> components){
    std::vector<Node> Nodes;
    //iterate through the components
    int index = 0;
    for(int i=0; i < components.size(); i++){ 
        //ensure the node list has a node of high enough index 
        while(Nodes.size()<=components[i]->get_cathode()){
            Node node;
            node.index = index;
            Nodes.push_back(node);
            index++;
        }
        while(Nodes.size()<=components[i]->get_anode()){
            Node node;
            node.index = index;
            Nodes.push_back(node);
            index++;
        }
<<<<<<< HEAD
        */
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
=======
        //add the components to the nodes it is attached to
        Nodes[components[i]->get_cathode()].components_attached.push_back(components[i]);
        Nodes[components[i]->get_anode()].components_attached.push_back(components[i]);
>>>>>>> 254ae12480bc0d49f7c3ab236daa4f8c4b92047d
    }
    return Nodes;
}

<<<<<<< HEAD
=======



>>>>>>> 254ae12480bc0d49f7c3ab236daa4f8c4b92047d
class Circuit
{
protected:
    std::vector<Node> nodes;
    std::vector<Component*> components;
    std::vector<double> voltages;
public:
<<<<<<< HEAD
    std::vector<double> get_voltages() const{
        return voltages;
    }    
    std::vector<Node> get_nodes() const{
        return nodes;
    }
    std::vector<Component> get_components() const{
        return components;
=======
    Circuit()
    {
        std::cerr<<"Circuit constructed"<<std::endl;
    }
    ~Circuit()
    {
        for(Component* i:components)
        {
            delete i;
            std::cerr<<"Component destructed"<<std::endl;
        }
        std::cerr<<"Circuit destructed"<<std::endl;
    }
    void set_voltages(std::vector<double> _voltages){
        voltages = _voltages;
    }
    void add_component(Component* _component)
    {
        components.push_back(_component);
    }
    void print_components() const
    {
        for(Component* i: components){
            std::cerr << i->get_name()<< std::endl;
        }

    }
    void print_node_components() const
    {
        for(Node i: nodes)
        {
            for(Component* j: i.components_attached)
            {
                std::cerr << j->get_name()<< std::endl;
            }
        }
    }
    /*
    after a circuit object is constructed by the parser
    updates vector<Node> nodes
    */
    void build_nodes()
    {
        nodes = NodeGenerator(components);
>>>>>>> 254ae12480bc0d49f7c3ab236daa4f8c4b92047d
    }
    int get_number_of_nodes() const{
        return nodes.size();
    }
<<<<<<< HEAD
    void build_components(const std::vector<Component> _components){components = _components;}
    void build_nodes(const std::vector<Node> _nodes){nodes = _nodes;}
    void set_voltages(std::vector<double> _voltages){voltages = _voltages;}
=======
    std::vector<Node> get_nodes() const{return nodes;}
    std::vector<Component*> get_components() const{return components;}
    std::vector<double> get_voltages() const{return voltages;}
>>>>>>> 254ae12480bc0d49f7c3ab236daa4f8c4b92047d
};



#endif