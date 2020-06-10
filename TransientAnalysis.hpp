#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <string>
#include "Component.hpp"
#include <memory> 
#include "Circuit.hpp"
#include <fstream>
#include "TransientSolver.hpp"
//#include "KCLSolver.hpp"
#include "leo_KCLSolver2.hpp"


using namespace std;
using Eigen::MatrixXd;

double current_at_voltage_component(Circuit &circuit, Voltage_Component* component, int _node = -1){
    double current = 0;
    int node = _node == -1?component->get_anode():_node;
    for(int i = 0; i < circuit.get_nodes().at(node).components_attached.size(); i++){
        Component* _component = circuit.get_nodes().at(node).components_attached[i];
        Voltage_Component* v = dynamic_cast<Voltage_Component*>(_component);
        if(v != component){
            if(dynamic_cast<Current_Component*>(_component)){
                current += ((Current_Component*)_component)->current * 
                    (_component->get_anode() == node?-1:1);
            }
            if(dynamic_cast<BJT*>(_component)){
                int connection = 1;
                if(_component->get_anode() == node){
                    connection = 0;
                }else if (_component->get_cathode() == node){
                    connection = 2;
                }
                current -= ((BJT*)_component)->currents[connection];
            }
            if(dynamic_cast<Voltage_Component*>(_component)){
                int next_node = _component->get_anode() == node?_component->get_cathode():_component->get_anode(); 
                current += current_at_voltage_component(circuit,dynamic_cast<Voltage_Component*>(_component),next_node);
            }
        }
    }
    return current;
}
void calculate_currents(Circuit &circuit){
    std::vector<Voltage_Component*> voltage_components;
    for (Component* component: circuit.get_components()){
        if(dynamic_cast<Current_Component*>(component)){
            ((Current_Component*)component)->current = ((Current_Component*)component)->get_current(circuit.get_voltages());
        }
        if(dynamic_cast<BJT*>(component)){
            ((BJT*)component)->currents = ((BJT*)component)->get_current(circuit.get_voltages());
        }
        if(dynamic_cast<Voltage_Component*>(component)){
            voltage_components.push_back(dynamic_cast<Voltage_Component*>(component));
        }
    }
    for(Voltage_Component* component: voltage_components){
        component->current = current_at_voltage_component(circuit,component);
    }
}
double GetCurrent(Component* component){
    if(dynamic_cast<Voltage_Component*>(component)){
        return dynamic_cast<Voltage_Component*>(component)->current;
    }
    if(dynamic_cast<Current_Component*>(component)){
        return dynamic_cast<Current_Component*>(component)->current;
    }
    return 0;
}
void NodeVoltagesToFile(Circuit& CKTIn , double CurrentTime){
    fstream myfile;
    myfile.open("output_voltage.txt",fstream::app);
    fstream myfile2;
    myfile2.open("output_current.txt",fstream::app);
    if (myfile.is_open())
    {
        myfile << CurrentTime << "," ;
        for(int i = 0; i < CKTIn.get_voltages().size(); i++) {
            myfile << CKTIn.get_voltages().at(i) << ",";
        }
        //myfile << CKTIn.get_voltages().at(CKTIn.get_voltages().size()-1);
        //COULD NOT REMOVE THE TRAILING , HOPE no affect?
        myfile << "\n";
        myfile.close();
    }else cout << "Unable to open voltage file";
    
    if(myfile2.is_open()){
        if(CurrentTime == 0){
            for(int i = 0; i < CKTIn.get_components().size(); i++){
            //for(Component* component: CKTIn.get_components()){
                if(dynamic_cast<BJT*>(CKTIn.get_components().at(i))){
                    myfile2 << CKTIn.get_components().at(i)->get_name() << "C" << ", ";
                    myfile2 << CKTIn.get_components().at(i)->get_name() << "B" << ", ";
                    myfile2 << CKTIn.get_components().at(i)->get_name() << "E" << ", ";
                }else{
                    myfile2 << CKTIn.get_components().at(i)->get_name() << ", ";
                }
            }
            /*if(dynamic_cast<BJT*>(CKTIn.get_components().at(CKTIn.get_components().size()-1))){
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name() << "C" << ", ";
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name() << "B" << ", ";
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name() << "E";
            }else{
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name();
            }*/
            myfile2 << "\n";
        }
        myfile2 << CurrentTime << "," ;
        for(int i = 0; i < CKTIn.get_components().size(); i++){
        //for(Component* component: CKTIn.get_components()){
                if(dynamic_cast<BJT*>(CKTIn.get_components().at(i))){
                    std::vector<double> current = ((BJT*)CKTIn.get_components().at(i))->currents;
                    myfile2 << current[0] << ", ";
                    myfile2 << current[1] << ", ";
                    myfile2 << current[2] << ", ";
                }else{
                
                    myfile2 << GetCurrent(CKTIn.get_components().at(i)) << ", ";
                }
        }
        /*if(dynamic_cast<BJT*>(CKTIn.get_components().at(CKTIn.get_components().size()-1))){
            std::vector<double> current = ((BJT*)CKTIn.get_components().at(CKTIn.get_components().size()-1))->get_current(CKTIn.get_voltages());
            myfile2 << current[0] << ", ";
            myfile2 << current[1] << ", ";
            myfile2 << current[2];
        }else{
            myfile2 << GetCurrent(CKTIn,CKTIn.get_components().at(CKTIn.get_components().size()-1));
        }*/
        myfile2 << "\n";
  }
  else cout << "Unable to open current file";
}

void UpdateNodeVoltages(Circuit &CKTIn , double CurrentTime,bool isLinear){
     //All this function does is update the integrals of each component and then passes the updates CKT to Transient Solver. 
     //Update Capacitors and inductors integrals
    double Vn;
    double In;
     //LOOPS THROUGH TO UPDATE NOT INTEGRAL COMPONENTS I.E AC SOURCE
      //IMPLEMENTATION ONLY WORKS FOR RLC 
    for(int i = 0 ; i < static_cast<int>(CKTIn.get_components().size()) ; i++){
        if (dynamic_cast<AC_Voltage_Source*>(CKTIn.get_components().at(i))){
            ((AC_Voltage_Source*)(CKTIn.get_components()[i]))->Set_Voltage(CurrentTime);
        }
    }
    //update all the voltages
    if(isLinear){
        Matrix_solver(CKTIn);
    }else{
        TransientSolver(CKTIn);
    }
    //update all the currents
    calculate_currents(CKTIn);
    //print the voltages and currents to files
    NodeVoltagesToFile(CKTIn,CurrentTime);

    //LOOPS THROUGH TO UPDATE INTEGRAL COMPONENTS
    for(int i = 0 ; i < static_cast<int>(CKTIn.get_components().size()) ; i++){
        if(dynamic_cast<Capacitor*>(CKTIn.get_components().at(i))){ //DETERMINES THAT THE COMPONENT IS A CAPACITOR
            Vn = (CKTIn.get_voltages()[CKTIn.get_components()[i]->get_anode()]) - 
                ((CKTIn.get_voltages()[CKTIn.get_components()[i]->get_cathode()])) ;
            ((Capacitor*)(CKTIn.get_components()[i]))->set_linear_current(Vn);
        }
        else if (dynamic_cast<Inductor*>(CKTIn.get_components().at(i)))
        {
            In = GetCurrent(CKTIn.get_components()[i]); //GETCURRENT WILL BE IMPLEMENTED BY MAX ,
            ((Inductor*)(CKTIn.get_components()[i]))->set_linear_current(In) ; 
        }
    }  
    
}

void SetConductancesForSim(Circuit &CKTIn , double deltatime){
    for(int i = 0 ; i < CKTIn.get_components().size() ; i++){
        if(dynamic_cast<Capacitor*>(CKTIn.get_components().at(i))){ //DETERMINES THAT THE COMPONENT IS A CAPACITOR
           ((Capacitor*)(CKTIn.get_components()[i]))->set_conductance(deltatime); 
        }
        else if (dynamic_cast<Inductor*>(CKTIn.get_components().at(i)))
        {
            ((Inductor*)(CKTIn.get_components()[i]))->set_conductance(deltatime); 
        }
    }  

}

void TransientAnalysis(Circuit &CKTIn , double TimePeriod , int TimeStep){
    bool isLinear;
    isLinear = IsLinear(CKTIn);
    double CurrentTime = 0;
    std::cout <<"timestep is "<< TimeStep << std::endl;
    double deltaTime = (TimePeriod/TimeStep);
    remove("output_voltage.txt");
    fstream myfile ("output_voltage.txt");
    remove("output_current.txt");
    fstream myfile2 ("output_current.txt");
    SetConductancesForSim(CKTIn,deltaTime); //SETS THE CONDUCTANCE FOR EACH INDUCTOR AND CAP THAT DEPENDS ON DELTA TIME (BUT REMAINS CONSTANT THROUGH SIM)
    for(double i = 0 ; i <= TimeStep ; i++){
        UpdateNodeVoltages(CKTIn , CurrentTime,isLinear); 
        CurrentTime = CurrentTime + deltaTime;
        //POSSIBLE SHIFT ERROR MAY OCCUR. 
    }
}
