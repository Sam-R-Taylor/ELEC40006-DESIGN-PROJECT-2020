#ifndef COMPONENT_HPP
#define COMPONENT_HPP


#include<string>

class Component
{
protected:
    std::string name;
    int anode;
    int cathode;


public:
    Component();
    virtual ~Component();

    /*
    reurns the name of the component
    */
    std::string get_name() const {return name;}

    /*
    reurns the anode of the component
    */     
    int get_anode() const {return anode;}

    /*
    reurns the cathode of the component
    */  
    int get_cathode() const {return cathode;}
};





class Resistor :
    public Component
{

public:
    Resistor();
    ~Resistor();
};

class Capacitor :
    public Component
{

public:
    Capacitor();
    ~Capacitor();
};

class Inductor :
    public Component
{

public:
    Inductor();
    ~Inductor();
};


#endif

