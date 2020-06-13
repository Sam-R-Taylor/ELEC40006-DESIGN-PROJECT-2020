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
#include "SparseKCLSolver.hpp"
#include "AnalysisOutput.hpp"


using namespace std;
using Eigen::MatrixXd;

void UpdateIntegralComponents(Circuit &CKTIn){
    //LOOPS THROUGH TO UPDATE INTEGRAL COMPONENTS
    
}

void UpdateNodeVoltages(Circuit &CKTIn , double CurrentTime,bool isLinear){
    //All this function does is update the integrals of each component and then passes the updates CKT to Transient Solver. 
    //Update Capacitors and inductors integrals
    double Vn;
    double In;
    for(int i = 0 ; i < static_cast<int>(CKTIn.get_components().size()) ; i++){
        if(dynamic_cast<Capacitor*>(CKTIn.get_components().at(i))){ //DETERMINES THAT THE COMPONENT IS A CAPACITOR
            Vn = (CKTIn.get_voltages()[CKTIn.get_components()[i]->get_anode()]) - 
                ((CKTIn.get_voltages()[CKTIn.get_components()[i]->get_cathode()]));
            ((Capacitor*)(CKTIn.get_components()[i]))->set_linear_current(Vn);
            //cout << ((Capacitor*)(CKTIn.get_components()[i]))->get_linear_current() << endl;
            //cout << ((Capacitor*)(CKTIn.get_components()[i]))->get_conductance() << endl;

        }
        else if (dynamic_cast<Inductor*>(CKTIn.get_components().at(i)))
        {
            In = GetCurrent(CKTIn.get_components()[i]); //GETCURRENT WILL BE IMPLEMENTED BY MAX ,
            ((Inductor*)(CKTIn.get_components()[i]))->set_linear_current(In) ; 
        }
    }
    //LOOPS THROUGH TO UPDATE NOT INTEGRAL COMPONENTS I.E AC SOURCE
    //IMPLEMENTATION ONLY WORKS FOR RLC 
    for(int i = 0 ; i < static_cast<int>(CKTIn.get_components().size()) ; i++){
        if (dynamic_cast<AC_Voltage_Source*>(CKTIn.get_components().at(i))){
            ((AC_Voltage_Source*)(CKTIn.get_components()[i]))->Set_Voltage(CurrentTime);
        }
    }
    //update all the voltages
    if(isLinear){
        Sparse_Matrix_solver(CKTIn);
    }else{
        TransientSolver(CKTIn);
    }
    //update all the currents
    Calculate_currents(CKTIn);
    //print the voltages and currents to files
    NodeVoltagesToFile(CKTIn,CurrentTime);   
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
/*
double CalculateTimeStep(vector<double> v, vector<double> voltages, double min_timestep, double current_timestep){
    double timestep = 0;
    double maxgradient = 0;
    for(int i = 0; i < voltages.size(); i++){
        double gradient = abs(v[i] - voltages[i])/current_timestep;
        if(gradient > maxgradient){
            maxgradient = gradient;
            timestep = ((1/gradient)) * min_timestep * 10; 
        }
    }
    if(timestep < pow(10,-5)){
        timestep = pow(10,-5);
    }
    if(timestep > min_timestep){
        timestep = min_timestep;
    }
    return timestep;
}*/
void TransientAnalysis(Circuit &CKTIn , double TimePeriod , int TimeStep){
    bool isLinear;
    isLinear = IsLinear(CKTIn);
    double CurrentTime = 0;
    //std::cout <<"timestep is "<< TimeStep << std::endl;
    double deltaTime = (TimePeriod/TimeStep);
    //remove("output_voltage.txt");
    //fstream myfile ("output_voltage.txt");
    //remove("output_current.txt");
    //fstream myfile2 ("output_current.txt");
    //cout << deltaTime << endl;
     //SETS THE CONDUCTANCE FOR EACH INDUCTOR AND CAP THAT DEPENDS ON DELTA TIME (BUT REMAINS CONSTANT THROUGH SIM)
    //for(double i = 1 ; i <= TimeStep ; i++){
    //int i = 1;
    double step = deltaTime;//pow(10,-5);
    bool run = true;
    SetConductancesForSim(CKTIn,step);
    //std::vector<double> oldvoltages;
    while(run){
        //CurrentTime = CurrentTime + deltaTime;
        CurrentTime = CurrentTime + step;
        //oldvoltages = CKTIn.get_voltages();
        UpdateNodeVoltages(CKTIn, CurrentTime, isLinear); 
        //vector<double> v = CKTIn.get_voltages();
        //double newstep = CalculateTimeStep(v, oldvoltages, deltaTime, step);
        //if(newstep != step){
        //    step = newstep;
        //    SetConductancesForSim(CKTIn,step);
        //}
        if(CurrentTime >= TimePeriod){
            run = false;
        }
    }
}


#endif