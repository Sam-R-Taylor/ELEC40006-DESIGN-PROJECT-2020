#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <iostream>
#include <string>
#include <cmath>

class Component
{
protected:
    std::string name;
    int anode;
    int cathode;
public:
    Component(){}                        
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
        conductance = deltatime/inductance;
    }
    void set_linear_current(double In) 
    {
        linear_current = In;
    }
    double get_current(const std::vector<double> &nodeVoltages) const
    {
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
    double Current;
public:
    Current_source(int _anode, int _cathode, std:: string _name, double _current){
        anode = _anode;
        cathode = _cathode; 
        name = _name;
        Current = _current;
    }
    ~Current_source(){}
    //present for compatibilty with current component
    double get_current(const std::vector<double> &nodeVoltages) const
    {
        return Current;
    }
    double get_current() const
    {
        return Current;
    }
};   

class Capacitor :
    public Current_Component
{
private:
    /*
    integral of current wrt time. Current goes from anode to cathode
    */
    double conductance = 0;
    double linear_current = 0;
    double value;
public:
    Capacitor(int _anode, int _cathode, std:: string _name, double _value){
        anode = _anode;
        cathode = _cathode;
        name = _name;
        value = _value;
    }
    ~Capacitor(){}

    double get_conductance() const
    {
        return conductance;
    }
    double get_linear_current() const
    {
        return linear_current;
    }
    void set_conductance(double deltatime)
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
        double V = nodevoltages[anode]-nodevoltages[cathode];
        double current = 0;
        if(V > -5*N*Vt){
            current = I_s*(exp((V)/(N*Vt))-1) + (V)*GMIN;
        }else if (V > BV){
            current = V*GMIN;
        }else{
            current = -I_s*(exp(-(BV+V)/Vt)-1+BV/Vt);
        }
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

    //Constants for increased speed
    double GbGcGe;
    double GbGe;
    double GbGc;
    double GcGe;
    double AfAr;
    double AfGcGe;
    double ArGcGe;
    double AfArGb;
    double AfArGc;
    double AfArGe;
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
        //calculated for increased speed
        GbGcGe = Gb*Gc*Ge;
        GbGe = Gb*Ge;
        GbGc = Gb*Gc;
        GcGe = Gc*Ge;
        AfAr = Af*Ar;
        AfGcGe = Af*Gc*Ge;
        ArGcGe = Ar*Gc*Ge;
        AfArGe = Af*Ar*Ge;
        AfArGb = Af*Ar*Gb;
        AfArGc = Af*Ar*Gc;
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

        denom = GbGcGe + GbGc*Gf + GcGe*Gf + GbGe*Gr + Gb*Gf*Gr + GcGe*Gr + Gc*Gf*Gr + Ge*Gf*Gr
            - AfGcGe*Gr - ArGcGe*Gf - AfArGb*Gf*Gr - AfArGc*Gf*Gr - AfArGe*Gf*Gr;
    }
    //denom = (Gb*Gc*Ge + Gb*Gc*Gf + Gc*Ge*Gf + Gb*Ge*Gr + Gb*Gf*Gr + Gc*Ge*Gr + Gc*Gf*Gr + Ge*Gf*Gr - Af*Gc*Ge*Gr - Ar*Gc*Ge*Gf - Af*Ar*Gb*Gf*Gr - Af*Ar*Gc*Gf*Gr - Af*Ar*Ge*Gf*Gr
    //V1 = Gb*Ge*Ir + Gb*Gf*Ir + Ge*Gf*Ir - Ge*Gr*If - Ar*Gb*Ge*If + Gb*Gc*Ge*Vc + Gb*Gc*Gf*Vc + Gc*Ge*Gf*Vc + Gb*Ge*Gr*Vb + Gb*Gf*Gr*Vb + Gc*Ge*Gr*Vc + Gc*Gf*Gr*Vc + Ge*Gf*Gr*Ve - Af*Ar*Gb*Gf*Ir - Af*Ar*Ge*Gf*Ir + Af*Ar*Ge*Gr*If - Ar*Gb*Ge*Gf*Vb - Af*Gc*Ge*Gr*Vc - Ar*Gc*Ge*Gf*Vc + Ar*Gb*Ge*Gf*Ve - Af*Ar*Gb*Gf*Gr*Vb - Af*Ar*Gc*Gf*Gr*Vc - Af*Ar*Ge*Gf*Gr*Ve + 
    //V2 = Gb*Gc*If + Gb*Gr*If - Gc*Gf*Ir + Gc*Gr*If - Af*Gb*Gc*Ir + Gb*Gc*Gf*Vb + Gb*Gc*Ge*Ve + Gc*Ge*Gf*Ve + Gb*Gf*Gr*Vb + Gb*Ge*Gr*Ve + Gc*Gf*Gr*Vc + Gc*Ge*Gr*Ve + Ge*Gf*Gr*Ve - Af*Ar*Gb*Gr*If + Af*Ar*Gc*Gf*Ir - Af*Ar*Gc*Gr*If - Af*Gb*Gc*Gr*Vb + Af*Gb*Gc*Gr*Vc - Af*Gc*Ge*Gr*Ve - Ar*Gc*Ge*Gf*Ve - Af*Ar*Gb*Gf*Gr*Vb - Af*Ar*Gc*Gf*Gr*Vc - Af*Ar*Ge*Gf*Gr*Ve + 
    //V0 = Af*Gc*Ge*Ir - Gc*Ge*Ir - Gc*Gf*Ir - Ge*Gr*If - Gc*Ge*If + Ar*Gc*Ge*If + Gb*Gc*Ge*Vb + Gb*Gc*Gf*Vb + Gc*Ge*Gf*Ve + Gb*Ge*Gr*Vb + Gb*Gf*Gr*Vb + Gc*Ge*Gr*Vc + Gc*Gf*Gr*Vc + Ge*Gf*Gr*Ve + Af*Ar*Gc*Gf*Ir + Af*Ar*Ge*Gr*If - Af*Gc*Ge*Gr*Vc - Ar*Gc*Ge*Gf*Ve - Af*Ar*Gb*Gf*Gr*Vb - Af*Ar*Gc*Gf*Gr*Vc - Af*Ar*Ge*Gf*Gr*Ve + 
    double get_collector_coefficient(int node){
        double response;
        if(node == anode){//collector
            response = (1-(GbGcGe + Gf*(GbGc + GcGe) + Gr*(GcGe + Gc*Gf - 
                AfGcGe) - Gf*(ArGcGe + AfArGc*Gr))/denom)*Gc;
            collector_current_coeff[0] = response;

        }else if (node == cathode){//emmitter
            response = -1 * Ge * (Gr*(Gc*Gf + Af*GbGc - AfArGc*Gf))/denom;
            emmitter_current_coeff[0] = response;
        }else if (node == base){//base
            response = -1 * Gb * Gr * (GcGe - AfGcGe + Gf*(Gc - AfArGc))/denom;
            base_current_coeff[0] = response;
        }
        return response;
    }
    double get_base_coefficient(int node){
        double response;
        if(node == anode){//collector
            response = -1 * Gc * (Gr*(GbGe + Gb*Gf) - Gf*(Ar*GbGe + AfArGb*Gr)) / denom;
            collector_current_coeff[1] = response;
        }else if (node == cathode){//emmitter
            response = -1 * Ge * (GbGc*Gf + Gr*(Gb*Gf - Af*GbGc - AfArGb*Gf)) / denom;
            emmitter_current_coeff[1] = response;
        }else if (node == base){//base
            response = (1 - (GbGcGe + GbGc*Gf + Gr*(GbGe + Gb*Gf - AfArGb*Gf)) / denom) * Gb;
            base_current_coeff[1] = response;
        }
        return response;
    }
    double get_emmitter_coefficient(int node){
        double response;
        if(node == anode){//collector
            response = -1 * Gc * (Ge*Gf*Gr + Gf*(Ar*GbGe - AfArGe*Gr)) / denom;
            collector_current_coeff[2] = response;
        }else if (node == cathode){//emmitter
            response = (1 - (GbGcGe + Gr*(GbGe + GcGe + Ge*Gf -
                AfGcGe) + Gf*( - ArGcGe - AfArGe*Gr + GcGe)) / denom) * Ge;
            emmitter_current_coeff[2] = response;
        }else if (node == base){//base
            response = -1 * Gb * Gf * (GcGe - ArGcGe + Gr*(Ge - AfArGe)) / denom;
            base_current_coeff[2] = response;
        }
        return response;
    }
    double get_constant_coefficient(int node){
        double response;
        if(node == anode){//collector
            response = -1 * Gc * (Ir*(GbGe + Gb*Gf - AfArGb*Gf - AfArGe*Gf
                 + Ge*Gf) + If*(AfArGe*Gr - Ge*Gr - Ar*GbGe)) / denom;
            collector_current_coeff[3] = response;
        }else if (node == cathode){//emmitter
            response = -1 * Ge * (If*(GbGc + Gr*(Gb + Gc - 
                AfArGb - AfArGc)) + Ir*(Gf*(AfArGc - Gc) - Af*GbGc)) /denom;
            emmitter_current_coeff[3] = response;
        }else if (node == base){//base
            response = -1 * Gb * (Ir*(AfGcGe - GcGe + Gf*(AfArGc - Gc)) + If*(Gr*(AfArGe - Ge) - GcGe + 
                ArGcGe)) /denom;
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
public:
    double current;
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
    double get_voltage(){
        return gain;
    };
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
        DC_Offset = _DC_Offset;
        currentVoltage = _DC_Offset;
    }
    double Get_Voltage_amplitude() const
    {
        return Voltage_amplitude;
    }
    double Get_Frequency() const
    {
        return frequency;
    }
    void Set_Voltage(double CurrentTime)
    {
        currentVoltage = (Voltage_amplitude)*sin((2*M_PI*frequency*CurrentTime)) + DC_Offset ;
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
