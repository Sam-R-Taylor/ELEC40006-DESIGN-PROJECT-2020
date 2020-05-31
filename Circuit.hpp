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
            Node* Collector = &nodes[i->get_diode_BC()->get_cathode()];
            Node* Base = &nodes[i->get_diode_BC()->get_anode()];
            Node* Emitter = &nodes[i->get_diode_EB()->get_cathode()];

            //add connection resistor RB
            int extra_node_base_index = this->get_number_of_nodes();
            Resistor* RB = new Resistor(extra_node_base_index, i->get_diode_BC()->get_anode(), "R_connection_base", i->get_RB());
            Base->components_attached.push_back(RB);
            i->get_diode_EB()->set_anode(extra_node_base_index);
            i->get_diode_BC()->set_anode(extra_node_base_index);

            //add extra_node connecting Diodes and RB
            Node extra_node_base;
            extra_node_base.index = extra_node_base_index;
            extra_node_base.components_attached = {i->get_diode_BC(),i->get_diode_EB(), RB};
            nodes.push_back(extra_node_base);

            
            //add connection resistor RC
            int extra_node_collector_index = this->get_number_of_nodes();
            Resistor* RC = new Resistor(extra_node_collector_index, i->get_diode_BC()->get_cathode(), "R_connection_collector", i->get_RC());
            Collector->components_attached.push_back(RB);
            i->get_diode_BC()->set_cathode(extra_node_collector_index);

            //add extra_node connecting Diode and RC
            Node extra_node_collector;
            extra_node_collector.index = extra_node_collector_index;
            extra_node_collector.components_attached = {i->get_diode_BC(),RC};
            nodes.push_back(extra_node_collector);

            //add connection resistor RE
            int extra_node_emitter_index = this->get_number_of_nodes();
            Resistor* RE = new Resistor(extra_node_emitter_index, i->get_diode_EB()->get_cathode(), "R_connection_emitter", i->get_RE());
            Emitter->components_attached.push_back(RE);
            i->get_diode_EB()->set_cathode(extra_node_emitter_index);

            //add extra_node connecting Diode and RE
            Node extra_node_emitter;
            extra_node_emitter.index=extra_node_emitter_index;
            extra_node_emitter.components_attached ={i->get_diode_EB(),RE};
            nodes.push_back(extra_node_emitter);


            //remove diodes from Base, Collector and Emitter nodes
            std::vector<Component*>::iterator it; 
            it = std::find(Collector->components_attached.begin(),Collector->components_attached.end(),i->get_diode_BC());
            Collector->components_attached.erase(it);
            it = std::find(Emitter->components_attached.begin(),Emitter->components_attached.end(),i->get_diode_EB());
            Emitter->components_attached.erase(it);

            it = std::find(Base->components_attached.begin(),Base->components_attached.end(),i->get_diode_EB());
            Base->components_attached.erase(it);
            it = std::find(Base->components_attached.begin(),Base->components_attached.end(),i->get_diode_BC());
            Base->components_attached.erase(it);
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
            if(dynamic_cast<Diode*>(i) && i->get_name()[0] == 'd')
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
                    if(dynamic_cast<Diode*>(insertion_node->components_attached[j]) && insertion_node->components_attached[j]->get_name()[0] == 'd')
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
    adds connection resistors for Diodes
    addd connection resistors for BJTs
    */
    void build_nodes()
    {
        nodes = NodeGenerator(components);
        this->add_connection_resistors_diodes();
    }
    void add_BJT(BJT BJT_component)
    {
        int collector = BJT_component.get_collector();
        int base = BJT_component.get_base();
        int emitter = BJT_component.get_emitter();
        double RE = BJT_component.get_RE();
        double RC = BJT_component.get_RC();
        

        //add diodes
        Diode* _diode_EB = new Diode(base,emitter,"BJT_diode_EB",RE);
        this->add_component(_diode_EB);
        Diode* _diode_BC = new Diode(base,collector,"BJT_diode_BC",RC);
        this->add_component(_diode_BC);

        //add custom current source
        BJT_current_source* custom_source = new BJT_current_source(_diode_EB,_diode_BC,"custom_source");
        this->add_BJT_current_source(custom_source);
        this->add_component(custom_source);
        BJT_current_sources.push_back(custom_source);
    }
    std::vector<Node> *get_nodes_ptr() {return &nodes;}
    std::vector<Node> get_nodes() const{return nodes;}
    std::vector<Component*> get_components() const{return components;}
    std::vector<double> get_voltages() const{return voltages;}
};



#endif