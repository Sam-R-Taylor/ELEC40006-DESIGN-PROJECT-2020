#ifndef LEO_COMPONENT_HPP
#define LEO_COMPONENT_HPP

#include<iostream>
#include<string>
#include<cmath>
#include<vector>




class Component
{
protected:
    std::string name;
    int anode;
    int cathode;
public:                          
    virtual ~Component(){}
    /*
    reurns the anode of the component
    */     
    int get_anode() const {return anode;}
    /*
    reurns the cathode of the component
    */  
    int get_cathode() const {return cathode;}
    /*
    reurns the name of the component
    */  
    std::string get_name() const {return name;}
};




class Resistor :
    public Component
{
private:
    double resistance;
public:
    Resistor(int _anode, int _cathode, std:: string _name, double _resistance){
        anode = _anode;
        cathode = _cathode;
        name = _name;
        resistance = _resistance;
    }
    double get_resistance() const{
        return resistance;
    }
    double get_current(const std::vector<double> &nodevoltages) const
    {
        double current = (nodevoltages[anode]-nodevoltages[cathode])/resistance;
        return current;
    }
    double get_conductance() const{return 1/resistance;}
};




class Current_source :
    public Component
{
private:
    /*
    fixed current going from anode to cathode
    */
    double current;
public:
    Current_source(int _anode, int _cathode, std:: string _name, double _current){
        anode = _anode;
        cathode = _cathode;
        name = _name;
        current = _current;
    }
    double get_current() const {return current;}
    double get_conductance() const{return 0;}
};    


#endif