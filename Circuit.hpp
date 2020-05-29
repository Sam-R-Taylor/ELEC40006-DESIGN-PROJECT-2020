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
        //add the components to the nodes it is attached to
        Nodes[components[i]->get_cathode()].components_attached.push_back(components[i]);
        Nodes[components[i]->get_anode()].components_attached.push_back(components[i]);
    }
    return Nodes;
}




class Circuit
{
protected:
    std::vector<Node> nodes;
    std::vector<Component*> components;
    std::vector<double> voltages;
    
public:
    //diodes constants
    double GMIN = pow(10,-12);
    double ABSTOL = pow(10,-12);
    double RELTOL = pow(10,-3);
    size_t max_iterations = 1000;           //edit after testing
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
            std::cerr << "node number "<< i.index << std::endl;
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
    }
    int get_number_of_nodes() const{
        return nodes.size();
    }
    /*
    after a circuit object is constructed by the parser
    and build_nodes() has been called
    
    adds connection_resistors to Diodes
    */
    void add_connection_resistors()
    {
        for(Component* i: components)
        {
            if(dynamic_cast<Diode*>(i))
            {   
                Diode* Dptr = dynamic_cast<Diode*>(i);
                int extra_node_index = this->get_number_of_nodes();
                Node* insertion_node = &nodes[Dptr->get_cathode()];
                
                //add Resistor
                Resistor* Rptr = new Resistor(extra_node_index, Dptr->get_cathode(), "R_connection", Dptr->get_rs());
                insertion_node->components_attached.push_back(Rptr);
                
                //remove Diode      
                for (int j = 0; j < insertion_node->components_attached.size(); j++)
                {
                    if(dynamic_cast<Diode*>(insertion_node->components_attached[j]))
                    {
                        (insertion_node->components_attached).erase((insertion_node->components_attached).begin() + j);
                    }
                }
                
                //modify Diode
                Dptr->set_cathode(extra_node_index);

                //add extra_node connecting Diode and Resistor
                Node extra_node;
                extra_node.index = extra_node_index;
                extra_node.components_attached = {Dptr, Rptr};
                nodes.push_back(extra_node);
            }
        }
    }
    std::vector<Node> *get_nodes_ptr() {return &nodes;}
    std::vector<Node> get_nodes() const{return nodes;}
    std::vector<Component*> get_components() const{return components;}
    std::vector<double> get_voltages() const{return voltages;}
};



#endif