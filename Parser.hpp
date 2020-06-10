#ifndef LEO_PARSER_HPP
#define LEO_PARSER_HPP

#include "Circuit.hpp"
#include "Component.hpp"
#include "KCLSolver.hpp"
#include "TransientSolver.hpp"
#include "TransientAnalysis.hpp"
#include "OPAnalysis.hpp"
#include <fstream>
#include <cctype>
#include <memory> 
#include <cmath>
#include <limits>
#include <cassert>
#include <chrono>

//declaring constants
double lfemto = pow(10,-15);
double lpico = pow(10,-12);
double lnano = pow(10,-9);
double lmicro = pow(10,-6);
double lmilli = pow(10,-3);
double lkilo = pow(10,3);
double lmega = pow(10,6);
double lgiga = pow(10,9);
double ltera = pow(10,12);

double lmil = 25.4*pow(10,-6);

//data for read_value
std::string digits("9876543210.");





//helper function
int read_node_number(const std::string& node_name)
{
    char first_char = node_name[0];
    assert(first_char == 'N' || first_char == '0');

    if (first_char == 'N')
    {
        return stoi(node_name.substr(1,std::string::npos));
    }
    else if (first_char == '0')
    {
        return stoi(node_name);
    }
    return 0;
}

//helper function
double read_value(const std::string& value_str)
{
    size_t index = value_str.find_first_not_of(digits);
    if (index == std::string::npos)
    {
        return stod(value_str);
    }
    else
    {
        char c = tolower(value_str[index]);
        double number = stod(value_str.substr(0,index));

        if(c == 'f'){return lfemto*number;}
        else if(c == 'p'){return lpico*number;}
        else if(c == 'n'){return lnano*number;}
        else if(c == 'u'){return lmicro*number;}
        else if(c == 'k'){return lkilo*number;}
        else if(c == 'm' && tolower(value_str[index+1]) == 'e' && tolower(value_str[index+2]) == 'g'){return lmega*number;}
        else if(c == 'g'){return lgiga*number;}
        else if(c == 't'){return ltera*number;}
        else if(c == 'm' && tolower(value_str[index+1]) == 'i' && tolower(value_str[index+2]) == 'l'){return lmil*number;}
        else if(c == 'm'){return lmilli*number;}
        else{return number;}
    }
    std::cerr << "problem in read_value" << std::endl;
    return 0;
}





void parse_input(std::fstream & src)

{
    //initializing variables for Circuit and component objects
    Circuit _circuit;
    std::vector<int> voltage_anodes;
    std::string _name;
    std::string _anode;
    std::string _cathode;
    
    //non-formatted reading
    char tmp;

    //dealing with commands
    std::string command;
    

    while(true)
    {
        //storing first char of the line
        tmp = tolower(src.peek());

        switch (tmp)
        {
        case '*':
            /* removing comments */
            break;
        
        case '.':
            /* reached a command */
            
            src.get();
            src >> command;

            if (command == "end")
            {   
                //close input file
                src.close();

                std::cout << "terminating parser" << std::endl;
                return;
            }
            else if (command == "tran")
            {
                std::string stop_time_str;
                std::string time_step_str;


                src.ignore(std::numeric_limits<std::streamsize>::max(), '0');
                src >> stop_time_str;
                double stop_time = read_value(stop_time_str);
                src.ignore(std::numeric_limits<std::streamsize>::max(), '0'); //IGNORES 0.1
                src >> time_step_str;
                double time_step = read_value(time_step_str);

                //building Circuit obj
                _circuit.build_nodes();
                
                int num_timestep = stop_time/time_step;     //number of timesteps within the simulation
                //NEEDS CONRTOLLER IMPLEMENTATION
                std::cout << "TRANSIENT" << endl ;
                //cout << "stop_time" << stop_time << endl ;
                //cout << "Time Step" << time_step << endl ;
                //cout << "Num Time Step" << num_timestep << endl ;
                OPAnalysis(_circuit);
                TransientAnalysis(_circuit,stop_time,num_timestep);
                std::cout << "Finished analysis" << endl ;

            }
            else if (command == "op")
            {
                std::cout << "OP" << endl ;
                _circuit.build_nodes();
                OPAnalysis(_circuit);
                std::cout << "Finished analysis" << endl ;
            }
            else
            {   
                //do nothing. Skip over a command

                //std::cout << "uknown command" << std::endl;
                //exit(1);
            }
            break;
        

        case 'r':
            {
                //Resistor added to _circuit
                std::string _resistance;
                src >> _name >> _anode >> _cathode >> _resistance;
                _circuit.add_component(new Resistor(read_node_number(_anode),read_node_number(_cathode),_name,read_value(_resistance)));
                std::cout<< "added "<<_name << read_node_number(_anode) << read_node_number(_cathode) <<" " <<read_value(_resistance) << std::endl;
            }
            break;

        case 'i':
            {
                //Current source added to _circuit
                std::string _current;
                src >> _name >> _anode >> _cathode >> _current;
                _circuit.add_component(new Current_source(read_node_number(_anode),read_node_number(_cathode),_name,read_value(_current)));
                std::cout<< "added "<<_name << read_node_number(_anode) << read_node_number(_cathode) << " "<<read_value(_current) << std::endl;
            }   
            break; 
        case 'v': 
            {
                //Voltage source added to _circuit
                std::string _voltage;
                src >> _name >> _anode >> _cathode;
                std::getline(src,_voltage);
                src.putback('\n');
                if(_voltage.substr(1,4)=="SINE")
                {
                    std::string _dc_offset;
                    std::string _amplitude;
                    std::string _frequency;

                    std::stringstream ss(_voltage,ios_base::in);
                    ss.ignore(std::numeric_limits<std::streamsize>::max(), '(');
                    ss >> _dc_offset >> _amplitude >> _frequency;
                    //std::cout <<  _dc_offset << _amplitude << _frequency<< std::endl;
                    //std::cout << "frequency read is " << read_value(_frequency) << std::endl;
                    //std::cout << "dc offset read is " << read_value(_dc_offset)<< std::endl;
                    //std::cout << "amplitude read is "<< read_value(_amplitude)<< std::endl;
                    //necessary as kcl algorithm requires that two voltage sources dont have the same anode
                    int anode = read_node_number(_anode);
                    int cathode = read_node_number(_cathode);
                    double amplitude = read_value(_amplitude);
                    double offset = read_value(_dc_offset);
                    bool swap = false;
                    for(auto x: voltage_anodes){
                        swap = anode == x;
                    }
                    if(swap){
                        int temp;
                        temp = anode;
                        anode = cathode;
                        cathode = temp;
                        amplitude = -1 * amplitude;
                        offset = -1 * offset;
                    }
                    voltage_anodes.push_back(anode);
                    _circuit.add_component(new AC_Voltage_Source(anode,cathode,_name,amplitude,read_value(_frequency),offset));
                    std::cout<< "added "<<_name << read_node_number(_anode) << read_node_number(_cathode) <<" A " <<read_value(_amplitude) << " f " << read_value(_frequency) << " offset " << read_value(_dc_offset)<< std::endl;

                }
                else
                {
                    size_t index = _voltage.find_first_of(digits);
                    _voltage = _voltage.substr(index,std::string::npos);

                    int anode = read_node_number(_anode);
                    int cathode = read_node_number(_cathode);
                    double voltage = read_value(_voltage);
                    bool swap = false;
                    for(auto x: voltage_anodes){
                        swap = anode == x;
                    }
                    if(swap){
                        int temp;
                        temp = anode;
                        anode = cathode;
                        cathode = temp;
                        voltage = -1 * voltage;
                    }
                    voltage_anodes.push_back(anode);
                    _circuit.add_component(new Voltage_Source(anode,cathode,_name,voltage));
                    std::cout<< "added "<<_name << read_node_number(_anode) << read_node_number(_cathode) <<" " <<read_value(_voltage) << std::endl;
                }
            }
            break;
        case 'd': 
            {
                //Diode source added to _circuit
                std::string _model_name;
                src >> _name >> _anode >> _cathode >> _model_name;
                _circuit.add_component(new Diode(read_node_number(_anode),read_node_number(_cathode),_name));
                std::cout<< "added "<<_name << read_node_number(_anode) << read_node_number(_cathode) <<" " <<_model_name << std::endl;
            }
            break;
        case 'c' :
            {
                //Capacitor added to circuit
                std::string _capacitance;
                src >> _name >> _anode >> _cathode >> _capacitance;
                _circuit.add_component(new Capacitor(read_node_number(_anode),read_node_number(_cathode),_name,read_value(_capacitance)));
                std::cout<< "added "<<_name << read_node_number(_anode) << read_node_number(_cathode) <<" " <<read_value(_capacitance) << std::endl;
            }
            break;
        case 'l' :
            {
                //Capacitor added to circuit
                std::string _inductance;
                src >> _name >> _anode >> _cathode >> _inductance;
                _circuit.add_component(new Inductor(read_node_number(_anode),read_node_number(_cathode),_name,read_value(_inductance)));
                std::cout<< "added "<<_name << read_node_number(_anode) << read_node_number(_cathode) <<" " <<read_value(_inductance) << std::endl;
            }
            break;
        case 'q' :
            {
                //Ebers-Moll BJT without connection resistances added to circuit
                std::string _collector;
                std::string _base;
                std::string _emitter;
                std::string _model_name;
                src >> _name >>_collector >> _base >> _emitter >> _model_name;
                _circuit.add_component(new BJT(read_node_number(_collector),read_node_number(_base),read_node_number(_emitter),
                    _name,0.67,0.995,1,10,0.2,0.3));
                std::cout<< "added "<<_name<< read_node_number(_collector) << read_node_number(_base) << read_node_number(_emitter) << " " <<_model_name << std::endl;
            }
            break;
        default:
            std::cout<< "non-handled case"<< std::endl;
            std::cout<<"tmp is "<<tmp<<std::endl;
            exit(1);
            break;
        }
        
        //ignoring input chars till \n
        src.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}


#endif
