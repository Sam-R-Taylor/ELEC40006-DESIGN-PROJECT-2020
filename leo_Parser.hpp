#include"Circuit.hpp"
#include"Component.hpp"
#include<fstream>
#include<cctype>
#include<cmath>
#include<limits>
#include<cassert>



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
}

//helper function
double read_value(const std::string& value_str)
{
    size_t index = value_str.find_first_not_of(digits);
    if (index == std::string::npos)
    {
        return stoi(value_str);
    }
    else
    {
        char c = tolower(value_str[index]);

        if(c == 'f'){return femto;}
        else if(c == 'p'){return pico;}
        else if(c == 'n'){return nano;}
        else if(c == 'u'){return micro;}
        else if(c == 'k'){return kilo;}
        else if(c == 'm' && tolower(value_str[index+1]) == 'e' && tolower(value_str[index+2]) == 'g'){return mega;}
        else if(c == 'g'){return giga;}
        else if(c == 't'){return tera;}
        else if(c == 'm' && tolower(value_str[index+1]) == 'i' && tolower(value_str[index+2]) == 'l'){return mil;}
        else if(c == 'm'){return milli;}
    }
    

}





void parse_input(const std::string& input)
{
    std::fstream src;
    src.open (input, std::fstream::in);


    //initializing variables for Circuit and component objects
    Circuit _circuit;

    std:: string _name;
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
            src.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        
        case '.':
            /* reached a command */
            
            src.get();
            src >> command;

            if (command == "end")
            {
                std::cout << "terminating program" << std::endl;
                exit(0);
            }
            else if (command == "tran")
            {
                double stop_time;
                double time_step;

                //FIX USING READ VALUE

                src.ignore(std::numeric_limits<std::streamsize>::max(), '0');
                src >> stop_time;
                src.ignore(std::numeric_limits<std::streamsize>::max(), '0');
                src >> time_step;

                //NEEDS CONRTOLLER IMPLEMENTATION
            }
            else
            {
                std::cerr << "uknown command" << std::endl;
                exit(1);
            }
            break;
        
        case 'r':
            {
            std::string _resistance;
            

            src >> _name >> _anode >> _cathode >> _resistance;
            

            }
            break;
            
            
        default:
            std::cerr<< "non-handled case"<< std::endl;
            exit(1);
            break;
        }
    }
}