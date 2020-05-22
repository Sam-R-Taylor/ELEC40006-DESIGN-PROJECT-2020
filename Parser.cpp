#include"Circuit.hpp"
#include"Component.hpp"
#include<fstream>
#include<cctype>
#include<cmath>


//declaring constants
double femto = pow(10,-15);
double pico = pow(10,-12);
double nano = pow(10,-9);
double micro = pow(10,-6);
double milli = pow(10,-3);
double kilo = pow(10,3);
double mega = pow(10,6);
double giga = pow(10,9);
double tera = pow(10,12);

double mil = 25.4*pow(10,-6);

/*
enables reading of prefixes such as kilo, mega ...
*/
double read_power_of_ten(std::istream& src)
{
    std::string tmp;
    if(src >> tmp)
    {            
        char c = tolower(tmp[0]);

        if(c == 'f'){return femto;}
        else if(c == 'p'){return pico;}
        else if(c == 'n'){return nano;}
        else if(c == 'u'){return micro;}
        else if(c == 'm'){return milli;}
        else if(c == 'k'){return kilo;}
        else if(c == 'm' && tolower(tmp[1]) == 'e' && tolower(tmp[2]) == 'g'){return mega;}
        else if(c == 'g'){return giga;}
        else if(c == 't'){return tera;}
        else if(c == 'm' && tolower(tmp[1]) == 'i' && tolower(tmp[2]) == 'l'){return mil;}
    }
    return 1; 
}






//input file specified as argv[1]
int main(int argc, char const *argv[])
{
    std::fstream src;
    src.open (argv[1], std::fstream::in);

    Circuit _circuit;
    char tmp;

    std:: string _name;
    int _anode;
    int _cathode; 
    double _value;

    while(true){

        //discards whitespace
        while (isspace(src.peek()))
        {
            src.get();
        }
        if (src.fail())
        {
            break;
        }
        


        //stores type of component in tmp
        tmp = src.peek();

        if (tmp == 'R' | tmp == 'r')             //Resistor added to _circuit
        {    
            src >> _name >> _anode >> _cathode >> _value;
            _value *= read_power_of_ten(src);
            _circuit.add_component(Resistor(_anode,_cathode,_name,_value));
        }
        else if (tmp == 'L' | tmp == 'l')            //Inductor added to _circuit
        {
            src >> _name >> _anode >> _cathode >> _value;
            _value *= read_power_of_ten(src);
            _circuit.add_component(Inductor(_anode,_cathode,_name,_value));
        }
        else if (tmp == 'I')            //Current source added to _circuit
        {
            double _current;
            src >> _name >> _anode >> _cathode >> _current;
            _current *= read_power_of_ten(src);
            _circuit.add_component(Current_source(_anode, _cathode, _name, _current));
        }
        else if (tmp == 'D')            //Diode added to _circuit
        {   
            src >> _name >> _anode >> _cathode;
            _circuit.add_component(Diode(_anode,_cathode,_name));
        }
        else if (tmp == 'V')            //Voltage source added to _circuit
        {
            double _voltage;
            src >> _name >> _anode >> _cathode >> _voltage;
            _voltage *= read_power_of_ten(src);
            _circuit.add_component(Voltage_Source(_anode, _cathode, _name, _voltage));
        }
        else if (tmp == 'C' | tmp == 'c')           //Capacitor added to _circuit
        {
            src >> _name >> _anode >> _cathode >> _value;
            _value *= read_power_of_ten(src);
            _circuit.add_component(Capacitor(_anode,_cathode,_name,_value));
        }
        else if (tmp == 'E')            //Voltage_Controlled_Voltage_Source added to _circuit
        {
            double _gain;
            int _control_voltage_anode;
            int _control_voltage_cathode;
            src >> _name >> _anode >> _cathode >> _control_voltage_anode >> _control_voltage_cathode >> _gain;
            _gain *= read_power_of_ten(src);
            _circuit.add_component(Voltage_Controlled_Voltage_Source(_anode, _cathode, _name, _gain, _control_voltage_anode, _control_voltage_cathode));

        }
        else if (tmp == 'G')            //Voltage_Controlled_Current_Source added to _circuit
        {
            double _gain;
            int _control_voltage_anode;
            int _control_voltage_cathode;
            src >> _name >> _anode >> _cathode >> _control_voltage_anode >> _control_voltage_cathode >> _gain;
            _gain *= read_power_of_ten(src);
            _circuit.add_component(Voltage_Controlled_Current_Source(_anode, _cathode, _name, _gain, _control_voltage_anode, _control_voltage_cathode));
        }
        else
        {
            //terminate the program if component is not known
            std::cerr << "unknown component" << std::endl;
            exit(1);
        }
    
    }
    src.close();
    
}


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
 