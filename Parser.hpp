#ifndef LEO_PARSER_HPP
#define LEO_PARSER_HPP

#include"Circuit.hpp"
#include"Component.hpp"
#include"leo_KCLSolver.hpp"

#include<fstream>
#include<cctype>
#include<cmath>
#include<limits>
#include<cassert>
#include<chrono>



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

//data for read_value
std::string digits("9876543210");





//helper function
int node_number(const std::string& node_name)
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
        //std::cerr << "if"<< std::endl;
        //std::cerr << value_str<< std::endl;
        return stod(value_str);
    }
    else
    {
        //std::cerr << "else"<< std::endl;
        //std::cerr << value_str<< std::endl;


        char c = tolower(value_str[index]);
        //std::cerr << "char is " << c << std::endl;
        double number = stod(value_str.substr(0,index));

        if(c == 'f'){return femto*number;}
        else if(c == 'p'){return pico*number;}
        else if(c == 'n'){return nano*number;}
        else if(c == 'u'){return micro*number;}
        else if(c == 'k'){return kilo*number;}
        else if(c == 'm' && tolower(value_str[index+1]) == 'e' && tolower(value_str[index+2]) == 'g'){return mega*number;}
        else if(c == 'g'){return giga*number;}
        else if(c == 't'){return tera*number;}
        else if(c == 'm' && tolower(value_str[index+1]) == 'i' && tolower(value_str[index+2]) == 'l'){return mil*number;}
        else if(c == 'm'){return milli*number;}
    }
    return 0;
}





void parse_input(const std::string& input)
{
    std::fstream src;
    src.open (input, std::fstream::in);


    //initializing variables for Circuit and component objects
    Circuit _circuit;

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

                std::cout << "terminating program" << std::endl;
                exit(0);
            }
            else if (command == "tran")
            {
                std::string stop_time_str;
                std::string time_step_str;

                src.ignore(std::numeric_limits<std::streamsize>::max(), '0');
                src >> stop_time_str;
                double stop_time = read_value(stop_time_str);
                src.ignore(std::numeric_limits<std::streamsize>::max(), '0');
                src >> time_step_str;
                double time_step = read_value(time_step_str);

                //building Circuit obj
                _circuit.build_nodes();


                //NEEDS CONRTOLLER IMPLEMENTATION
                auto start = std::chrono::steady_clock::now();
                for(int i=0; i<1; i++){
                    Matrix_solver(_circuit); 
                }
                //for(int i = 0; i < 1000; i++){
                //    NodeVoltageSolver(circuit);
                //}
                auto end = std::chrono::steady_clock::now();
                auto diff = end - start;
                std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

            }
            else if (command == "op")
            {
                //building Circuit obj
                _circuit.build_nodes();
                _circuit.print_node_components();
                //std::cout << "added connections" <<std::endl;
                //_circuit.add_connection_resistors();
                //_circuit.print_node_components();

                //need to set capacitors as OC and inductors as SC

                Matrix_solver(_circuit);
            }
            else
            {   
                std::cerr << "uknown command" << std::endl;
                exit(1);
            }
            break;
        

        case 'r':
            {
                //Resistor added to _circuit
                std::string _resistance;
                src >> _name >> _anode >> _cathode >> _resistance;
                _circuit.add_component(new Resistor(node_number(_anode),node_number(_cathode),_name,read_value(_resistance)));
                std::cerr<< "added "<<_name << node_number(_anode) << node_number(_cathode) <<" " <<read_value(_resistance) << std::endl;
            }
            break;

        case 'i':
            {
                //Current source added to _circuit
                std::string _current;
                src >> _name >> _anode >> _cathode >> _current;
                _circuit.add_component(new Current_source(node_number(_anode),node_number(_cathode),_name,read_value(_current)));
                std::cerr<< "added "<<_name << node_number(_anode) << node_number(_cathode) << " "<<read_value(_current) << std::endl;
            }   
            break; 
        case 'v': 
            {
                //Voltage source added to _circuit
                std::string _voltage;
                src >> _name >> _anode >> _cathode >> _voltage;
                _circuit.add_component(new Voltage_Source(node_number(_anode),node_number(_cathode),_name,read_value(_voltage)));
                std::cerr<< "added "<<_name << node_number(_anode) << node_number(_cathode) <<" " <<read_value(_voltage) << std::endl;
            }
            break;
        case 'd': 
            {
                //Voltage source added to _circuit
                std::string _model_name;
                src >> _name >> _anode >> _cathode >> _model_name;
                _circuit.add_component(new Diode(node_number(_anode),node_number(_cathode),_name));
                std::cerr<< "added "<<_name << node_number(_anode) << node_number(_cathode) <<" " <<_model_name << std::endl;
            }
            break;
        default:
            std::cerr<< "non-handled case"<< std::endl;
            std::cerr<<"tmp is "<<tmp<<std::endl;
            exit(1);
            break;
        }

        //ignoring input chars till \n
        src.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}


#endif