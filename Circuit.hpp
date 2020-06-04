#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP


#include<vector>
#include<algorithm>

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
    std::vector<BJT_current_source*> BJT_current_sources;

    
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
    void print_specific_node_components(int _index)
    {
        std::cerr << "node number "<<_index << std::endl;
        std::cerr << "number of components attached is "<< nodes[_index].components_attached.size()<< std::endl;
        for(Component* i: nodes[_index].components_attached)
        {
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
    int get_number_of_nodes() const{
        return nodes.size();
    }
    void add_BJT_current_source(BJT_current_source* BJT_current_source)
    {
        BJT_current_sources.push_back(BJT_current_source);
    }
    /*
    after a circuit object is constructed by the parser
    and NodeGenerator has created the nodes
    
    adds connection_resistors to Diodes
    */
   void add_connection_resistors_BJTs()
   {
       for(BJT_current_source* i: BJT_current_sources)
       {    
            int Collector = i->get_diode_BC()->get_cathode();
            int Base = i->get_diode_BC()->get_anode();
            int Emitter = i->get_diode_EB()->get_cathode();

            //std::cout<<"print collector 1"<< std::endl;
            //print_specific_node_components(Collector);

            //add connection resistor RB
            //std::cout<<"add connection resistor RB"<< std::endl;
            int extra_node_base_index = this->get_number_of_nodes();
            Resistor* RB = new Resistor(extra_node_base_index, i->get_diode_BC()->get_anode(), "R_connection_base", i->get_RB());
            this->add_component(RB);

            nodes[Base].components_attached.push_back(RB);
            i->get_diode_EB()->set_anode(extra_node_base_index);
            i->get_diode_BC()->set_anode(extra_node_base_index);

            //add extra_node connecting Diodes and RB
            //std::cout<<"add extra_node connecting Diodes and RB"<< std::endl;
            Node extra_node_base;
            extra_node_base.index = extra_node_base_index;
            extra_node_base.components_attached = {i->get_diode_BC(),i->get_diode_EB(), RB};
            nodes.push_back(extra_node_base);

            
            //add connection resistor RC
            //std::cout<<"add connection resistor RC"<< std::endl;
            int extra_node_collector_index = this->get_number_of_nodes();
            Resistor* RC = new Resistor(extra_node_collector_index, i->get_diode_BC()->get_cathode(), "R_connection_collector", i->get_RC());
            this->add_component(RC);
            nodes[Collector].components_attached.push_back(RC);
            i->get_diode_BC()->set_cathode(extra_node_collector_index);

            //std::cout<<"print collector 2"<< std::endl;
            //print_specific_node_components(Collector);

            //add extra_node connecting Diode and RC
            //std::cout<<"add extra_node connecting Diode and RC"<< std::endl;
            Node extra_node_collector;
            extra_node_collector.index = extra_node_collector_index;
            extra_node_collector.components_attached = {i->get_diode_BC(),RC,i}; 
            nodes.push_back(extra_node_collector);

            //std::cout<<"print extra node collector "<< std::endl;
            //print_specific_node_components(extra_node_collector_index);

            //std::cout<<"print collector 3"<< std::endl;
            //print_specific_node_components(Collector);

            //add connection resistor RE
            //std::cout<<"add connection resistor RE"<< std::endl;
            int extra_node_emitter_index = this->get_number_of_nodes();
            Resistor* RE = new Resistor(extra_node_emitter_index, i->get_diode_EB()->get_cathode(), "R_connection_emitter", i->get_RE());
            this->add_component(RE);
            nodes[Emitter].components_attached.push_back(RE);
            i->get_diode_EB()->set_cathode(extra_node_emitter_index);

            //add extra_node connecting Diode and RE
            //std::cout<<"add extra_node connecting Diode and RE"<< std::endl;
            Node extra_node_emitter;
            extra_node_emitter.index=extra_node_emitter_index;
            extra_node_emitter.components_attached ={i->get_diode_EB(),RE,i};
            nodes.push_back(extra_node_emitter);

            //add custom current source
            i->set_anode(extra_node_collector_index);
            i->set_cathode(extra_node_emitter_index);



            //std::cout<<"print collector"<< std::endl;
            //print_specific_node_components(Collector);
            

            //remove diodes from Base, Collector and Emitter nodes
            //std::cout<<"remove diodes from Base, Collector and Emitter nodes"<< std::endl;
            std::vector<Component*>::iterator it; 
            it = std::find(nodes[Collector].components_attached.begin(),nodes[Collector].components_attached.end(),i->get_diode_BC());
            //if(it ==nodes[Collector].components_attached.end()) std::cout<< "can't find diode"<<std::endl;
            nodes[Collector].components_attached.erase(it);
            it = std::find(nodes[Emitter].components_attached.begin(),nodes[Emitter].components_attached.end(),i->get_diode_EB());
            nodes[Emitter].components_attached.erase(it);

            it = std::find(nodes[Base].components_attached.begin(),nodes[Base].components_attached.end(),i->get_diode_EB());
            nodes[Base].components_attached.erase(it);
            it = std::find(nodes[Base].components_attached.begin(),nodes[Base].components_attached.end(),i->get_diode_BC());
            nodes[Base].components_attached.erase(it);

            //remove custom current source from node 0 (guaranted never to be attached to GND)
            it = std::find(nodes[0].components_attached.begin(),nodes[0].components_attached.end(),i);
            nodes[0].components_attached.erase(it);
            nodes[0].components_attached.erase(it);
       }
   }
    /*
    after a circuit object is constructed by the parser
    and NodeGenerator has created the nodes
    
    adds connection_resistors to Diodes
    */
    void add_connection_resistors_diodes()
    {
        for(Component* i: components)
        {
            if(dynamic_cast<Diode*>(i) && i->get_name()[0] == 'D')
            {   
                Diode* Dptr = dynamic_cast<Diode*>(i);
                int extra_node_index = this->get_number_of_nodes();
                Node* insertion_node = &nodes[Dptr->get_cathode()];
                
                //add Resistor
                Resistor* Rptr = new Resistor(extra_node_index, Dptr->get_cathode(), "R_connection_diode", Dptr->get_rs());
                insertion_node->components_attached.push_back(Rptr);
                
                //remove Diode      
                for (int j = 0; j < insertion_node->components_attached.size(); j++)
                {
                    if(dynamic_cast<Diode*>(insertion_node->components_attached[j]) && insertion_node->components_attached[j]->get_name()[0] == 'D')
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
    /*
    after a circuit object is constructed by the parser
    
    updates vector<Node> nodes
    initializes vector<double> voltages to 0
    adds connection resistors for Diodes
    addd connection resistors for BJTs
    */
    void build_nodes()
    {
        nodes = NodeGenerator(components);
        //initialize voltages to 0
        vector<double> tmp(nodes.size(),0);
        voltages = tmp;

        //add connection resistors
        //this->add_connection_resistors_diodes();
        this->add_connection_resistors_BJTs();
    }
    void add_BJT(BJT BJT_component)
    {
        int collector = BJT_component.get_collector();
        int base = BJT_component.get_base();
        int emitter = BJT_component.get_emitter();
        double RE = BJT_component.get_RE();
        double RC = BJT_component.get_RC();
        double BF = BJT_component.get_BF();
        double BR = BJT_component.get_BR();
    
        

        //add diodes
        Diode* _diode_EB = new Diode(base,emitter,"BJT_diode_EB",RE,BF);
        this->add_component(_diode_EB);
        Diode* _diode_BC = new Diode(base,collector,"BJT_diode_BC",RC,BR);
        this->add_component(_diode_BC);

        //add custom current source
        BJT_current_source* custom_source = new BJT_current_source(_diode_EB,_diode_BC,"custom_source");
        this->add_BJT_current_source(custom_source);
        this->add_component(custom_source);
    }
    std::vector<Node> *get_nodes_ptr() {return &nodes;}
    std::vector<Node> get_nodes() const{return nodes;}
    std::vector<Component*> get_components() const{return components;}
    std::vector<double> get_voltages() const{return voltages;}
};



#endif