#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include<iostream>
#include<string>
#include<cmath>

class Component
{
protected:
    std::string name;
    int anode;
    int cathode;
public:
    Component(){}                            //to implement
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
protected:
    double resistance;
    double conductance;
    
public:
    Resistor(int _anode, int _cathode, std:: string _name, double _resistance){
        anode = _anode;
        cathode = _cathode;
        name = _name; 
        resistance = _resistance;
        conductance = 1/resistance;
    }
    ~Resistor(){}

    double get_value() const{
        return resistance;
    }
    double get_current(const std::vector<double> &nodevoltages) const
    {
        double current = (nodevoltages[anode]-nodevoltages[cathode])/resistance;
        //std::cout << "Resistor Current" << current << std::endl;
        return current;
    }
    double get_conductance() const{
        return conductance;
    }
};






class Inductor :
    public Component
{
private:
    /*
    integral of voltage wrt time. Voltage is defined as V_anode - V_cathode
    */
    double integral;
    double inductance;
public:
    Inductor(int _anode, int _cathode, std:: string _name, double _inductance){
        anode = _anode;
        cathode = _cathode;
        name = _name;
        inductance = _inductance;
    }
    ~Inductor(){}
    /*
    given an increment computed by "controller" due to a timestep
    updates the inductance of Inductor::integral
    */
    void update_integral(double increment){
        integral += increment;
    }
    
    double get_current() const        //does not need the parameter. modify later
    {
        return integral/inductance;
    }
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
    ~Current_source(){}
    double get_current() const
    {
        return current;
    }
};    



class Diode:
    public Component
{
private:
    //Thermal Voltage
    double Vt = 0.025;
    double I_s = 0.0000000000001;
    //Voltage across terminals, used for linear aporximations
    double vd = 0;
    //current at voltage guess, used for linear aproximations
    double id0 = 0;
public:
    Diode(int _anode, int _cathode, std:: string _name){
        anode = _anode;
        cathode = _cathode;
        name = _name;
    }
    ~Diode(){}
    //set the voltage across the diode
    void set_vd(double _vd){
        vd = _vd;
    }
    //set the current through the diode at operating point
    void set_id0(double _id0){
        id0 = _id0;
    }
    //get the linear aproximation of the conductor for the diode
    double get_conductance(){
        double conductance = ((I_s/Vt)*exp(vd/Vt));
        conductance = conductance<0.01?0.01:conductance;
        conductance = conductance>20?20:conductance;
        std::cout << "conductance " << conductance << std::endl;
        std::cout << "Vd " << vd << std::endl;
        return conductance;
    }
    //get the linear aproximations of the current source from the diode
    double get_linear_current(){
        std::cout << "current " << (id0 - ((I_s/Vt)*exp(vd/Vt))*vd) << std::endl;
        double current = (id0 - this->get_conductance()*vd);
        current = current>10?10:current;
        return current;
    }
    //get the current through the diode
    double get_current(const std::vector<double> &nodevoltages) const
    {
        double current = I_s*(exp((nodevoltages[anode]-nodevoltages[cathode])/Vt)-1);
        current = current>10?10:current;
        std::cout << "Current  " <<current << std::endl; 
        return current;
    }
    
};

//added sketch of Voltage_Controlled_Current_Source
class Voltage_Controlled_Current_Source :                
    public Component
{
private:
    double gain;
    int control_voltage_anode;
    int control_voltage_cathode;
public:
    Voltage_Controlled_Current_Source(int _anode, int _cathode, std:: string _name, double _gain, int _control_voltage_anode, int _control_voltage_cathode)
    {
        anode = _anode;
        cathode = _cathode;
        name = _name;
        control_voltage_anode = _control_voltage_anode;
        control_voltage_cathode = _control_voltage_cathode;
    }
    virtual double get_current() const
    {
        return gain;        //todo need something that returns voltage at control_voltage_anode and control_voltage_cathode
    }
};



class Voltage_Component:
    public Component
{

public:
    /*
    given a vector<double> nodevoltages where 
    -each index of nodevoltages corresponds to a node
    -each double of nodevoltages corresponds to a voltage at the given node
    returns the  through the voltage difference across the component from anode to cathode
    */ 
    virtual double get_voltage() const =0;

    
};
class Voltage_Source:
    public Voltage_Component
{
private:
    /*
    voltage going from anode to cathode
    */
    double voltage;
public:
    Voltage_Source(int _anode, int _cathode, std:: string _name, double _voltage){
        anode = _anode;
        cathode = _cathode;
        name = _name;
        voltage = _voltage;
    }
    ~Voltage_Source(){}

    double get_voltage() const
    {
        return voltage;
    }
};





class Capacitor :
    public Voltage_Component
{
private:
    /*
    integral of current wrt time. Current goes from anode to cathode
    */
    double integral;
    double value;
public:
    Capacitor(int _anode, int _cathode, std:: string _name, double _value){
        anode = _anode;
        cathode = _cathode;
        name = _name;
        value = _value;
    }
    ~Capacitor(){}


    double get_voltage() const
    {
        return integral/value;
    }
    /*
    given an increment computed by "controller" due to a timestep
    updates the value of Capacitor::integral
    */
    void update_integral(double increment)
    {
        std::cout << "Integral " << integral << std::endl;
        integral += increment;
    }
};




//added sketch of Voltage_Controlled_Voltage_Source
class Voltage_Controlled_Voltage_Source :                
    public Voltage_Component
{
private:
    double gain;
    int control_voltage_anode;
    int control_voltage_cathode;
public:
    Voltage_Controlled_Voltage_Source(int _anode, int _cathode, std:: string _name, double _gain, int _control_voltage_anode, int _control_voltage_cathode)
    {
        anode = _anode;
        cathode = _cathode;
        name = _name;
        control_voltage_anode = _control_voltage_anode;
        control_voltage_cathode = _control_voltage_cathode;
    }
    double get_voltage() const
    {
        return gain;        //todo need something that returns voltage at control_voltage_anode and control_voltage_cathode
    }
};




#endif