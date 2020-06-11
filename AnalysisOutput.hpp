#ifndef ANALYSIS_OUTPUT_HPP
#define ANALYSIS_OUTPUT_HPP

#include <vector>
#include <iostream>
#include <string>
#include "Component.hpp"
#include <memory> 
#include "Circuit.hpp"
#include <fstream>
#include "TransientSolver.hpp"
#include "KCLSolver.hpp"
using namespace std;

double Current_at_voltage_component(Circuit &circuit, Voltage_Component* component, int _node = -1){
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
                current += Current_at_voltage_component(circuit,dynamic_cast<Voltage_Component*>(_component),next_node);
            }
        }
    }
    return current;
}

void Calculate_currents(Circuit &circuit){
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
        component->current = Current_at_voltage_component(circuit,component);
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
void NodeVoltagesToFile(Circuit& CKTIn , double CurrentTime = -1){
    bool OP = false;
    if(CurrentTime == -1){
        OP = true;
    }
    fstream myfile;
    myfile.open("output_voltage.txt",fstream::app);
    fstream myfile2;
    myfile2.open("output_current.txt",fstream::app);
    if (myfile.is_open())
    {
        if(!OP){
        myfile << CurrentTime << "," ;
        }
        for(int i = 0; i < CKTIn.get_voltages().size()-1; i++) {
            myfile << CKTIn.get_voltages().at(i) << ",";
        }
        myfile << CKTIn.get_voltages().at(CKTIn.get_voltages().size()-1);
        myfile << "\n";
        myfile.close();
    }else cout << "Unable to open voltage file";
    
    if(myfile2.is_open()){
        if(CurrentTime <= 0){
            for(int i = 0; i < CKTIn.get_components().size()-1; i++){
                if(dynamic_cast<BJT*>(CKTIn.get_components().at(i))){
                    myfile2 << CKTIn.get_components().at(i)->get_name() << "C" << ", ";
                    myfile2 << CKTIn.get_components().at(i)->get_name() << "B" << ", ";
                    myfile2 << CKTIn.get_components().at(i)->get_name() << "E" << ", ";
                }else{
                    myfile2 << CKTIn.get_components().at(i)->get_name() << ", ";
                }
            }
            if(dynamic_cast<BJT*>(CKTIn.get_components().at(CKTIn.get_components().size()-1))){
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name() << "C" << ", ";
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name() << "B" << ", ";
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name() << "E";
            }else{
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name();
            }
            myfile2 << "\n";
        }
        if(!OP){
            myfile2 << CurrentTime << "," ;
        }
        for(int i = 0; i < CKTIn.get_components().size()-1; i++){
                if(dynamic_cast<BJT*>(CKTIn.get_components().at(i))){
                    std::vector<double> current = ((BJT*)CKTIn.get_components().at(i))->currents;
                    myfile2 << current[0] << ", ";
                    myfile2 << current[1] << ", ";
                    myfile2 << current[2] << ", ";
                }else{
                
                    myfile2 << GetCurrent(CKTIn.get_components().at(i)) << ", ";
                }
        }
        if(dynamic_cast<BJT*>(CKTIn.get_components().at(CKTIn.get_components().size()-1))){
            std::vector<double> current = ((BJT*)CKTIn.get_components().at(CKTIn.get_components().size()-1))->get_current(CKTIn.get_voltages());
            myfile2 << current[0] << ", ";
            myfile2 << current[1] << ", ";
            myfile2 << current[2];
        }else{
            myfile2 << GetCurrent(CKTIn.get_components().at(CKTIn.get_components().size()-1));
        }
        myfile2 << "\n";
  }
  else cout << "Unable to open current file";
}

#endif