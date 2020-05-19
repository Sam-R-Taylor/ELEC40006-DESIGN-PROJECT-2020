#include"Circuit.hpp"
#include"Component.hpp"
#include<fstream>
#include<cctype>


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

    //discards whitespace
    while (isspace(src.peek()))
    {
        src.get();
    }
    
    //stores type of component in tmp
    tmp = src.peek();

    if (tmp == 'R')             //Resistor added to _circuit
    {    
        src >> _name >> _anode >> _cathode >> _value;
        _circuit.add_component(Resistor(_anode,_cathode,_name,_value));
    }
    else if (tmp == 'L')            //Inductor added to _circuit
    {
        src >> _name >> _anode >> _cathode >> _value;
        _circuit.add_component(Inductor(_anode,_cathode,_name,_value));
    }
    else if (tmp == 'I')            //Current source added to _circuit
    {
        double _current;
        src >> _name >> _anode >> _cathode >> _current;
        _circuit.add_component(Current_source(_anode, _cathode, _name, _current));
    }
    else if (tmp == 'D')            //Diode added to _circuit
    {   
        src >> _name >> _anode >> _cathode;
        _circuit.add_component(Diode(_anode,_cathode,_name));
    }

    src.close();
    
}
