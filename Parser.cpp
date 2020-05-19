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

    //discards whitespace
    while (isspace(src.peek()))
    {
        src.get();
    }
    
    //stores type of component in tmp
    tmp = src.peek();

    if (tmp == 'R')
    {
        std:: string _name;
        int _anode;
        int _cathode; 
        double _value;

        src >> _name >> _anode >> _cathode >> _value;

        _circuit.add_component(Resistor(_anode,_cathode,_name,_value));
    }
    
    src.close();
    
}
