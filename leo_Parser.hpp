#include"Circuit.hpp"
#include"Component.hpp"
#include<fstream>
#include<cctype>
#include<cmath>
#include<limits>

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







void parse_input(const std::string& input)
{
    std::fstream src;
    src.open (input, std::fstream::in);


    //initializing variables for Circuit and component objects
    Circuit _circuit;

    std:: string _name;
    int _anode;
    int _cathode; 
    double _value;


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
            double resistance;
            std::string node_a;
            std::string node_c;

            }
            break;
            
            
        default:
            std::cerr<< "non-handled case"<< std::endl;
            exit(1);
            break;
        }

        




    }




}