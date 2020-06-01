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


class Current_Component:
    public Component
{
    public:
    virtual double get_current(const std::vector<double> &nodeVoltages) const = 0;
};


class Resistor :
    public Current_Component
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
    double get_current(const std::vector<double> &nodeVoltages) const
    {
        double current = (nodeVoltages[anode]-nodeVoltages[cathode])/resistance;
        //std::cout << "Resistor Current" << current << std::endl;
        return current;
    }
    double get_conductance() const{
        return conductance;
    }
};






class Inductor :
    public Current_Component
{
private:
    /*
    integral of voltage wrt time. Voltage is defined as V_anode - V_cathode
    */
    double integral;
    double inductance;
    double conductance;
    double linear_current;
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

    double get_conductance() const
    {
        return conductance;
    }
    double get_linear_current() const
    {
        //std::cout << "Conductance " << conductance << std::endl;
        //std::cout << "VoltageN " << conductance << std::endl;
        return linear_current;
    }
    void set_conductance(double deltatime) //Not const as setters
    {
        conductance = deltatime/inductance;
    }
    void set_linear_current(double In) 
    {
        //std::cout << "current " << In << std::endl;
        linear_current = In;
    }
    double get_current(const std::vector<double> &nodeVoltages) const
    {
        std::cout << "Conductance " << conductance << std::endl;
        std::cout << "Voltage " << (nodeVoltages[anode]-nodeVoltages[cathode]) << std::endl;
        return linear_current + conductance * (nodeVoltages[anode]-nodeVoltages[cathode]);
    }
};




class Current_source :
    public Current_Component
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
    double get_current(const std::vector<double> &nodeVoltages) const
    {
        return current;
    }
    double get_current() const
    {
        return current;
    }
};   


class Capacitor :
    public Current_Component
{
private:
    /*
    integral of current wrt time. Current goes from anode to cathode
    */
    double conductance;
    double linear_current;
    double value;
    double integral;
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
    double get_conductance() const
    {
        return conductance;
    }
    double get_linear_current() const
    {
        return linear_current;
    }
    void set_conductance(double deltatime) //Not const as setters
    {
        conductance = value/deltatime;
    }
    void set_linear_current(double VoltageN) 
    {
        linear_current = conductance*VoltageN;
    }
    double get_current(const std::vector<double> &nodeVoltages) const
    {
        return linear_current + conductance * -1 * (nodeVoltages[anode]-nodeVoltages[cathode]);
    }
    /*
    given an increment computed by "controller" due to a timestep
    updates the value of Capacitor::integral
    */
    void update_integral(double increment)
    {
        //std::cout << "Integral " << integral << std::endl;
        integral += increment;
    }
};




class Diode:
    public Component
{
private:
    //Thermal Voltage
    double Vt = 0.0258641;
    double I_s = 2.52* pow(10,-9);
    //Voltage across terminals, used for linear aporximations
    double vd = 0;
    //current at voltage guess, used for linear aproximations
    double id0 = 0;
    //ideality coefficient 
    double N;;
    //breakdown voltage
    double BV = -75;
    double GMIN = pow(10,-12);
    //series resistance
    double Rs;
    //Conductances
    double G1;
    double G2;
public:
    Diode(int _anode, int _cathode, std:: string _name, double _Rs = 0.568, double _N = 1.752){
        anode = _anode;
        cathode = _cathode;
        name = _name;
        Rs =_Rs;
        N = _N;
        G1 = 1/Rs;
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
        double conductance = GMIN;
        //check if less than break down voltage
        if(vd < BV){
            conductance += ((-I_s/(N*Vt))*exp(-(BV+vd)/(N*Vt)));
        }//check if greater than -5*N*Vt
        else if(vd > -5*N*Vt){
            conductance += ((I_s/(N*Vt))*exp(vd/(N*Vt)));
        }
        return conductance;
    }
    //get the linear aproximations of the current source from the diode
    double get_linear_current(){
        //double current = (id0 - this->get_conductance()*vd);
        return (id0 - this->get_conductance()*vd);
    }
    void set_conductance(){
        G2 = get_conductance();
    }
    double get_anode_coefficient(){
        return (G1 - (G1*G1)/(G1+G2));
    }
    double get_cathode_coefficient(){
        return -1 * ( (G2*G1)/(G1+G2) );
    }
    double get_constant_coefficient(){
        return ((id0 - G2*vd)*(G1/(G1+G2)));
    }
    //get the current through the diode
    double get_current(const std::vector<double> &nodevoltages) const
    {
        double current = I_s*(exp((nodevoltages[anode]-nodevoltages[cathode])/(N*Vt))-1) + (nodevoltages[anode]-nodevoltages[cathode])*GMIN;
        //current = current>10?10:current;
        //std::cout << "Current  " <<current << std::endl; 
        return current;
    }
    double get_rs(){
        return Rs;
    }
    void set_cathode(int _index){cathode = _index;}
    void set_anode(int _index){anode = _index;}
};

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
        gain = _gain;
        control_voltage_anode = _control_voltage_anode;
        control_voltage_cathode = _control_voltage_cathode;
    }
    virtual double get_gain() const
    {
        return gain;        
    }
    int get_control_anode() const
    {
        return control_voltage_anode;
    }
    int get_control_cathode() const
    {
        return control_voltage_cathode;
    }
};

class Voltage_Component:
    public Component
{
public:
    virtual double get_voltage() const = 0;
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
        gain = _gain;
        control_voltage_anode = _control_voltage_anode;
        control_voltage_cathode = _control_voltage_cathode;
    }
    double get_gain() const
    {
        return gain;        
    }
    int get_control_anode() const
    {
        return control_voltage_anode;
    }
    int get_control_cathode() const
    {
        return control_voltage_cathode;
    }
};



class AC_Voltage_Source:
    public Voltage_Component
{
    private:
    double Voltage_amplitude;
    double frequency ;
    double phase;
    double currentVoltage;
    double DC_Offset;

    public:
    AC_Voltage_Source(int _anode, int _cathode, std::string _name,  double _Voltage_amplitude, double _frequency, double _phase , double _DC_Offset){
        anode = _anode;
        cathode = _cathode;
        name = _name;
        Voltage_amplitude = _Voltage_amplitude;
        frequency = _frequency;
        phase = _phase;
        DC_Offset = _DC_Offset;

    }
    double Get_Voltage_amplitude() const
    {
        return Voltage_amplitude;
    }
    double Get_Frequency() const
    {
        return frequency;
    }
    double Get_phase() const
    {
        return phase;
    }
    void Set_Voltage(double CurrentTime){
        currentVoltage = (Voltage_amplitude)*sin((2*M_PI*frequency*CurrentTime) + phase) + DC_Offset ;
        //y(t)=Asin(2pift + phase)

    }
    double get_voltage() const
    {
        return currentVoltage;
    }
    double Get_DC_Offset() const
    {
        return DC_Offset;
    }
};


/*
class BJT
{
//Ebers-Moll model
private:
    //diodes constants
    double IS;
    double NF;
    double NR;
    double BF;
    double BR;

    //diodes
    Diode D1();
    Diode D2();

    //connection resistances
    Resistor RE();
    Resistor RB();
    Resistor RC();
public:
    BJT(std::string _name,int _collector, int _base, int _emitter, std::string _model) {}
    ~BJT() {}
};*/


#endif