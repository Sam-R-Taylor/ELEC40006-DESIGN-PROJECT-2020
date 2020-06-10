#ifndef TRANSIENT_ANALYSIS_HPP
#define TRANSIENT_ANALYSIS_HPP

#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <string>
#include "Component.hpp"
#include <memory> 
#include "Circuit.hpp"
#include <fstream>
#include "TransientSolver.hpp"
#include "AnalysisOutput.hpp"
#include "KCLSolver.hpp"


using namespace std;
using Eigen::MatrixXd;

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
    Calculate_currents(CKTIn);
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
    //std::cout <<"timestep is "<< TimeStep << std::endl;
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
#endif