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
    double current = 0;
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
        //std::cout << "Conductance " << conductance << std::endl;
        //std::cout << "Voltage " << (nodeVoltages[anode]-nodeVoltages[cathode]) << std::endl;
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
        return conductance * (nodeVoltages[anode]-nodeVoltages[cathode]) - linear_current;
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
    public Current_Component
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
        //std::cout << "Vd " << vd << std::endl;
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
        //std::cout << "conductance " << get_conductance() << std::endl;
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
    public Current_Component
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

class BJT:                
    public Component
{    
private:
    bool PNP;

    double Af;//The gain for the BE component current source
    double Ar;//The gain for the BC component current source
    int base;//stores the base node

    double Ge;//The emmitter conductance
    double Gc;//The collector conductance
    double Gb;//The base conductance
    //Conductances of linear model
    double Gf;
    double Gr;
    //Currents of linear model
    double Ir;
    double If;
    //Current multipliers for linear Model
    double Br;
    double Bf;
    //Diode Parameters
    double Vt = 0.0258641;//Thermal Voltage
    double I_s = 1* pow(10,-14);
    double N;//ideality coefficient 
    double BV = -75;//breakdown voltage
    double GMIN = pow(10,-12);//Min Conductance
    
    //variable to speed things up
    double denom;

    //store the coefficients for current to work out the current passing through
    std::vector<double> collector_current_coeff{0,0,0,0}; //current comming into collector c b e i
    std::vector<double> base_current_coeff{0,0,0,0}; //current coming into base
    std::vector<double> emmitter_current_coeff{0,0,0,0}; //current coming into emmitter

public:
    std::vector<double> currents{0,0,0};
    BJT(int _collector, int _base, int _emmitter, std::string _name, double _Af, double _Ar, int _N, double _Rb, double _Rc, double _Re, bool _PNP = false)
    {
        PNP = _PNP;
        anode = _collector;
        cathode = _emmitter;
        base = _base;
        name = _name;
        Ar = _Ar;
        Af = _Af;
        N = _N;
        Gb = 1/_Rb;
        Gc = 1/_Rc;
        Ge = 1/_Re;
        Af = _Af;
        Ar = _Ar;
    }
    //gets the current through the diode with voltage v
    double get_diode_current(double v){
            double current = I_s*(exp((v)/(N*Vt))-1) + (v)*GMIN;
            return current;
    }
    double get_diode_current_derrivative(double v){
        double current = (I_s/(N*Vt))*exp((v)/(N*Vt)) + GMIN;
        return current;
    }
    //must be called by transient solver
    void set_op(std::vector<double> voltages){
        double vd;
        double _vd;
        double current_collector = collector_current_coeff[0] * voltages[anode] + 
            collector_current_coeff[1] * voltages[base] +
            collector_current_coeff[2] * voltages[cathode] + collector_current_coeff[3];
        double current_emmitter = emmitter_current_coeff[0] * voltages[anode] + 
            emmitter_current_coeff[1] * voltages[base] +
            emmitter_current_coeff[2] * voltages[cathode] + emmitter_current_coeff[3];
        double V0 = voltages[base] + (current_collector + current_emmitter) / Gb;
        double V1 = voltages[anode] - (current_collector) / Gc;
        double V2 = voltages[cathode] - (current_emmitter) / Ge;
        //vd = voltages[base] - voltages[anode];//anode is the collector cathode is emmitter
        //_vd = voltages[base] - voltages[cathode];
        vd = V0 - V1;
        _vd = V0 - V2;
        if(PNP){
            vd = -vd;
            _vd = -_vd;
        }
        Gr = get_diode_current_derrivative(vd);
        Gf = get_diode_current_derrivative(_vd);
        Ir = get_diode_current(vd) - vd*Gr;
        If = get_diode_current(_vd) - _vd*Gf;

        denom = Gb*Gc*Ge + Gb*Gc*Gf + Gc*Ge*Gf + Gb*Ge*Gr + Gb*Gf*Gr + Gc*Ge*Gr + Gc*Gf*Gr + Ge*Gf*Gr
            - Af*Gc*Ge*Gr - Ar*Gc*Ge*Gf - Af*Ar*Gb*Gf*Gr - Af*Ar*Gc*Gf*Gr - Af*Ar*Ge*Gf*Gr;
    }
    //denom = (Gb*Gc*Ge + Gb*Gc*Gf + Gc*Ge*Gf + Gb*Ge*Gr + Gb*Gf*Gr + Gc*Ge*Gr + Gc*Gf*Gr + Ge*Gf*Gr - Af*Gc*Ge*Gr - Ar*Gc*Ge*Gf - Af*Ar*Gb*Gf*Gr - Af*Ar*Gc*Gf*Gr - Af*Ar*Ge*Gf*Gr
    //V1 = Gb*Ge*Ir + Gb*Gf*Ir + Ge*Gf*Ir - Ge*Gr*If - Ar*Gb*Ge*If + Gb*Gc*Ge*Vc + Gb*Gc*Gf*Vc + Gc*Ge*Gf*Vc + Gb*Ge*Gr*Vb + Gb*Gf*Gr*Vb + Gc*Ge*Gr*Vc + Gc*Gf*Gr*Vc + Ge*Gf*Gr*Ve - Af*Ar*Gb*Gf*Ir - Af*Ar*Ge*Gf*Ir + Af*Ar*Ge*Gr*If - Ar*Gb*Ge*Gf*Vb - Af*Gc*Ge*Gr*Vc - Ar*Gc*Ge*Gf*Vc + Ar*Gb*Ge*Gf*Ve - Af*Ar*Gb*Gf*Gr*Vb - Af*Ar*Gc*Gf*Gr*Vc - Af*Ar*Ge*Gf*Gr*Ve + 
    //V2 = Gb*Gc*If + Gb*Gr*If - Gc*Gf*Ir + Gc*Gr*If - Af*Gb*Gc*Ir + Gb*Gc*Gf*Vb + Gb*Gc*Ge*Ve + Gc*Ge*Gf*Ve + Gb*Gf*Gr*Vb + Gb*Ge*Gr*Ve + Gc*Gf*Gr*Vc + Gc*Ge*Gr*Ve + Ge*Gf*Gr*Ve - Af*Ar*Gb*Gr*If + Af*Ar*Gc*Gf*Ir - Af*Ar*Gc*Gr*If - Af*Gb*Gc*Gr*Vb + Af*Gb*Gc*Gr*Vc - Af*Gc*Ge*Gr*Ve - Ar*Gc*Ge*Gf*Ve - Af*Ar*Gb*Gf*Gr*Vb - Af*Ar*Gc*Gf*Gr*Vc - Af*Ar*Ge*Gf*Gr*Ve + 
    //V0 = Af*Gc*Ge*Ir - Gc*Ge*Ir - Gc*Gf*Ir - Ge*Gr*If - Gc*Ge*If + Ar*Gc*Ge*If + Gb*Gc*Ge*Vb + Gb*Gc*Gf*Vb + Gc*Ge*Gf*Ve + Gb*Ge*Gr*Vb + Gb*Gf*Gr*Vb + Gc*Ge*Gr*Vc + Gc*Gf*Gr*Vc + Ge*Gf*Gr*Ve + Af*Ar*Gc*Gf*Ir + Af*Ar*Ge*Gr*If - Af*Gc*Ge*Gr*Vc - Ar*Gc*Ge*Gf*Ve - Af*Ar*Gb*Gf*Gr*Vb - Af*Ar*Gc*Gf*Gr*Vc - Af*Ar*Ge*Gf*Gr*Ve + 
    double get_collector_coefficient(int node){
        double response;
        if(node == anode){//collector
            response = (1-(Gb*Gc*Ge + Gb*Gc*Gf + Gc*Ge*Gf + Gc*Ge*Gr + Gc*Gf*Gr - 
                Af*Gc*Ge*Gr - Ar*Gc*Ge*Gf - Af*Ar*Gc*Gf*Gr)/denom)*Gc;
            collector_current_coeff[0] = response;

        }else if (node == cathode){//emmitter
            response = -1 * Ge * (Gc*Gf*Gr + Af*Gb*Gc*Gr - Af*Ar*Gc*Gf*Gr)/denom;
            emmitter_current_coeff[0] = response;
        }else if (node == base){//base
            response = -1 * Gb * (Gc*Ge*Gr + Gc*Gf*Gr - Af*Gc*Ge*Gr - Af*Ar*Gc*Gf*Gr)/denom;
            base_current_coeff[0] = response;
        }
        return response;
    }
    double get_base_coefficient(int node){
        double response;
        if(node == anode){//collector
            response = -1 * Gc * (Gb*Ge*Gr + Gb*Gf*Gr - Ar*Gb*Ge*Gf - Af*Ar*Gb*Gf*Gr) / denom;
            collector_current_coeff[1] = response;
        }else if (node == cathode){//emmitter
            response = -1 * Ge * (Gb*Gc*Gf + Gb*Gf*Gr - Af*Gb*Gc*Gr - Af*Ar*Gb*Gf*Gr) / denom;
            emmitter_current_coeff[1] = response;
        }else if (node == base){//base
            response = (1 - (Gb*Gc*Ge + Gb*Gc*Gf + Gb*Ge*Gr + Gb*Gf*Gr - Af*Ar*Gb*Gf*Gr) / denom) * Gb;
            base_current_coeff[1] = response;
        }
        return response;
    }
    double get_emmitter_coefficient(int node){
        double response;
        if(node == anode){//collector
            response = -1 * Gc * (Ge*Gf*Gr + Ar*Gb*Ge*Gf - Af*Ar*Ge*Gf*Gr) / denom;
            collector_current_coeff[2] = response;
        }else if (node == cathode){//emmitter
            response = (1 - (Gb*Gc*Ge + Gc*Ge*Gf + Gb*Ge*Gr + Gc*Ge*Gr + Ge*Gf*Gr -
                Af*Gc*Ge*Gr - Ar*Gc*Ge*Gf - Af*Ar*Ge*Gf*Gr) / denom) * Ge;
            emmitter_current_coeff[2] = response;
        }else if (node == base){//base
            response = -1 * Gb * (Gc*Ge*Gf + Ge*Gf*Gr - Ar*Gc*Ge*Gf - Af*Ar*Ge*Gf*Gr) / denom;
            base_current_coeff[2] = response;
        }
        return response;
    }
    double get_constant_coefficient(int node){
        double response;
        if(node == anode){//collector
            response = -1 * Gc * (Gb*Ge*Ir + Gb*Gf*Ir + Ge*Gf*Ir - Ge*Gr*If - Ar*Gb*Ge*If
                - Af*Ar*Gb*Gf*Ir - Af*Ar*Ge*Gf*Ir + Af*Ar*Ge*Gr*If) / denom;
            collector_current_coeff[3] = response;
        }else if (node == cathode){//emmitter
            response = -1 * Ge * (Gb*Gc*If + Gb*Gr*If - Gc*Gf*Ir + Gc*Gr*If - Af*Gb*Gc*Ir - 
                Af*Ar*Gb*Gr*If + Af*Ar*Gc*Gf*Ir - Af*Ar*Gc*Gr*If) /denom;
            emmitter_current_coeff[3] = response;
        }else if (node == base){//base
            response = -1 * Gb * (Af*Gc*Ge*Ir - Gc*Ge*Ir - Gc*Gf*Ir - Ge*Gr*If - Gc*Ge*If + 
                Ar*Gc*Ge*If + Af*Ar*Gc*Gf*Ir + Af*Ar*Ge*Gr*If) /denom;
            base_current_coeff[3] = response;
        }
        return response;
    }

    int get_base(){
        return base;
    }
    
    std::vector<double> get_current(std::vector<double> voltages){
        std::vector<double> output{0,0,0};
        double current_collector = collector_current_coeff[0] * voltages[anode] + 
            collector_current_coeff[1] * voltages[base] +
            collector_current_coeff[2] * voltages[cathode] + collector_current_coeff[3];
        double current_emmitter = emmitter_current_coeff[0] * voltages[anode] + 
            emmitter_current_coeff[1] * voltages[base] +
            emmitter_current_coeff[2] * voltages[cathode] + emmitter_current_coeff[3];
        double current_base = base_current_coeff[0] * voltages[anode] + 
            base_current_coeff[1] * voltages[base] +
            base_current_coeff[2] * voltages[cathode] + base_current_coeff[3];
        output[0] = current_collector;
        output[1] = current_base;
        output[2] = current_emmitter;
        return output;
    }
};

class Voltage_Component:
    public Component
{
    std::vector<double> current_coefficients;
public:
    double current;
    virtual double get_voltage() const = 0;
    void set_coefficients(std::vector<double> _current_coefficients){
        current_coefficients = _current_coefficients;
    }
    double get_current(std::vector<double> voltages){
        double current;
        for(int i=0; i< voltages.size(); i++){
            current += voltages[i] * current_coefficients[i];
        }
        return current;
    }
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
    double get_voltage() const{};
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
    AC_Voltage_Source(int _anode, int _cathode, std::string _name,  double _Voltage_amplitude, double _frequency, double _DC_Offset){
        anode = _anode;
        cathode = _cathode;
        name = _name;
        Voltage_amplitude = _Voltage_amplitude;
        frequency = _frequency;
        //phase = _phase;
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
    /*
    double Get_phase() const
    {
        return phase;
    }
    */
    void Set_Voltage(double CurrentTime)
    {
        currentVoltage = (Voltage_amplitude)*sin((2*M_PI*frequency*CurrentTime)) + DC_Offset ;
        //std::cout << "sin arg is " << (2*M_PI*frequency*CurrentTime) << std::endl;

        //std::cout<<"current frequency is " << frequency << std::endl;
        //y(t)=Asin(2pift)

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


#endif
